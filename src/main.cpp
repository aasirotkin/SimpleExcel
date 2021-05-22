#include <iostream>

#include "sheet.h"

inline Position operator"" _pos(const char* str, std::size_t) {
    return Position::FromString(str);
}

void Tests();

int main() {
    Tests();

// ---------- Usage example ----------------------------------------------------

    using namespace std::string_literals;

    auto sheet = CreateSheet();

    sheet->SetCell("A1"_pos, "=5"s);
    sheet->SetCell("B5"_pos, "=A1 - 3"s);
    sheet->SetCell("E1"_pos, "=ABCD3"s);
    sheet->SetCell("C3"_pos, "10"s);
    sheet->SetCell("D8"_pos, "Good luck"s);
    sheet->SetCell("A4"_pos, "=A1/(C3 - 10)"s);
    sheet->SetCell("E6"_pos, "'=This is text"s);
    sheet->SetCell("D2"_pos, "=D8 * 3"s);
    try {
        // Circular dependency error
        sheet->SetCell("C7"_pos, "=C7"s);
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    sheet->PrintValues(std::cout);

// -----------------------------------------------------------------------------

    return 0;
}
