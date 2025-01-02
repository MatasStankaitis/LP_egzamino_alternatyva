#pragma once

#include <caf/all.hpp>
#include <caf/json_writer.hpp>
#include "actor_traits.hpp"

class SenderState
{
public:
    SenderState(sender_actor::pointer_view ptr);
    ~SenderState();
    sender_actor::behavior_type make_behavior();

private:
    int sockfd_ = -1;
    sender_actor::pointer_view self;
    caf::json_writer writer;
};