#include "receiver_state.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <caf/json_reader.hpp>

ReceiverState::ReceiverState(receiver_actor::pointer_view ptr,
                             results_collector_actor results_collector)
    : self(ptr), results_collector_(results_collector) {
  // nop
}

receiver_actor::behavior_type ReceiverState::make_behavior() {
  return {
      [this](start_atom) {
        // Create the server socket
        int serv_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (serv_sockfd < 0) {
          self->println("Failed to create receiver socket");
          return;
        }

        int opt = 1;
        setsockopt(serv_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // Bind to localhost:9998
        sockaddr_in serv_addr{};
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(9998);
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(serv_sockfd, reinterpret_cast<sockaddr *>(&serv_addr),
                 sizeof(serv_addr)) < 0) {
          self->println("Failed to bind receiver socket on port 9998");
          close(serv_sockfd);
          return;
        }

        // Listen for a single client
        if (listen(serv_sockfd, 1) < 0) {
          self->println("Failed to listen on port 9998");
          close(serv_sockfd);
          return;
        }
        self->println("ReceiverState listening on port 9998");

        // Accept the Python sender
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int accepted_fd =
            accept(serv_sockfd, reinterpret_cast<sockaddr *>(&client_addr),
                   &client_len);
        if (accepted_fd < 0) {
          self->println("Failed to accept incoming connection");
          close(serv_sockfd);
          return;
        }
        close(serv_sockfd);  // no longer need the listening socket
        int socket_fd_ = accepted_fd;

        uint32_t size;
        std::array<uint8_t, sizeof(size)> size_bytes;
        ssize_t bytes_read = recv(socket_fd_, size_bytes.data(), 4, 0);
        if (bytes_read != sizeof(size)) {
          self->println("Failed to receive size");
          return;
        }
        std::memcpy(&size, size_bytes.data(), sizeof(size));

        self->println("Expected to receive {} bytes", size);

        if (size == 0) {
          self->println("Received 0 bytes. Terminating...");
          self->mail(finish_atom_v).send(results_collector_);
          self->quit(caf::exit_reason::user_shutdown);
          return;
        }
        // Allocate buffer of exact size
        std::vector<char> buffer(size + 1);
        bytes_read = recv(socket_fd_, buffer.data(), size, 0);
        if (bytes_read <= 0) {
          self->println("Failed to receive data");
          return;
        }

        buffer[bytes_read] = '\0';
        std::string json_str(buffer.data());
        self->println("Received {} bytes: {}", bytes_read, json_str);

        movie_list movies;
        if (caf::json_reader reader;
            (reader.load(json_str)) && (reader.apply(movies))) {
          for (const auto &m : movies) {
            self->println("Receiver sending movie {}", m.title);
            self->mail(caf::put_atom_v, m).send(results_collector_);
          }
        } else {
          self->println("Failed to parse JSON object {}", json_str);
        }
        self->mail(finish_atom_v).send(results_collector_);
        self->quit(caf::exit_reason::user_shutdown);
      },
  };
}
