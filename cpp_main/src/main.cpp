/**
 * @file main.cpp
 *
 * Matas Stankaitis IFF-2/5
 * LP egzamino alternatyva 2025
 *
 * 1 worker procesas - 6000 ms
 * 4 worker procesai - 3000 ms
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "caf/all.hpp"
#include "main_state.hpp"
#include "printer_state.hpp"
#include "receiver_state.hpp"
#include "results_collector_state.hpp"
#include "sender_state.hpp"
#include "worker_state.hpp"

using namespace caf;

static constexpr int PORT = 9999;
static constexpr char IP[] = "127.0.0.1";

int open_tcp_socket() {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "Error: failed to create socket\n";
    return -1;
  }
  sockaddr_in serv_addr{};
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0) {
    std::cerr << "Invalid address/ Address not supported\n";
    close(sockfd);
    return -1;
  }
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cerr << "Connection to Python server failed.\n";
    close(sockfd);
    return -1;
  }
  std::cout << "Successfully connected to " << IP << ":" << PORT << "\n";
  return sockfd;
}

void caf_main(caf::actor_system &sys, [[maybe_unused]] const config &cfg) {
  auto start_time = std::chrono::steady_clock::now();
  int socket_fd = open_tcp_socket();
  int worker_count = get_or(cfg, "worker-count", 8);
  std::string input_file = get_or(cfg, "input-file", "data/test4.json");
  std::string output_file = get_or(cfg, "output-file", "data/output.txt");

  worker_list worker_actors;
  scoped_actor self{sys};

  auto printer_actor =
      sys.spawn(caf::actor_from_state<PrinterState>, output_file);
  auto results_collector_actor =
      sys.spawn(caf::actor_from_state<ResultsCollectorState>, printer_actor,
                worker_count);
  auto receiver_actor = sys.spawn(caf::actor_from_state<ReceiverState>,
                                  results_collector_actor, socket_fd);
  self->mail(start_atom_v).send(receiver_actor);

  for (int i = 0; i < worker_count; i++) {
    worker_actors.emplace_back(sys.spawn(caf::actor_from_state<WorkerState>,
                                         results_collector_actor, i));
  }
  auto sender_actor = sys.spawn(caf::actor_from_state<SenderState>, socket_fd);
  auto main_actor =
      sys.spawn(caf::actor_from_state<MainState>, worker_actors, sender_actor,
                results_collector_actor, printer_actor, input_file);
  self->mail(start_atom_v).send(main_actor);

  self->await_all_other_actors_done();
  self->println("All actors finished execution");

  auto end_time = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                      end_time - start_time)
                      .count();

  std::cout << "Total execution time with " << worker_count
            << " C++ workers: " << duration << "ms\n";
}

CAF_MAIN(id_block::exam_app)