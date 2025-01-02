#pragma once

#include <caf/all.hpp>
#include <string>
#include <vector>

#include "actor_interfaces.hpp"

class MainActor
{
public:
    explicit MainActor(caf::event_based_actor *ptr, std::vector<caf::actor> worker_actors, caf::actor results_collector,
                       caf::actor sender, std::string_view input_file_path);

    caf::behavior make_behavior();

private:
    std::vector<caf::actor> worker_actors_;
    caf::actor results_collector_;
    caf::actor sender_;
    std::string_view input_file_path_;

    caf::event_based_actor *self = nullptr;

    void read_input_file(std::string_view file_path, std::vector<std::string> &workload);
    void distribute_workload(const std::vector<std::string> &workload);
};
