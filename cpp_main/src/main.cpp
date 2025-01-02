#include "caf/all.hpp"
#include "main_actor.hpp"
#include <chrono>
#include <cstdlib>
#include <random>
#include "caf/json_reader.hpp"

using namespace caf;

struct movie
{
    std::string title;
    int year;
    double rating;
};

template <class Inspector>
bool inspect(Inspector &f, movie &x)
{
    return f.object(x).fields(f.field("title", x.title), f.field("year", x.year),
                              f.field("rating", x.rating));
}

using movie_list = std::vector<movie>;

CAF_BEGIN_TYPE_ID_BLOCK(example_app, caf::first_custom_type_id)

CAF_ADD_TYPE_ID(example_app, (movie))
CAF_ADD_TYPE_ID(example_app, (movie_list))

CAF_END_TYPE_ID_BLOCK(example_app)

struct main_actor_trait
{
    using signatures = caf::type_list<
        caf::result<void>(caf::spawn_atom)>;
};

struct worker_actor_trait
{
    using signatures = caf::type_list<
        caf::result<void>(caf::put_atom, std::string),
        caf::result<std::string>(caf::get_atom)>;
};

struct sender_actor_trait
{
    using signatures = caf::type_list<
        caf::result<void>(caf::put_atom, movie_list)>;
};

using main_actor = caf::typed_actor<main_actor_trait>;
using worker_actor = caf::typed_actor<worker_actor_trait>;
using sender_actor = caf::typed_actor<sender_actor_trait>;

using worker_list = std::vector<worker_actor>;

struct worker_state
{
    worker_state(worker_actor::pointer_view ptr)
        : self(ptr)
    {
        // nop
    }

    worker_actor::behavior_type make_behavior()
    {
        return {
            [this](caf::put_atom, std::string data)
            {
                data_ = data;
                self->println("worker actor received put_atom");
            },
            [this](caf::get_atom)
            {
                self->println("worker actor received get_atom");
                return data_;
            },
        };
    }

    worker_actor::pointer_view self;
    std::string data_;
};

class main_state
{
public:
    main_state(main_actor::pointer_view ptr, worker_list workers, sender_actor sender)
        : self(ptr), workers_(workers), sender_(sender)
    {
        self->println("main actor created");
    }

    main_actor::behavior_type make_behavior()
    {
        return {
            [this](caf::spawn_atom)
            {
                self->println("main actor received spawn_atom");
                read_input_file("data/test.json");
            },
        };
        self->mail(caf::put_atom_v, workload).send(sender_);
    }

    int read_input_file(std::string_view file_path)
    {
        self->println("main actor received read_input_file");
        caf::json_reader reader;
        if (!reader.load_file(file_path.data()))
        {
            self->println("*** failed to load the user list: {}\n", reader.get_error());
            return EXIT_FAILURE;
        }

        if (!reader.apply(workload))
        {
            self->println("*** failed to deserialize the user list: {}\n", reader.get_error());
            return EXIT_FAILURE;
        }
        // Print the list in "CAF format".
        self->println("Entries loaded from file:\n");
        for (auto &entry : workload)
            self->println("- {}", entry);
        return EXIT_SUCCESS;
    }

    void distribute_workload();

    main_actor::pointer_view self;
    movie_list workload;
    worker_list workers_;
    sender_actor sender_;
};

struct sender_state
{
    sender_state(sender_actor::pointer_view ptr)
        : self(ptr)
    {
        // nop
    }

    sender_actor::behavior_type make_behavior()
    {
        return {
            [this](caf::put_atom, movie_list data)
            {
                self->println("sender actor received movie: {}\n", data[0].title);
            },
        };
    }

    sender_actor::pointer_view self;
};

void caf_main(caf::actor_system &sys)
{
    int worker_count = 5;
    worker_list worker_actors;

    for (int i = 0; i < worker_count; i++)
    {
        worker_actors.emplace_back(sys.spawn(caf::actor_from_state<worker_state>));
    }
    auto sender_actor = sys.spawn(caf::actor_from_state<sender_state>);

    auto main_actor = sys.spawn(caf::actor_from_state<main_state>, worker_actors, sender_actor);

    scoped_actor self{sys};
    self->mail(caf::spawn_atom_v).send(main_actor);
}

CAF_MAIN()