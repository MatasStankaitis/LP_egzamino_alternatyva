#include "printer_state.hpp"
#include <fstream>
#include <iomanip>

static constexpr int max_title_length = 30;

PrinterState::PrinterState(printer_actor::pointer_view ptr, std::string_view output_file)
    : self(ptr), output_file_(output_file)
{
    // nop
}

printer_actor::behavior_type PrinterState::make_behavior()
{
    return {
        [this](caf::put_atom, movie_list data)
        {
            self->println("printer actor received put_atom");

            std::ofstream out(output_file_, std::ios::app);
            out << "\nFiltered movies:\n";
            out << std::setfill('-') << std::setw(100) << "-" << std::endl;
            out << std::setfill(' ');
            out << std::left
                << std::setw(max_title_length) << "Title"
                << std::setw(10) << "Year"
                << std::setw(10) << "Rating"
                << std::setw(20) << "Hash1"
                << std::setw(20) << "Hash2" << "\n";
            out << std::setfill('-') << std::setw(100) << "-" << std::endl;
            out << std::setfill(' ');

            for (const auto &m : data)
            {
                self->println("Movie: {} (year: {}, rating: {}, hash1: {}, hash2: {})",
                              m.title, m.year, m.rating, m.hash1, m.hash2);
                out << std::left
                    << std::setw(max_title_length) << truncate_title(m.title)
                    << std::setw(10) << m.year
                    << std::setw(10) << m.rating
                    << std::setw(20) << m.hash1
                    << std::setw(20) << m.hash2 << "\n";
            }
            out << std::setfill('-') << std::setw(100) << "-" << std::endl;
            out.flush();
            out.close();
        },
        [this](initial_data_atom, movie_list data)
        {
            self->println("printer actor received initial_data_atom");

            std::ofstream out(output_file_);
            out << "Initial data:\n";
            out << std::setfill('-') << std::setw(100) << "-" << std::endl;
            out << std::setfill(' ');
            out << std::left
                << std::setw(30) << "Title"
                << std::setw(10) << "Year"
                << std::setw(10) << "Rating"
                << std::setw(20) << "Hash1"
                << std::setw(20) << "Hash2" << "\n";
            out << std::setfill('-') << std::setw(100) << "-" << std::endl;
            out << std::setfill(' ');

            for (const auto &m : data)
            {
                out << std::left
                    << std::setw(max_title_length) << truncate_title(m.title)
                    << std::setw(10) << m.year
                    << std::setw(10) << m.rating
                    << std::setw(20) << m.hash1
                    << std::setw(20) << m.hash2 << "\n";
            }
            out << std::setfill('-') << std::setw(100) << "-" << std::endl;
            out.flush();
            out.close();
        },
    };
}

std::string PrinterState::truncate_title(const std::string &title) const
{
    if (title.length() <= max_title_length)
    {
        return title;
    }
    return title.substr(0, max_title_length - 4) + "...";
}