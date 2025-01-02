#include <caf/all.hpp>
#include "movie.hpp"
#include "actor_traits.hpp"

class WorkerState
{
public:
    WorkerState(worker_actor::pointer_view ptr, results_collector_actor results_collector, int worker_id);
    ~WorkerState() = default;

    worker_actor::behavior_type make_behavior();

private:
    results_collector_actor results_collector_;
    int wid;
    worker_actor::pointer_view self;
};