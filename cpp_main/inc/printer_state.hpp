#pragma once

#include <caf/all.hpp>

#include "actor_traits.hpp"
#include "movie.hpp"

class PrinterState {
 public:
  PrinterState(printer_actor::pointer_view ptr, std::string_view output_file);
  ~PrinterState() = default;

  printer_actor::behavior_type make_behavior();

 private:
  printer_actor::pointer_view self;
  std::string output_file_;

  /**
   * @brief Truncates the title to a maximum length (30 chars).
   *
   * @param title The title to truncate.
   *
   * @return The truncated title.
   */
  std::string truncate_title(std::string_view title) const;
};
