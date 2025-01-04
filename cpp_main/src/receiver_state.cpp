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
        self->println(caf::term::yellow, "[Receiver] received start_atom");

        uint32_t size;
        std::array<uint8_t, sizeof(size)> size_bytes;
        self->println(caf::term::yellow,
                      "[Receiver] waiting for incoming data size...");
        ssize_t bytes_read =
            recv(socket_fd_, size_bytes.data(), sizeof(size), 0);
        if (bytes_read != sizeof(size)) {
          self->println(
              caf::term::bold_yellow,
              "[Receiver] failed to receive data size. Terminating...");
          self->mail(finish_atom_v).send(results_collector_);
          self->quit(caf::exit_reason::user_shutdown);
          return;
        }

        std::memcpy(&size, size_bytes.data(), sizeof(size));
        if (size <= 0) {
          self->println(
              caf::term::bold_yellow,
              "[Receiver] expecting to receive {} bytes. Terminating...", size);
          self->mail(finish_atom_v).send(results_collector_);
          self->quit(caf::exit_reason::user_shutdown);
          return;
        }

        self->println(caf::term::yellow,
                      "[Receiver] expecting to receive {} bytes", size);

        std::vector<char> data_buffer(size + 1);
        bytes_read = recv(socket_fd_, data_buffer.data(), size, 0);
        if (bytes_read <= 0) {
          self->println(caf::term::bold_yellow,
                        "[Receiver] failed to receive data. Terminating...");
          self->mail(finish_atom_v).send(results_collector_);
          self->quit(caf::exit_reason::user_shutdown);
          return;
        }

        data_buffer[bytes_read] = '\0';
        std::string json_str(data_buffer.data());

        movie_list movies;
        if (caf::json_reader reader;
            (reader.load(json_str)) && (reader.apply(movies))) {
          for (const auto &m : movies) {
            self->println(caf::term::yellow,
                          "[Receiver] sending movie {} to collector", m.title);
            self->mail(caf::put_atom_v, m).send(results_collector_);
          }
        } else {
          self->println(caf::term::bold_yellow,
                        "[Receiver] failed to parse JSON object {}", json_str);
        }
        self->mail(finish_atom_v).send(results_collector_);
        self->println(caf::term::yellow, "[Receiver] terminating...");
        self->quit(caf::exit_reason::user_shutdown);
      },
  };
}
