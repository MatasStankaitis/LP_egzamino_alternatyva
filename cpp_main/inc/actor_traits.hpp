#pragma once

#include <caf/all.hpp>
#include "caf_config.hpp"

struct main_actor_trait
{
    using signatures = caf::type_list<
        caf::result<void>(start_atom)>;
};

struct worker_actor_trait
{
    using signatures = caf::type_list<
        caf::result<void>(caf::put_atom, movie),
        caf::result<void>(finish_atom)>;
};

struct sender_actor_trait
{
    using signatures = caf::type_list<
        caf::result<void>(caf::put_atom, movie_list)>;
};

struct results_collector_actor_trait
{
    using signatures = caf::type_list<
        caf::result<void>(caf::put_atom, movie),
        caf::result<void>(finish_atom)>;
};

struct receiver_actor_trait
{
    using signatures = caf::type_list<
        caf::result<void>(start_atom)>;
};

struct printer_actor_trait
{
    using signatures = caf::type_list<
        caf::result<void>(caf::put_atom, movie_list),
        caf::result<void>(initial_data_atom, movie_list)>;
};

using main_actor = caf::typed_actor<main_actor_trait>;
using worker_actor = caf::typed_actor<worker_actor_trait>;
using sender_actor = caf::typed_actor<sender_actor_trait>;
using results_collector_actor = caf::typed_actor<results_collector_actor_trait>;
using receiver_actor = caf::typed_actor<receiver_actor_trait>;
using printer_actor = caf::typed_actor<printer_actor_trait>;

using worker_list = std::vector<worker_actor>;