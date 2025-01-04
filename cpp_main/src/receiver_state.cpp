#include "receiver_state.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <caf/json_reader.hpp>

ReceiverState::ReceiverState(receiver_actor::pointer_view ptr,
                             results_collector_actor results_collector,
                             int socket_fd)
    : self(ptr), results_collector_(results_collector), socket_fd_(socket_fd) {
  // nop
}

receiver_actor::behavior_type ReceiverState::make_behavior() {
  return {
      [this](start_atom) {
        uint32_t size;
        std::array<uint8_t, sizeof(size)> size_bytes;
        self->println("ReceiverState waiting for size...");
        ssize_t bytes_read = recv(socket_fd_, size_bytes.data(), 4, 0);
        self->println("ReceiverState received {} bytes", bytes_read);
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
