#include "cell.h"
#include "common.h"
#include "formula.h"
#include "FormulaAST.h"
#include "position.h"
#include "sheet.h"
#include "test_runner_p.h"

using namespace std::literals;

namespace {

// -----------------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& output, Position pos) {
    return output << "(" << pos.row << ", " << pos.col << ")";
}

inline Position operator"" _pos(const char* str, std::size_t) {
    return Position::FromString(str);
}

inline std::ostream& operator<<(std::ostream& output, Size size) {
    return output << "(" << size.rows << ", " << size.cols << ")";
}

// -----------------------------------------------------------------------------

void TestPositionAndStringConversion() {
    auto test_single = [](Position pos, std::string_view str) {
        ASSERT_EQUAL(pos.ToString(), str);
        ASSERT_EQUAL(Position::FromString(str), pos);
    };

    for (int i = 0; i < 25; ++i) {
        test_single(Position{ i, i }, char('A' + i) + std::to_string(i + 1));
    }

    test_single(Position{ 0, 0 }, "A1");
    test_single(Position{ 0, 1 }, "B1");
    test_single(Position{ 0, 25 }, "Z1");
    test_single(Position{ 0, 26 }, "AA1");
    test_single(Position{ 0, 27 }, "AB1");
    test_single(Position{ 0, 51 }, "AZ1");
    test_single(Position{ 0, 52 }, "BA1");
    test_single(Position{ 0, 53 }, "BB1");
    test_single(Position{ 0, 77 }, "BZ1");
    test_single(Position{ 0, 78 }, "CA1");
    test_single(Position{ 0, 701 }, "ZZ1");
    test_single(Position{ 0, 702 }, "AAA1");
    test_single(Position{ 136, 2 }, "C137");
    test_single(Position{ Position::MAX_ROWS - 1, Position::MAX_COLS - 1 }, "XFD16384");
}

void TestPositionToStringInvalid() {
    ASSERT_EQUAL((Position::NONE).ToString(), "");
    ASSERT_EQUAL((Position{ -10, 0 }).ToString(), "");
    ASSERT_EQUAL((Position{ 1, -3 }).ToString(), "");
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
    ASSERT((Position{ 0, 0 }) == (Position{ 0, 0 }));
    ASSERT((Position{ 10, 0 }) == (Position{ 10, 0 }));
    ASSERT(!((Position{ 10, 0 }) == (Position{ 0, 10 })));
    ASSERT(!((Position{ 0, 10 }) == (Position{ 10, 0 })));

    ASSERT(!((Position{ 0, 0 }) < (Position{ 0, 0 })));
    ASSERT(((Position{ 0, 10 }) < (Position{ 0, 20 })));
    ASSERT(!((Position{ 1, 10 }) < (Position{ 0, 20 })));
}

void TestIsValid() {
    ASSERT((Position{ 0, 0 }).IsValid());
    ASSERT((Position{ Position::MAX_ROWS - 1, Position::MAX_COLS - 1 }).IsValid());
    ASSERT(!(Position{ Position::MAX_ROWS, Position::MAX_COLS }).IsValid());
    ASSERT(!(Position{ Position::MAX_ROWS + 1, Position::MAX_COLS }).IsValid());
    ASSERT(!(Position{ Position::MAX_ROWS, Position::MAX_COLS + 1 }).IsValid());
    ASSERT(!Position::NONE.IsValid());
    ASSERT(!(Position{ -1, 0 }).IsValid());
    ASSERT(!(Position{ 0, -1 }).IsValid());
}

void TestToString() {
    ASSERT_EQUAL((Position{ 0, 0 }).ToString(), "A1");
    ASSERT_EQUAL((Position{ 0, 1 }).ToString(), "B1");
    ASSERT_EQUAL((Position{ 0, 25 }).ToString(), "Z1");
    ASSERT_EQUAL((Position{ 0, 26 }).ToString(), "AA1");
    ASSERT_EQUAL((Position{ 0, 27 }).ToString(), "AB1");
    ASSERT_EQUAL((Position{ 0, 51 }).ToString(), "AZ1");
    ASSERT_EQUAL((Position{ 0, 52 }).ToString(), "BA1");
    ASSERT_EQUAL((Position{ 0, 53 }).ToString(), "BB1");
    ASSERT_EQUAL((Position{ 0, 77 }).ToString(), "BZ1");
    ASSERT_EQUAL((Position{ 0, 78 }).ToString(), "CA1");
    ASSERT_EQUAL((Position{ 0, 701 }).ToString(), "ZZ1");
    ASSERT_EQUAL((Position{ 0, 702 }).ToString(), "AAA1");
    ASSERT_EQUAL((Position{ 136, 2 }).ToString(), "C137");
    ASSERT_EQUAL((Position{ Position::MAX_ROWS - 1, Position::MAX_COLS - 1 }).ToString(), "XFD16384");
}

