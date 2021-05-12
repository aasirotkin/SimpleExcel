#include "common.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
#include <tuple>

const int LETTERS = 26;
const int MAX_POSITION_LENGTH = 17;
const int MAX_POS_LETTER_COUNT = 3;

const Position Position::NONE = {-1, -1};

class PositionCreator {
public:
    static Position Build(std::string_view str) {
        auto it = std::find_if(str.begin(), str.end(), IsValidIntSymbol);
        if (it == str.begin() || it == str.end()) {
            return Position::NONE;
        }
        if (!IsValidString(str.begin(), it) || !IsValidNumber(it, str.end())) {
            return Position::NONE;
        }
        Position pos = CreatePosition(str.begin(), it, str.end());

        return (pos.IsValid()) ? pos : Position::NONE; 
    }

    static std::string Build(const Position& pos) {
        if (!pos.IsValid()) {
            return std::string();
        }
        return ColToString(pos.col) + RowToString(pos.row);
    }

private:
    static const int A_INDEX = 65;
    static const int LETTERS = 26;
    static const int MAX_NUMBER_LENGTH = 17;
    static const int MAX_LETTER_COUNT = 3;

private:
    static std::string RowToString(int row) {
        return std::to_string(row + 1);
    }

    static std::string ColToString(int col) {
        int numerator = col;
        std::string col_to_str{};
        while (numerator >= PositionCreator::LETTERS) {
            int remainder = numerator % PositionCreator::LETTERS;
            col_to_str.insert(0, 1, GetChar(remainder));
            numerator = numerator / PositionCreator::LETTERS - 1;
        }
        col_to_str.insert(0, 1, GetChar(numerator));
        return col_to_str;
    }

    static char GetChar(int i) {
        assert(i >= 0 && i < PositionCreator::LETTERS);
        return static_cast<char>(i + PositionCreator::A_INDEX);
    }

    static int GetIndex(char c) {
        assert(IsValidCharSymbol(c));
        return static_cast<int>(c) - PositionCreator::A_INDEX;
    }

    static bool IsValidCharSymbol(char c) {
        return c >= 'A' && c <= 'Z';
    }

    static bool IsValidIntSymbol(char c) {
        return c >= '0' && c <= '9';
    }

private:

template <typename It>
static bool IsValidString(It begin, It end) {
    return
        (end - begin) <= PositionCreator::MAX_LETTER_COUNT &&
        std::all_of(begin, end, IsValidCharSymbol);
}

template <typename It>
static bool IsValidNumber(It begin, It end) {
    if (*begin == '0') {
        return (end - begin) == 1;
    }
    return
        (end - begin) <= PositionCreator::MAX_NUMBER_LENGTH &&
        std::all_of(begin, end, IsValidIntSymbol);
}

template <typename It>
static Position CreatePosition(It letters_begin, It letters_end, It end) {
    int row = std::stoi(std::string(letters_end, end)) - 1;

    int col = 0;
    int power = (letters_end - letters_begin) - 1;
    while (power != 0) {
        col += (GetIndex(*letters_begin++) + 1) * std::pow(PositionCreator::LETTERS, power--);
    }
    col += GetIndex(*letters_begin);

    return { row, col };
}

};

// Реализуйте методы:
bool Position::operator==(const Position rhs) const {
    return (row == rhs.row) && (col == rhs.col);
}

bool Position::operator<(const Position rhs) const {
    return std::make_tuple(row, col) < std::make_tuple(rhs.row, rhs.col);
}

bool Position::IsValid() const {
    return row >= 0 && col >= 0 && row < MAX_ROWS && col < MAX_COLS;
}

std::string Position::ToString() const {
    return PositionCreator::Build(*this);
}

Position Position::FromString(std::string_view str) {
    return PositionCreator::Build(str);
}
