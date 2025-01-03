#include "worker_state.hpp"

static constexpr int ITERATIONS = 3000000;

WorkerState::WorkerState(worker_actor::pointer_view ptr, results_collector_actor results_collector, int worker_id)
    : self(ptr), results_collector_(results_collector), wid(worker_id)
{
    // nop
}

worker_actor::behavior_type WorkerState::make_behavior()
{
    return {
        [this](caf::put_atom, movie data)
        {
            if (first_message_)
            {
                start_time_ = std::chrono::steady_clock::now();
                first_message_ = false;
            }

            self->println("worker {} actor received {}", wid, data);

            uint64_t hash = 0;
            for (int i = 0; i < ITERATIONS; ++i)
            {
                hash = std::hash<uint64_t>{}(hash + data.year + data.rating);
            }
            if (data.year >= 2000)
            {
                data.hash1 = hash;
                self->mail(caf::put_atom_v, data).send(results_collector_);
            }
        },
        [this](finish_atom)
        {
            auto end_time = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_).count();

            self->println("worker {} actor finished processing in {} ms", wid, duration);
            self->mail(finish_atom_v).send(results_collector_);
            self->quit(caf::exit_reason::user_shutdown);
        },
    };
}