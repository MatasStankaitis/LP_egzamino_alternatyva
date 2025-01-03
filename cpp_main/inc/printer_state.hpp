#pragma once

#include <caf/all.hpp>
#include "movie.hpp"
#include "actor_traits.hpp"

class PrinterState
{
public:
    PrinterState(printer_actor::pointer_view ptr, std::string_view output_file);
    ~PrinterState() = default;

    printer_actor::behavior_type make_behavior();

private:
    printer_actor::pointer_view self;
    std::string output_file_;

    std::string truncate_title(const std::string &title) const;
};
