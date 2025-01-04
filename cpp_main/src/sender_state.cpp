#include "sender_state.hpp"

#include <sys/socket.h>

SenderState::SenderState(sender_actor::pointer_view ptr, int socket_fd)
    : self(ptr), socket_fd_(socket_fd) {
  // nop
}

sender_actor::behavior_type SenderState::make_behavior() {
  return {
      [this](caf::put_atom, const movie_list& data) {
        self->println(caf::term::magenta, "[Sender] received put_atom");
        writer.reset();
        writer.skip_object_type_annotation(true);

        std::string json_str = "[";

        for (size_t i = 0; i < data.size(); ++i) {
          writer.reset();
          if (!writer.apply(data[i])) {
            self->println(caf::term::magenta,
                          "[Sender] failed to serialize movie {} to json",
                          data[i].title);
            continue;
          }
          json_str += writer.str();
          if (i < data.size() - 1) {
            json_str += ",";
          }
        }

        json_str += "]\n";

        self->println(caf::term::magenta,
                      "[Sender] sending data of size {} to receiver",
                      json_str.length());
        send(socket_fd_, json_str.c_str(), json_str.length(), 0);

        self->println(caf::term::magenta, "[Sender] terminating...");
        self->quit(caf::exit_reason::user_shutdown);
      },
  };
}