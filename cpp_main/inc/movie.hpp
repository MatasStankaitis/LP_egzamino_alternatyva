#pragma once

#include <caf/all.hpp>
#include <cstdint>
#include <string>

class Movie {
 public:
  Movie() = default;
  ~Movie() = default;
  Movie(const Movie &) = default;
  Movie &operator=(const Movie &) = default;

  int id;
  uint64_t hash1 = 0;
  uint64_t hash2 = 0;
  std::string title;
  int year;
  double rating;

  template <class Inspector>
  friend bool inspect(Inspector &f, Movie &x) {
    return f.object(x).fields(
        f.field("id", x.id).fallback(0), f.field("hash1", x.hash1).fallback(0),
        f.field("hash2", x.hash2).fallback(0), f.field("title", x.title),
        f.field("year", x.year), f.field("rating", x.rating));
  }
};

using movie_list = std::vector<Movie>;
