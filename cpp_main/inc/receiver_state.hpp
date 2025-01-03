#pragma once

#include "actor_traits.hpp"
#include "caf/all.hpp"

class ReceiverState {
 public:
  ReceiverState(receiver_actor::pointer_view ptr,
                results_collector_actor results_collector);
  ~ReceiverState() = default;

  receiver_actor::behavior_type make_behavior();

 private:
  receiver_actor::pointer_view self;
  results_collector_actor results_collector_;
};