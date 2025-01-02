#include <caf/all.hpp>
#include "movie.hpp"
#include "actor_traits.hpp"

class results_collector_state
{
public:
    results_collector_state(results_collector_actor::pointer_view ptr)
        : self(ptr)
    {
        // nop
    }

    results_collector_actor::behavior_type make_behavior()
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
                cnt++;
                if (cnt == 2)
                {
                    self->println("results collector actor received both finish_atoms");
                    std::unordered_map<int, int> id_count;
                    for (const auto &m : results)
                    {
                        id_count[m.id]++;
                    }
                    movie_list duplicates;
                    for (const auto &m : results)
                    {
                        if (id_count[m.id] > 1)
                        {
                            duplicates.push_back(m);
                        }
                    }
                    results = std::move(duplicates);
                    self->println("results collector actor filtered results size {}", results.size());
                }
            },
        };
    }

    results_collector_actor::pointer_view self;
    movie_list results;
    int cnt = 0;
};