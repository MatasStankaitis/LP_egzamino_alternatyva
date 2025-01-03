#include <caf/all.hpp>

#include "actor_traits.hpp"
#include "movie.hpp"

class ResultsCollectorState {
 public:
  ResultsCollectorState(results_collector_actor::pointer_view ptr,
                        printer_actor printer, int worker_count);
  ~ResultsCollectorState() = default;

  results_collector_actor::behavior_type make_behavior();

 private:
  results_collector_actor::pointer_view self;
  printer_actor printer_;
  movie_list results;
  int finish_messages_count = 0;
  int worker_count_;
};