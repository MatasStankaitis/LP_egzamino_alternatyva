#include "receiver_state.hpp"
#include <caf/json_reader.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

ReceiverState::ReceiverState(receiver_actor::pointer_view ptr, results_collector_actor results_collector)
    : self(ptr), results_collector_(results_collector)
{
    // // Create socket
    // sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    // if (sockfd_ < 0)
    // {
    //     self->println("Error creating socket");
    //     return;
    // }

    // // Bind to port
    // struct sockaddr_in serv_addr;
    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_port = htons(9998);
    // serv_addr.sin_addr.s_addr = INADDR_ANY;

    // if (bind(sockfd_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    // {
    //     self->println("Bind failed");
    //     close(sockfd_);
    //     return;
    // }

    // // Listen
    // if (listen(sockfd_, 1) < 0)
    // {
    //     self->println("Listen failed");
    //     close(sockfd_);
    //     return;
    // }
    // self->println("Receiver listening on port 9998");
}

receiver_actor::behavior_type ReceiverState::make_behavior()
{
    return {
        [this](start_atom)
        {
            // // Accept connection
            // struct sockaddr_in client_addr;
            // socklen_t addr_len = sizeof(client_addr);
            // client_fd_ = accept(sockfd_, (struct sockaddr *)&client_addr, &addr_len);

            // if (client_fd_ < 0)
            // {
            //     self->println("Accept failed");
            //     return;
            // }

            // // Receive data
            // char buffer[4096];
            // std::string json_str;

            // while (true)
            // {
            //     int bytes = recv(client_fd_, buffer, sizeof(buffer) - 1, 0);
            //     if (bytes <= 0)
            //         break;

            //     buffer[bytes] = '\0';
            //     json_str += buffer;

            //     // // Process complete JSON objects
            //     // size_t pos;
            //     // while ((pos = json_str.find("\n")) != std::string::npos)
            //     // {
            //     //     std::string json_obj = json_str.substr(0, pos);
            //     //     json_str = json_str.substr(pos + 1);

            //     //     // Parse JSON to movie
            //     //     caf::json_reader reader;
            //     //     movie m;
            //     //     if (reader.load(json_obj) && reader.apply(m))
            //     //     {
            //     self->println("Received movie over net {}", json_str);
            //     // self->send(results_collector_, caf::put_atom_v, m);
            //     //     }
            //     // }
            // }
        },
    };
}
