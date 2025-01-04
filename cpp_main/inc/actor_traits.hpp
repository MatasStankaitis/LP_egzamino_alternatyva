#pragma once

#include <caf/all.hpp>

#include "caf_config.hpp"

/**
 * @brief Actor traits for the main actor.
 *
 * The main actor accepts a start atom message to start execution.
 */
struct main_actor_trait {
  using signatures = caf::type_list<caf::result<void>(start_atom)>;
};

/**
 * @brief Actor traits for the worker actor.
 *
 * The worker actor accepts two types of messages:
 * 1. A put atom message with a movie object to process.
 * 2. A finish atom message to signal the end of execution.
 */
struct worker_actor_trait {
  using signatures = caf::type_list<caf::result<void>(caf::put_atom, Movie),
                                    caf::result<void>(finish_atom)>;
};

/**
 * @brief Actor traits for the sender actor.
 *
 * The sender actor accepts a put atom message with whole movie list to send to
 * Python application over the network.
 */
struct sender_actor_trait {
  using signatures =
      caf::type_list<caf::result<void>(caf::put_atom, movie_list),
                     caf::result<void>(finish_atom)>;
};

/**
 * @brief Actor traits for the results collector actor.
 *
 * The results collector actor accepts two types of messages:
 * 1. A put atom message with a movie object to collect.
 * 2. A finish atom message to signal the end of execution.
 */
struct results_collector_actor_trait {
  using signatures = caf::type_list<caf::result<void>(caf::put_atom, Movie),
                                    caf::result<void>(finish_atom)>;
};

/**
 * @brief Actor traits for the receiver actor.
 *
 * The receiver actor accepts a start atom message to start listening for the
 * data from Python application over the network.
 */
struct receiver_actor_trait {
  using signatures = caf::type_list<caf::result<void>(start_atom)>;
};

/**
 * @brief Actor traits for the printer actor.
 *
 * The printer actor accepts two types of messages:
 * 1. A put atom message with a movie list to print.
 * 2. An initial data atom message with a movie list to print.
 */
struct printer_actor_trait {
  using signatures =
      caf::type_list<caf::result<void>(caf::put_atom, movie_list),
                     caf::result<void>(initial_data_atom, movie_list)>;
};

/* type alias for statically-typed actors */
using main_actor = caf::typed_actor<main_actor_trait>;
using worker_actor = caf::typed_actor<worker_actor_trait>;
using sender_actor = caf::typed_actor<sender_actor_trait>;
using results_collector_actor = caf::typed_actor<results_collector_actor_trait>;
using receiver_actor = caf::typed_actor<receiver_actor_trait>;
using printer_actor = caf::typed_actor<printer_actor_trait>;

using worker_list = std::vector<worker_actor>;