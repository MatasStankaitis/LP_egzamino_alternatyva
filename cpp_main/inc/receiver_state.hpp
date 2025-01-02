#pragma once

#include "caf/all.hpp"
#include "actor_traits.hpp"
#include "movie.hpp"

class ReceiverState
{
public:
    ReceiverState(receiver_actor::pointer_view, results_collector_actor);
    ~ReceiverState();

    receiver_actor::behavior_type make_behavior();

private:
    receiver_actor::pointer_view self;
    results_collector_actor results_collector_;
    int sockfd_ = -1;
    int client_fd_ = -1;
};