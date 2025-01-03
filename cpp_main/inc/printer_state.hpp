#pragma once

#include <caf/all.hpp>
#include "movie.hpp"
#include "actor_traits.hpp"

class PrinterState
{
public:
    PrinterState(printer_actor::pointer_view ptr);
    ~PrinterState() = default;

    printer_actor::behavior_type make_behavior();

private:
    printer_actor::pointer_view self;
};