void TestFromString() {
    ASSERT_EQUAL(Position::FromString("A1"), (Position{ 0, 0 }));
    ASSERT_EQUAL(Position::FromString("B1"), (Position{ 0, 1 }));
    ASSERT_EQUAL(Position::FromString("Z1"), (Position{ 0, 25 }));
    ASSERT_EQUAL(Position::FromString("AA1"), (Position{ 0, 26 }));
    ASSERT_EQUAL(Position::FromString("AB1"), (Position{ 0, 27 }));
    ASSERT_EQUAL(Position::FromString("AZ1"), (Position{ 0, 51 }));
    ASSERT_EQUAL(Position::FromString("BA1"), (Position{ 0, 52 }));
    ASSERT_EQUAL(Position::FromString("BB1"), (Position{ 0, 53 }));
    ASSERT_EQUAL(Position::FromString("BZ1"), (Position{ 0, 77 }));
    ASSERT_EQUAL(Position::FromString("CA1"), (Position{ 0, 78 }));
    ASSERT_EQUAL(Position::FromString("ZZ1"), (Position{ 0, 701 }));
    ASSERT_EQUAL(Position::FromString("AAA1"), (Position{ 0, 702 }));
    ASSERT_EQUAL(Position::FromString("C137"), (Position{ 136, 2 }));
    ASSERT_EQUAL(Position::FromString("XFD16384"), (Position{ Position::MAX_ROWS - 1, Position::MAX_COLS - 1 }));
}

void TestPosition() {
    TestEqualLess();
    TestIsValid();
    TestToString();
    TestFromString();
    TestPositionAndStringConversion();
    TestPositionToStringInvalid();
    TestStringToPositionInvalid();
}

// -----------------------------------------------------------------------------

void SetCellValue(const std::string& value, Position pos, Sheet& sheet) {
    CellInterface* ci = sheet.GetCell(pos);
    if (!ci) {
        sheet.SetCell(pos, value);
        ci = sheet.GetCell(pos);
    }
    dynamic_cast<Cell*>(ci)->Set(value);
}

void CheckCache(bool value, Position pos, Sheet& sheet) {
    CellInterface* ci = sheet.GetCell(pos);
    if (!ci) {
        ASSERT(false);
    }
    ASSERT_EQUAL(dynamic_cast<Cell*>(ci)->IsCacheValie(), value);
}

struct CellValueChecker {
    void operator() (const std::string& value) {
        ASSERT(std::holds_alternative<std::string>(expected_value));
        ASSERT_EQUAL(value, std::get<std::string>(expected_value));
    }

    void operator() (double value) {
        ASSERT(std::holds_alternative<double>(expected_value));
        ASSERT_EQUAL(value, std::get<double>(expected_value));
    }

    void operator() (const FormulaError& value) {
        ASSERT(std::holds_alternative<FormulaError>(expected_value));
        ASSERT_EQUAL(value, std::get<FormulaError>(expected_value));
    }

    CellInterface::Value expected_value;
};

void TestSimpleCircularDependecies() {
    Sheet sheet;

    Position a1 = Position::FromString("A1"sv);
    Position b2 = Position::FromString("B2"sv);
    Position c3 = Position::FromString("C3"sv);
    Position d4 = Position::FromString("D4"sv);
    Position e5 = Position::FromString("E5"sv);

    sheet.SetCell(a1, "=E5 * 10"s);
    sheet.SetCell(b2, "=A1 + 2"s);
    sheet.SetCell(c3, "=B2 + A1"s);
    sheet.SetCell(d4, "=C3 + A1 + B2"s);

    try {
        sheet.SetCell(e5, "= B2 / C3 - D4 + A1"s);
        ASSERT(false);
    }
    catch (const CircularDependencyException& exp) {
    }
    catch (...) {
        ASSERT(false);
    }
}

