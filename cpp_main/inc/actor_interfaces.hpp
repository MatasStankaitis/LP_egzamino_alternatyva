// #pragma once
// #include <caf/all.hpp>

// CAF_BEGIN_TYPE_ID_BLOCK(my_project, caf::first_custom_type_id)
// CAF_ADD_ATOM(my_project, start_atom)
// CAF_ADD_ATOM(my_project, task_atom)
// CAF_ADD_ATOM(my_project, result_atom)
// CAF_ADD_ATOM(my_project, send_atom)
// CAF_ADD_ATOM(my_project, print_atom)
// CAF_END_TYPE_ID_BLOCK(my_project)
// // Worker Actor Interface
// using worker_actor = caf::typed_actor<
//     caf::result<result_atom, std::string>(task_atom, std::string)>;

// // Results Collector Actor Interface
// using results_collector_actor = caf::typed_actor<
//     caf::reacts_to<result_atom, std::string>>;

// // Sender Actor Interface
// using sender_actor = caf::typed_actor<
//     caf::reacts_to<send_atom, std::string>>;

// // Receiver Actor Interface
// using receiver_actor = caf::typed_actor<
//     caf::result<result_atom, std::string>(receive_atom, std::string)>;

// // Printer Actor Interface
// using printer_actor = caf::typed_actor<
//     caf::reacts_to<print_atom>>;

// // Main Actor Interface
// using main_actor = caf::typed_actor<
//     caf::reacts_to<start_atom>>;