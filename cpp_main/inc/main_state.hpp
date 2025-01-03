#pragma once

#include <caf/all.hpp>
#include <caf/json_reader.hpp>
#include "actor_traits.hpp"

class MainState
{
public:
    MainState(main_actor::pointer_view ptr, worker_list workers, sender_actor sender, results_collector_actor results_collector, printer_actor printer, std::string_view input_file);
    ~MainState() = default;

    main_actor::behavior_type make_behavior();

private:
    int read_input_file();
    void distribute_workload();
    void terminate_workers();

    main_actor::pointer_view self;
    movie_list workload;
    worker_list workers_;
    sender_actor sender_;
    results_collector_actor results_collector_;
    printer_actor printer_;
    std::string input_file_;
};
