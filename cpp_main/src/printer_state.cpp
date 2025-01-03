#include "printer_state.hpp"
#include <fstream>

PrinterState::PrinterState(printer_actor::pointer_view ptr)
    : self(ptr)
{
    // nop
}

printer_actor::behavior_type PrinterState::make_behavior()
{
    return {
        [this](caf::put_atom, movie_list data)
        {
            self->println("printer actor received put_atom");

            std::ofstream out("data/output.txt");
            for (const auto &m : data)
            {
                self->println("Movie: {} (year: {}, rating: {}, hash1: {}, hash2: {})",
                              m.title, m.year, m.rating, m.hash1, m.hash2);
                out << "Movie: " << m.title << " (year: " << m.year << ", rating: " << m.rating << ", hash1: " << m.hash1 << ", hash2: " << m.hash2 << ")\n";
            }
            out.flush();
            out.close();
        },
    };
}