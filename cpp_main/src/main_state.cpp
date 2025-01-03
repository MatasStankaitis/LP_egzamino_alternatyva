#include "main_state.hpp"

MainState::MainState(main_actor::pointer_view ptr, worker_list workers, sender_actor sender, results_collector_actor results_collector)
    : self(ptr), workers_(workers), sender_(sender), results_collector_(results_collector)
{
    self->println("main actor created");
}

main_actor::behavior_type MainState::make_behavior()
{
    return {
        [this](start_atom)
        {
            self->println("main actor received start_atom");
            int ret = read_input_file("data/test.json");
            if (ret != EXIT_SUCCESS)
            {
                self->println("Failed to read input file. Terminating... ");
                self->quit(caf::exit_reason::user_shutdown);
                return;
            }
            self->mail(caf::put_atom_v, workload).send(sender_);
            distribute_workload();
            terminate_workers();
        },
    };
}

int MainState::read_input_file(std::string_view file_path)
{
    self->println("main actor received read_input_file");
    caf::json_reader reader;
    if (!reader.load_file(file_path.data()))
    {
        self->println("*** failed to load the user list: {}\n", reader.get_error());
        return EXIT_FAILURE;
    }

    if (!reader.apply(workload))
    {
        self->println("*** failed to deserialize the user list: {}\n", reader.get_error());
        return EXIT_FAILURE;
    }

    if (workload.size() == 0)
    {
        self->println("*** no movies found in the input file\n");
        return EXIT_FAILURE;
    }

    for (int i = 1; i <= workload.size(); i++)
    {
        workload[i - 1].id = i;
    }
    return EXIT_SUCCESS;
}

void MainState::distribute_workload()
{
    for (int i = 0; i < workload.size(); i++)
    {
        self->mail(caf::put_atom_v, workload[i]).send(workers_[i % workers_.size()]);
    }
}

void MainState::terminate_workers()
{
    for (auto &worker : workers_)
    {
        self->mail(finish_atom_v).send(worker);
    }
}