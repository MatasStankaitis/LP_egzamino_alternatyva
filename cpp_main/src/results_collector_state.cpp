#include "results_collector_state.hpp"

ResultsCollectorState::ResultsCollectorState(
    results_collector_actor::pointer_view ptr, printer_actor printer,
    int worker_count)
    : self(ptr), printer_(printer), worker_count_(worker_count) {
  // nop
}

results_collector_actor::behavior_type ResultsCollectorState::make_behavior() {
  return {
      [this](caf::put_atom, Movie data) {
        self->println(caf::term::blue, "[Results Collector] received {}", data);
        results.push_back(data);
        self->println(caf::term::blue,
                      "[Results Collector] buffer items count {}",
                      results.size());
      },
      [this](finish_atom) {
        self->println(caf::term::blue,
                      "[Results Collector] received finish_atom");
        finish_messages_count++;
        /* If results collector received finish message from all workers and
         * from receiver */
        if (finish_messages_count == worker_count_ + 1) {
          self->println(caf::term::blue,
                        "[Results Collector] received all finish_atoms");

          std::unordered_map<int, std::vector<Movie>> id_movies;
          for (const auto& m : results) {
            id_movies[m.id].push_back(m);
          }

          /* Merge movies with the same id */
          movie_list final_results;
          for (const auto& [id, movies] : id_movies) {
            if (movies.size() != 2) {
              continue;
            }
            /* Merge hashes from both entries */
            Movie merged = movies[0];
            if (merged.hash1 == 0) {
              merged.hash1 = movies[1].hash1;
            }
            if (merged.hash2 == 0) {
              merged.hash2 = movies[1].hash2;
            }
            final_results.push_back(merged);
          }

          self->println(caf::term::blue,
                        "[Results Collector] final results size: {}",
                        final_results.size());
          self->mail(caf::put_atom_v, final_results).send(printer_);
          self->println(caf::term::blue, "[Results Collector] terminating...");
          self->quit(caf::exit_reason::user_shutdown);
        }
      },
  };
}