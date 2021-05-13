#include "position.h"
#include "test_runner_p.h"

#include <iostream>
#include <string>
#include <string_view>

inline std::ostream& operator<<(std::ostream& output, Position pos) {
    return output << "(" << pos.row << ", " << pos.col << ")";
}

inline Position operator"" _pos(const char* str, std::size_t) {
    return Position::FromString(str);
}

namespace {

void TestPositionAndStringConversion() {
    auto test_single = [](Position pos, std::string_view str) {
        ASSERT_EQUAL(pos.ToString(), str);
        ASSERT_EQUAL(Position::FromString(str), pos);
    };

    for (int i = 0; i < 25; ++i) {
        test_single(Position{i, i}, char('A' + i) + std::to_string(i + 1));
    }

    test_single(Position{0, 0}, "A1");
    test_single(Position{0, 1}, "B1");
    test_single(Position{0, 25}, "Z1");
    test_single(Position{0, 26}, "AA1");
    test_single(Position{0, 27}, "AB1");
    test_single(Position{0, 51}, "AZ1");
    test_single(Position{0, 52}, "BA1");
    test_single(Position{0, 53}, "BB1");
    test_single(Position{0, 77}, "BZ1");
    test_single(Position{0, 78}, "CA1");
    test_single(Position{0, 701}, "ZZ1");
    test_single(Position{0, 702}, "AAA1");
    test_single(Position{136, 2}, "C137");
    test_single(Position{Position::MAX_ROWS - 1, Position::MAX_COLS - 1}, "XFD16384");
}

void TestPositionToStringInvalid() {
    ASSERT_EQUAL((Position::NONE).ToString(), "");
    ASSERT_EQUAL((Position{-10, 0}).ToString(), "");
    ASSERT_EQUAL((Position{1, -3}).ToString(), "");
}

void TestStringToPositionInvalid() {
    ASSERT(!Position::FromString("").IsValid());
    ASSERT(!Position::FromString("A").IsValid());
    ASSERT(!Position::FromString("1").IsValid());
    ASSERT(!Position::FromString("e2").IsValid());
    ASSERT(!Position::FromString("A0").IsValid());
    ASSERT(!Position::FromString("A-1").IsValid());
    ASSERT(!Position::FromString("A+1").IsValid());
    ASSERT(!Position::FromString("R2D2").IsValid());
    ASSERT(!Position::FromString("C3PO").IsValid());
    ASSERT(!Position::FromString("XFD16385").IsValid());
    ASSERT(!Position::FromString("XFE16384").IsValid());
    ASSERT(!Position::FromString("A1234567890123456789").IsValid());
    ASSERT(!Position::FromString("ABCDEFGHIJKLMNOPQRS8").IsValid());
}

void TestEqualLess() {
    ASSERT((Position{0, 0}) == (Position{0, 0}));
    ASSERT((Position{10, 0}) == (Position{10, 0}));
    ASSERT(!((Position{10, 0}) == (Position{0, 10})));
    ASSERT(!((Position{0, 10}) == (Position{10, 0})));

    ASSERT(!((Position{0, 0}) < (Position{0, 0})));
    ASSERT(((Position{0, 10}) < (Position{0, 20})));
    ASSERT(!((Position{1, 10}) < (Position{0, 20})));
}

void TestIsValid() {
    ASSERT((Position{0, 0}).IsValid());
    ASSERT((Position{Position::MAX_ROWS - 1, Position::MAX_COLS - 1}).IsValid());
    ASSERT(!(Position{Position::MAX_ROWS, Position::MAX_COLS}).IsValid());
    ASSERT(!(Position{Position::MAX_ROWS + 1, Position::MAX_COLS}).IsValid());
    ASSERT(!(Position{Position::MAX_ROWS, Position::MAX_COLS + 1}).IsValid());
    ASSERT(!Position::NONE.IsValid());
    ASSERT(!(Position{-1, 0}).IsValid());
    ASSERT(!(Position{0, -1}).IsValid());
}

void TestToString() {
    ASSERT_EQUAL((Position{0, 0}).ToString(), "A1");
    ASSERT_EQUAL((Position{0, 1}).ToString(), "B1");
    ASSERT_EQUAL((Position{0, 25}).ToString(), "Z1");
    ASSERT_EQUAL((Position{0, 26}).ToString(), "AA1");
    ASSERT_EQUAL((Position{0, 27}).ToString(), "AB1");
    ASSERT_EQUAL((Position{0, 51}).ToString(), "AZ1");
    ASSERT_EQUAL((Position{0, 52}).ToString(), "BA1");
    ASSERT_EQUAL((Position{0, 53}).ToString(), "BB1");
    ASSERT_EQUAL((Position{0, 77}).ToString(), "BZ1");
    ASSERT_EQUAL((Position{0, 78}).ToString(), "CA1");
    ASSERT_EQUAL((Position{0, 701}).ToString(), "ZZ1");
    ASSERT_EQUAL((Position{0, 702}).ToString(), "AAA1");
    ASSERT_EQUAL((Position{136, 2}).ToString(), "C137");
    ASSERT_EQUAL((Position{Position::MAX_ROWS - 1, Position::MAX_COLS - 1}).ToString(), "XFD16384");
}

void TestFromString() {
    ASSERT_EQUAL(Position::FromString("A1"), (Position{0, 0}));
    ASSERT_EQUAL(Position::FromString("B1"), (Position{0, 1}));
    ASSERT_EQUAL(Position::FromString("Z1"), (Position{0, 25}));
    ASSERT_EQUAL(Position::FromString("AA1"), (Position{0, 26}));
    ASSERT_EQUAL(Position::FromString("AB1"), (Position{0, 27}));
    ASSERT_EQUAL(Position::FromString("AZ1"), (Position{0, 51}));
    ASSERT_EQUAL(Position::FromString("BA1"), (Position{0, 52}));
    ASSERT_EQUAL(Position::FromString("BB1"), (Position{0, 53}));
    ASSERT_EQUAL(Position::FromString("BZ1"), (Position{0, 77}));
    ASSERT_EQUAL(Position::FromString("CA1"), (Position{0, 78}));
    ASSERT_EQUAL(Position::FromString("ZZ1"), (Position{0, 701}));
    ASSERT_EQUAL(Position::FromString("AAA1"), (Position{0, 702}));
    ASSERT_EQUAL(Position::FromString("C137"), (Position{136, 2}));
    ASSERT_EQUAL(Position::FromString("XFD16384"), (Position{Position::MAX_ROWS - 1, Position::MAX_COLS - 1}));
}

}  // namespace

int main() {
    TestRunner tr;

    RUN_TEST(tr, TestEqualLess);
    RUN_TEST(tr, TestIsValid);
    RUN_TEST(tr, TestToString);
    RUN_TEST(tr, TestFromString);
    RUN_TEST(tr, TestPositionAndStringConversion);
    RUN_TEST(tr, TestPositionToStringInvalid);
    RUN_TEST(tr, TestStringToPositionInvalid);

    return 0;
}
