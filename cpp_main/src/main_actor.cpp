// #include <iostream>
// #include <fstream>
// #include <caf/all.hpp>

// #include "main_actor.hpp"

// MainActor::MainActor(caf::event_based_actor *ptr, std::vector<caf::actor> worker_actors, caf::actor results_collector,
//                      caf::actor sender, std::string_view input_file_path)
//     : self(ptr), input_file_path_(input_file_path), worker_actors_(std::move(worker_actors)),
//       results_collector_(results_collector), sender_(sender)
// {
//     // nop
// }

// caf::behavior MainActor::make_behavior()
// {
//     return {
//         [this](add_atom)
//         {
//             std::vector<std::string> workload;
//             read_input_file(input_file_path_, workload);
//             distribute_workload(workload);
//         },
//     };
// }
