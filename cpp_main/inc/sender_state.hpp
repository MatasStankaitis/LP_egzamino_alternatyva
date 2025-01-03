#pragma once

#include <caf/all.hpp>
#include <caf/json_writer.hpp>

#include "actor_traits.hpp"

class SenderState {
 public:
  SenderState(sender_actor::pointer_view ptr, int socket_fd);
  ~SenderState() = default;
  sender_actor::behavior_type make_behavior();

 private:
  sender_actor::pointer_view self;
  int socket_fd_ = -1;
  caf::json_writer writer;
};