#pragma once

#include <caf/all.hpp>
#include <caf/json_reader.hpp>
#include "actor_traits.hpp"

class MainState
{
public:
    MainState(main_actor::pointer_view ptr, worker_list workers, sender_actor sender);
    ~MainState() = default;

    main_actor::behavior_type make_behavior();

private:
    int read_input_file(std::string_view file_path);
    void distribute_workload();

    main_actor::pointer_view self;
    movie_list workload;
    worker_list workers_;
    sender_actor sender_;
};
