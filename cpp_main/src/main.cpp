#include "caf/all.hpp"
#include <cstdlib>
#include "main_state.hpp"
#include "sender_state.hpp"
#include "worker_state.hpp"
#include "results_collector_state.hpp"
#include "printer_state.hpp"
#include "receiver_state.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

using namespace caf;

static constexpr int PORT = 9999;
static constexpr char IP[] = "127.0.0.1";

// The user must set up the socket and connect:
int open_tcp_socket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error: failed to create socket\n";
        return -1;
    }
    // Connect
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported\n";
        close(sockfd);
        return -1;
    }
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection to Python server failed.\n";
        close(sockfd);
        return -1;
    }
    std::cout << "Successfully connected to " << IP << ":" << PORT << "\n";
    return sockfd;
}

void caf_main(caf::actor_system &sys, [[maybe_unused]] const config &cfg)
{
    int socket_fd = open_tcp_socket();
    int worker_count = get_or(cfg, "worker-count", 8);
    std::string input_file = get_or(cfg, "input-file", "data/test4.json");
    std::string output_file = get_or(cfg, "output-file", "data/output.txt");

    worker_list worker_actors;
    scoped_actor self{sys};

    auto printer_actor = sys.spawn(caf::actor_from_state<PrinterState>, output_file);
    auto results_collector_actor = sys.spawn(caf::actor_from_state<ResultsCollectorState>, printer_actor, worker_count);
    auto receiver_actor = sys.spawn(caf::actor_from_state<ReceiverState>, results_collector_actor, socket_fd);
    self->mail(start_atom_v).send(receiver_actor);

    for (int i = 0; i < worker_count; i++)
    {
        worker_actors.emplace_back(sys.spawn(caf::actor_from_state<WorkerState>, results_collector_actor, i));
    }
    auto sender_actor = sys.spawn(caf::actor_from_state<SenderState>, socket_fd);

    auto main_actor = sys.spawn(caf::actor_from_state<MainState>, worker_actors, sender_actor, results_collector_actor, printer_actor, input_file);
    self->mail(start_atom_v).send(main_actor);
}

CAF_MAIN(id_block::exam_app)