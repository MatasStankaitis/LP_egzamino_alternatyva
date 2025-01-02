#include "sender_state.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

static constexpr int PORT = 9999;
static constexpr char IP[] = "127.0.0.1";

SenderState::SenderState(sender_actor::pointer_view ptr)
    : self(ptr)
{
    // Create socket
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ < 0)
    {
        self->println("Error creating socket");
        return;
    }

    // Connect to Python server
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &serv_addr.sin_addr);

    if (connect(sockfd_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        self->println("Connection Failed");
        close(sockfd_);
        return;
    }
    self->println("Connected to Python server");
}

SenderState::~SenderState()
{
    close(sockfd_);
}

sender_actor::behavior_type SenderState::make_behavior()
{
    return {
        [this](caf::put_atom, movie_list data)
        {
            writer.reset();
            writer.skip_object_type_annotation(true);
            std::string json_str;

            for (auto &m : data)
            {
                writer.reset();
                if (!writer.apply(m))
                {
                    self->println("Failed to serialize movie {}", m.id);
                    continue;
                }
                json_str += writer.str();
                json_str += ",\n";
            }
            send(sockfd_, json_str.c_str(), json_str.length(), 0);
        }};
}