void TestSimpleDependecies() {
    Sheet sheet;

    Position a1 = Position::FromString("A1"sv);
    Position a2 = Position::FromString("A2"sv);
    Position a3 = Position::FromString("A3"sv);

    sheet.SetCell(a1, "3"s);
    sheet.SetCell(a2, "=A1 + 2"s);
    sheet.SetCell(a3, "=A2 + 3"s);

    ASSERT(sheet.GetCell(a1)->GetReferencedCells().empty());
    ASSERT(sheet.GetCell(a2)->GetReferencedCells().size() == 1);
    ASSERT(sheet.GetCell(a3)->GetReferencedCells().size() == 2);

    std::visit(CellValueChecker{ "3" }, sheet.GetCell(a1)->GetValue());
    std::visit(CellValueChecker{ 5.0 }, sheet.GetCell(a2)->GetValue());
    std::visit(CellValueChecker{ 8.0 }, sheet.GetCell(a3)->GetValue());

    SetCellValue("=3"s, a2, sheet);

    ASSERT(sheet.GetCell(a1)->GetReferencedCells().empty());
    ASSERT(sheet.GetCell(a2)->GetReferencedCells().empty());
    ASSERT(sheet.GetCell(a3)->GetReferencedCells().size() == 1);

    std::visit(CellValueChecker{ "3" }, sheet.GetCell(a1)->GetValue());
    std::visit(CellValueChecker{ 3.0 }, sheet.GetCell(a2)->GetValue());
    std::visit(CellValueChecker{ 6.0 }, sheet.GetCell(a3)->GetValue());

    SetCellValue("=A3 - A2"s, a1, sheet);

    ASSERT(sheet.GetCell(a1)->GetReferencedCells().size() == 2);
    ASSERT(sheet.GetCell(a2)->GetReferencedCells().empty());
    ASSERT(sheet.GetCell(a3)->GetReferencedCells().size() == 1);

    std::visit(CellValueChecker{ 3.0 }, sheet.GetCell(a1)->GetValue());
    std::visit(CellValueChecker{ 3.0 }, sheet.GetCell(a2)->GetValue());
    std::visit(CellValueChecker{ 6.0 }, sheet.GetCell(a3)->GetValue());
}

void TestCommonCases() {
    Sheet sheet;

#define CREATE_CELL(a) \
    Position a = Position::FromString(#a);

    CREATE_CELL(A1);
    CREATE_CELL(C5);
    CREATE_CELL(D8);
    CREATE_CELL(E3);
    CREATE_CELL(B16);
    CREATE_CELL(F4);
    CREATE_CELL(O54);
    CREATE_CELL(J36);
    CREATE_CELL(K15);
    CREATE_CELL(AA33);
    CREATE_CELL(CB15);

    SetCellValue("=7"s, C5, sheet);
    SetCellValue("=C5*A1"s, D8, sheet);

    ASSERT(sheet.GetCell(A1)->GetReferencedCells().size() == 0);
    ASSERT(sheet.GetCell(C5)->GetReferencedCells().size() == 0);
    ASSERT(sheet.GetCell(D8)->GetReferencedCells().size() == 2);

    std::visit(CellValueChecker{ 0.0 }, sheet.GetCell(D8)->GetValue());

    SetCellValue("=D8/A1"s, E3, sheet);

    ASSERT(sheet.GetCell(E3)->GetReferencedCells().size() == 3);
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Div0) }, sheet.GetCell(E3)->GetValue());

    SetCellValue("'7"s, B16, sheet);
    ASSERT(sheet.GetCell(B16)->GetReferencedCells().size() == 0);
    std::visit(CellValueChecker{ "7"s }, sheet.GetCell(B16)->GetValue());

    SetCellValue("=B16 - 1"s, F4, sheet);
    ASSERT(sheet.GetCell(F4)->GetReferencedCells().size() == 1);
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Value) }, sheet.GetCell(F4)->GetValue());

    SetCellValue("=FFFF4 - 1"s, O54, sheet);
    ASSERT(sheet.GetCell(O54)->GetReferencedCells().size() == 0);
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Ref) }, sheet.GetCell(O54)->GetValue());

    SetCellValue("=A1234567 - 1"s, J36, sheet);
    ASSERT(sheet.GetCell(J36)->GetReferencedCells().size() == 0);
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Ref) }, sheet.GetCell(J36)->GetValue());

    SetCellValue("13"s, K15, sheet);
    ASSERT(sheet.GetCell(K15)->GetReferencedCells().size() == 0);

    SetCellValue("=(K15 - 10) * C5"s, AA33, sheet);
    ASSERT(sheet.GetCell(AA33)->GetReferencedCells().size() == 2);
    std::visit(CellValueChecker{ 21.0 }, sheet.GetCell(AA33)->GetValue());

    SetCellValue("=AA33 * 2"s, CB15, sheet);
    ASSERT(sheet.GetCell(CB15)->GetReferencedCells().size() == 3);
    std::visit(CellValueChecker{ 42.0 }, sheet.GetCell(CB15)->GetValue());

    try {
        SetCellValue("=CB15 / 2"s, K15, sheet);
        ASSERT(false);
    }
    catch (const CircularDependencyException& exp) {
        ASSERT(sheet.GetCell(K15)->GetReferencedCells().size() == 0);
        std::visit(CellValueChecker{ "13" }, sheet.GetCell(K15)->GetValue());
    }
    catch (...) {
        ASSERT(false);
    }
}

