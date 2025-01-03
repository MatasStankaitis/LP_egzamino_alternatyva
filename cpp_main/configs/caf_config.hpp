#pragma once

#include <caf/all.hpp>

#include "movie.hpp"

CAF_BEGIN_TYPE_ID_BLOCK(exam_app, caf::first_custom_type_id)

CAF_ADD_TYPE_ID(exam_app, (Movie))
CAF_ADD_TYPE_ID(exam_app, (movie_list))
CAF_ADD_ATOM(exam_app, start_atom)
CAF_ADD_ATOM(exam_app, finish_atom)
CAF_ADD_ATOM(exam_app, size_atom)
CAF_ADD_ATOM(exam_app, initial_data_atom)

CAF_END_TYPE_ID_BLOCK(exam_app)

struct config : caf::actor_system_config {
  config() {
    opt_group{custom_options_, "global"}
        .add<int>("worker-count,w", "number of worker actors")
        .add<std::string>("input-file,i", "input file path")
        .add<std::string>("output-file,o", "output file path");
  }
};