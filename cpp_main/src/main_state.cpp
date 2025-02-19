#include "main_state.hpp"

MainState::MainState(main_actor::pointer_view ptr, const worker_list &workers,
                     sender_actor sender,
                     results_collector_actor results_collector,
                     printer_actor printer, std::string_view input_file)
    : self(ptr),
      workers_(workers),
      sender_(sender),
      results_collector_(results_collector),
      printer_(printer),
      input_file_(input_file) {}

main_actor::behavior_type MainState::make_behavior() {
  return {
      [this](start_atom) {
        self->println(caf::term::red, "[Main actor] received start_atom");
        if (read_input_file() != EXIT_SUCCESS) {
          self->println(
              caf::term::bold_red,
              "[Main actor] failed to read input file. Terminating... ");
          terminate_workers();
          self->mail(finish_atom_v).send(sender_);
          self->quit(caf::exit_reason::user_shutdown);
          return;
        }
        self->mail(caf::put_atom_v, workload).send(sender_);
        self->mail(initial_data_atom_v, workload).send(printer_);
        distribute_workload();

        terminate_workers();
        self->println(caf::term::red, "[Main actor] finished. Terminating...");
        self->quit(caf::exit_reason::user_shutdown);
      },
  };
}

int MainState::read_input_file() {
  caf::json_reader reader;
  if (!reader.load_file(input_file_.c_str())) {
    self->println(caf::term::bold_red,
                  "[Main actor] failed to load the user list: {}\n",
                  reader.get_error());
    return EXIT_FAILURE;
  }

  /* Deserializes json to typeof(workload) */
  if (!reader.apply(workload)) {
    self->println(caf::term::bold_red,
                  "[Main actor] failed to deserialize the user list: {}\n",
                  reader.get_error());
    return EXIT_FAILURE;
  }

  if (workload.size() == 0) {
    self->println(caf::term::bold_red,
                  "[Main actor] no movies found in the input file\n");
    return EXIT_FAILURE;
  }

  for (int i = 1; i <= workload.size(); i++) {
    workload[i - 1].id = i;
  }
  return EXIT_SUCCESS;
}

void MainState::distribute_workload() {
  for (int i = 0; i < workload.size(); i++) {
    self->mail(caf::put_atom_v, workload[i])
        .send(workers_[i % workers_.size()]);
  }
}

void MainState::terminate_workers() {
  for (const auto &worker : workers_) {
    self->mail(finish_atom_v).send(worker);
  }
}