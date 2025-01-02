#pragma once

#include <string>
#include <caf/all.hpp>
#include <cstdint>

class movie
{
public:
    int id;
    uint64_t hash = 0;
    std::string title;
    int year;
    double rating;

    template <class Inspector>
    friend bool inspect(Inspector &f, movie &x)
    {
        return f.object(x).fields(
            f.field("title", x.title),
            f.field("year", x.year),
            f.field("rating", x.rating));
    }
};

using movie_list = std::vector<movie>;
