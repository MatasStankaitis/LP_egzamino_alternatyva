#include "caf/all.hpp"
#include <cstdlib>
#include "main_state.hpp"
#include "sender_state.hpp"
#include "worker_state.hpp"
#include "results_collector_state.hpp"
#include "printer_state.hpp"
#include "receiver_state.hpp"

using namespace caf;

void caf_main(caf::actor_system &sys)
{
    scoped_actor self{sys};
    auto results_collector_actor = sys.spawn(caf::actor_from_state<results_collector_state>);
    // auto receiver_actor = sys.spawn(caf::actor_from_state<ReceiverState>, results_collector_actor);
    // self->mail(start_atom_v).send(receiver_actor);

    int worker_count;
    worker_list worker_actors;

    for (int i = 0; i < worker_count; i++)
    {
        worker_actors.emplace_back(sys.spawn(caf::actor_from_state<WorkerState>, results_collector_actor, i));
    }

    auto sender_actor = sys.spawn(caf::actor_from_state<SenderState>);
    auto main_actor = sys.spawn(caf::actor_from_state<MainState>, worker_actors, sender_actor);
    self->mail(start_atom_v).send(main_actor);
}

CAF_MAIN(id_block::exam_app)