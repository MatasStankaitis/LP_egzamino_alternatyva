#include "results_collector_state.hpp"

ResultsCollectorState::ResultsCollectorState(results_collector_actor::pointer_view ptr, printer_actor printer, int worker_count)
    : self(ptr), printer_(printer), worker_count_(worker_count)
{
    // nop
}

results_collector_actor::behavior_type ResultsCollectorState::make_behavior()
{
    return {
        [this](caf::put_atom, movie data)
        {
            self->println("results collector actor received {}", data);
            results.push_back(data);
            self->println("results collector actor results size {}", results.size());
        },
        [this](finish_atom)
        {
            self->println("results collector actor received finish_atom");
            cnt++;
            if (cnt == worker_count_ + 1)
            {
                self->println("results collector actor received all finish_atoms");

                // Count occurrences of each movie ID
                std::unordered_map<int, std::vector<movie>> id_movies;
                for (const auto &m : results)
                {
                    id_movies[m.id].push_back(m);
                }

                // Process only movies that appear twice
                movie_list final_results;
                for (const auto &pair : id_movies)
                {
                    if (pair.second.size() == 2)
                    {
                        // Merge hashes from both entries
                        movie merged = pair.second[0];
                        if (merged.hash1 == 0)
                        {
                            merged.hash1 = pair.second[1].hash1;
                        }
                        if (merged.hash2 == 0)
                        {
                            merged.hash2 = pair.second[1].hash2;
                        }
                        final_results.push_back(merged);
                        self->println("Merged movie: {} (hash1: {}, hash2: {})",
                                      merged.title, merged.hash1, merged.hash2);
                    }
                }

                self->println("Results collector actor final results size: {}", final_results.size());
                self->mail(caf::put_atom_v, final_results).send(printer_);
            }
        },
    };
}