void TestErrorDependencies() {
    Sheet sheet;

#define CREATE_CELL(a) \
    Position a = Position::FromString(#a);

    CREATE_CELL(A1);
    CREATE_CELL(A2);
    CREATE_CELL(A3);
    CREATE_CELL(A4);
    CREATE_CELL(A5);

    SetCellValue("=A4 + A3 + A2 + A1"s, A5, sheet);
    SetCellValue("=A3 + A2 + A1"s, A4, sheet);
    SetCellValue("=A2 + A1"s, A3, sheet);
    SetCellValue("=A1"s, A2, sheet);
    SetCellValue("'7"s, A1, sheet);

    CheckCache(false, A5, sheet);
    CheckCache(false, A4, sheet);
    CheckCache(false, A3, sheet);
    CheckCache(false, A2, sheet);
    CheckCache(false, A1, sheet);

    std::visit(CellValueChecker{ "7" }, sheet.GetCell(A1)->GetValue());
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Value) }, sheet.GetCell(A2)->GetValue());
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Value) }, sheet.GetCell(A3)->GetValue());
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Value) }, sheet.GetCell(A4)->GetValue());
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Value) }, sheet.GetCell(A5)->GetValue());

    CheckCache(true, A5, sheet);
    CheckCache(true, A4, sheet);
    CheckCache(true, A3, sheet);
    CheckCache(true, A2, sheet);
    CheckCache(false, A1, sheet); // A1 has string type

    SetCellValue("=ZZZZ13"s, A1, sheet);

    CheckCache(false, A5, sheet);
    CheckCache(false, A4, sheet);
    CheckCache(false, A3, sheet);
    CheckCache(false, A2, sheet);
    CheckCache(false, A1, sheet);

    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Ref) }, sheet.GetCell(A1)->GetValue());
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Ref) }, sheet.GetCell(A2)->GetValue());
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Ref) }, sheet.GetCell(A3)->GetValue());
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Ref) }, sheet.GetCell(A4)->GetValue());
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Ref) }, sheet.GetCell(A5)->GetValue());

    CheckCache(true, A5, sheet);
    CheckCache(true, A4, sheet);
    CheckCache(true, A3, sheet);
    CheckCache(true, A2, sheet);
    CheckCache(true, A1, sheet);

    SetCellValue("=1/0"s, A1, sheet);

    CheckCache(false, A5, sheet);
    CheckCache(false, A4, sheet);
    CheckCache(false, A3, sheet);
    CheckCache(false, A2, sheet);
    CheckCache(false, A1, sheet);

    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Div0) }, sheet.GetCell(A1)->GetValue());
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Div0) }, sheet.GetCell(A2)->GetValue());
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Div0) }, sheet.GetCell(A3)->GetValue());
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Div0) }, sheet.GetCell(A4)->GetValue());
    std::visit(CellValueChecker{ FormulaError(FormulaError::Category::Div0) }, sheet.GetCell(A5)->GetValue());
}

// -----------------------------------------------------------------------------

}  // namespace

void Tests() {
    TestRunner tr;

    RUN_TEST(tr, TestPosition);
    RUN_TEST(tr, TestSimpleCircularDependecies);
    RUN_TEST(tr, TestSimpleDependecies);
    RUN_TEST(tr, TestCommonCases);
    RUN_TEST(tr, TestErrorDependencies);
}
