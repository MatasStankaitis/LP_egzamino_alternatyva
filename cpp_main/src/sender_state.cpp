#include "sender_state.hpp"

#include <sys/socket.h>

SenderState::SenderState(sender_actor::pointer_view ptr, int socket_fd)
    : self(ptr), socket_fd_(socket_fd) {
  // nop
}

sender_actor::behavior_type SenderState::make_behavior() {
  return {
      [this](caf::put_atom, const movie_list& data) {
        writer.reset();
        writer.skip_object_type_annotation(true);

        // Start JSON array
        std::string json_str = "[";

        for (size_t i = 0; i < data.size(); ++i) {
          writer.reset();
          if (!writer.apply(data[i])) {
            self->println("Failed to serialize movie {}", data[i].id);
            continue;
          }
          json_str += writer.str();
          // Add comma if not last element
          if (i < data.size() - 1) {
            json_str += ",";
          }
        }

        // Close JSON array
        json_str += "]\n";

        send(socket_fd_, json_str.c_str(), json_str.length(), 0);
        self->quit(caf::exit_reason::user_shutdown);
      },
  };
}