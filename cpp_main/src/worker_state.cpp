#include "worker_state.hpp"

static constexpr int ITERATIONS = 7000000;

WorkerState::WorkerState(worker_actor::pointer_view ptr,
                         results_collector_actor results_collector,
                         int worker_id)
    : self(ptr), results_collector_(results_collector), wid(worker_id) {
  // nop
}

static inline uint64_t calculate_hash(const Movie &data) {
  uint64_t hash = 0;
  for (int i = 0; i < ITERATIONS; ++i) {
    hash = std::hash<uint64_t>{}(hash + data.year +
                                 static_cast<uint64_t>(data.rating));
  }
  return hash;
}

worker_actor::behavior_type WorkerState::make_behavior() {
  return {
      [this](caf::put_atom, Movie data) {
        if (first_message_) {
          start_time_ = std::chrono::steady_clock::now();
          self->println(caf::term::green,
                        "[Worker {}] received put_atom. Starting processing...",
                        wid);
          first_message_ = false;
        }
        self->println(caf::term::green, "[Worker {}] processing {}", wid, data);

        uint64_t hash = calculate_hash(data);

        if (data.year >= 2000) {
          data.hash1 = hash;
          self->mail(caf::put_atom_v, data).send(results_collector_);
        }
      },

      [this](finish_atom) {
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                            end_time - start_time_)
                            .count();
        if (first_message_) {
          duration = 0;
        }

        self->println(caf::term::green, "[Worker {}] finished work in {} ms",
                      wid, duration);
        self->mail(finish_atom_v).send(results_collector_);
        self->println(caf::term::green, "[Worker {}] terminating...", wid);
        self->quit(caf::exit_reason::user_shutdown);
      },
  };
}