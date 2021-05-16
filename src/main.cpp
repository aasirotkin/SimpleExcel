#include "cell.h"
#include "common.h"
#include "formula.h"
#include "FormulaAST.h"
#include "position.h"
#include "sheet.h"
#include "test_runner_p.h"

using namespace std::string_literals;

namespace {
double ExecuteASTFormula(const std::string& expression) {
    return ParseFormulaAST(expression).Execute();
}
}  // namespace

struct FormulaValueChecker {
    void operator()(double d) {
        ASSERT_EQUAL(d, reference_value);
        ASSERT(!is_fail);
    }

    void operator()(FormulaError&) {
        ASSERT(is_fail);
    }

    double reference_value;
    bool is_fail;
};

void AstFormulaTest() {
    ASSERT_EQUAL(ExecuteASTFormula("1"s), 1.0);
    ASSERT_EQUAL(ExecuteASTFormula("1+2*3-4/5"s), 6.2);
    try {
        ExecuteASTFormula("1/0"s);
        ASSERT(false);
    }
    catch (const FormulaError& fe) {
        ASSERT_EQUAL(std::string(fe.what()), "DIV/0");
    }
}

void ParseFormulaTest()
{
    using namespace std::string_literals;

    auto formula_1 = ParseFormula("3*4/2 - 2 * (3 - (1)) / ((2 - 1))"s);
    std::visit(FormulaValueChecker{ 2.0, false }, formula_1->Evaluate());
    ASSERT_EQUAL(formula_1->GetExpression(), "3*4/2-2*(3-1)/(2-1)"s);

    auto formula_2 = ParseFormula("3*4/2 - 2 * (3 - (1)) / ((1 - 1))"s);
    std::visit(FormulaValueChecker{ 0.0, true }, formula_2->Evaluate());
    ASSERT_EQUAL(formula_2->GetExpression(), "3*4/2-2*(3-1)/(1-1)"s);
}

inline std::ostream& operator<<(std::ostream& output, const CellInterface::Value& value) {
    std::visit([&](const auto& x) { output << x; }, value);
    return output;
}

std::unique_ptr<CellInterface> CreateCell(const std::string& str) {
    std::unique_ptr<CellInterface> cell = std::make_unique<Cell>();
    cell->Set(str);
    return cell;
}

void CellTest() {
    auto simple_text = CreateCell("simple_text");
    ASSERT_EQUAL(simple_text->GetText(), "simple_text");
    ASSERT_EQUAL(std::get<std::string>(simple_text->GetValue()), "simple_text");

    auto empty_apostroph = CreateCell("'");
    ASSERT_EQUAL(empty_apostroph->GetText(), "'");
    ASSERT_EQUAL(std::get<std::string>(empty_apostroph->GetValue()), "");

    auto apostroph = CreateCell("'apostroph");
    ASSERT_EQUAL(apostroph->GetText(), "'apostroph");
    ASSERT_EQUAL(std::get<std::string>(apostroph->GetValue()), "apostroph");

    auto text_formula = CreateCell("'=1+2");
    ASSERT_EQUAL(text_formula->GetText(), "'=1+2");
    ASSERT_EQUAL(std::get<std::string>(text_formula->GetValue()), "=1+2");

    auto empty_formula = CreateCell("=");
    ASSERT_EQUAL(empty_formula->GetText(), "=");
    ASSERT_EQUAL(std::get<std::string>(empty_formula->GetValue()), "=");

    auto formula = CreateCell("=1+2");
    ASSERT_EQUAL(formula->GetText(), "=1+2");
    ASSERT_EQUAL(std::get<double>(formula->GetValue()), 3);

    auto switch_text = CreateCell("1+2");
    ASSERT_EQUAL(switch_text->GetText(), "1+2");
    ASSERT_EQUAL(std::get<std::string>(switch_text->GetValue()), "1+2");

    switch_text->Set("=1+2");
    ASSERT_EQUAL(switch_text->GetText(), "=1+2");
    ASSERT_EQUAL(std::get<double>(switch_text->GetValue()), 3);

    switch_text->Set("=1/0");
    ASSERT_EQUAL(switch_text->GetText(), "=1/0");
    //std::cout << std::get<FormulaError>(switch_text->GetValue()) << std::endl;
}

inline std::ostream& operator<<(std::ostream& output, Position pos) {
    return output << "(" << pos.row << ", " << pos.col << ")";
}

inline Position operator"" _pos(const char* str, std::size_t) {
    return Position::FromString(str);
}

inline std::ostream& operator<<(std::ostream& output, Size size) {
    return output << "(" << size.rows << ", " << size.cols << ")";
}

namespace {

void TestEmpty() {
    auto sheet = CreateSheet();
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{0, 0}));
}

void TestInvalidPosition() {
    auto sheet = CreateSheet();
    try {
        sheet->SetCell(Position{-1, 0}, "");
    } catch (const InvalidPositionException&) {
    }
    try {
        sheet->GetCell(Position{0, -2});
    } catch (const InvalidPositionException&) {
    }
    try {
        sheet->ClearCell(Position{Position::MAX_ROWS, 0});
    } catch (const InvalidPositionException&) {
    }
}

void TestSetCellPlainText() {
    auto sheet = CreateSheet();

    auto checkCell = [&](Position pos, std::string text) {
        sheet->SetCell(pos, text);
        CellInterface* cell = sheet->GetCell(pos);
        ASSERT(cell != nullptr);
        ASSERT_EQUAL(cell->GetText(), text);
        ASSERT_EQUAL(std::get<std::string>(cell->GetValue()), text);
    };

    checkCell("A1"_pos, "Hello");
    checkCell("A1"_pos, "World");
    checkCell("B2"_pos, "Purr");
    checkCell("A3"_pos, "Meow");

    const SheetInterface& constSheet = *sheet;
    ASSERT_EQUAL(constSheet.GetCell("B2"_pos)->GetText(), "Purr");

    sheet->SetCell("A3"_pos, "'=escaped");
    CellInterface* cell = sheet->GetCell("A3"_pos);
    ASSERT_EQUAL(cell->GetText(), "'=escaped");
    ASSERT_EQUAL(std::get<std::string>(cell->GetValue()), "=escaped");
}

void TestClearCell() {
    auto sheet = CreateSheet();

    sheet->SetCell("C2"_pos, "Me gusta");
    sheet->ClearCell("C2"_pos);
    ASSERT(sheet->GetCell("C2"_pos) == nullptr);

    sheet->ClearCell("A1"_pos);
    sheet->ClearCell("J10"_pos);
}
void TestPrint() {
    auto sheet = CreateSheet();
    sheet->SetCell("A2"_pos, "meow");
    sheet->SetCell("B2"_pos, "=1+2");
    sheet->SetCell("A1"_pos, "=1/0");

    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{2, 2}));

    std::ostringstream texts;
    sheet->PrintTexts(texts);
    ASSERT_EQUAL(texts.str(), "=1/0\t\nmeow\t=1+2\n");

    std::ostringstream values;
    sheet->PrintValues(values);
    ASSERT_EQUAL(values.str(), "#DIV/0!\t\nmeow\t3\n");

    sheet->ClearCell("B2"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{2, 1}));
}

void TestSheet() {
    TestEmpty();
    TestInvalidPosition();
    TestSetCellPlainText();
    TestClearCell();
    TestPrint();
}

}  // namespace

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

void TestPosition() {
    TestEqualLess();
    TestIsValid();
    TestToString();
    TestFromString();
    TestPositionAndStringConversion();
    TestPositionToStringInvalid();
    TestStringToPositionInvalid();
}

}  // namespace

int main() {
    TestRunner tr;

    RUN_TEST(tr, TestPosition);
    RUN_TEST(tr, AstFormulaTest);
    RUN_TEST(tr, ParseFormulaTest);
    RUN_TEST(tr, CellTest);
    RUN_TEST(tr, TestSheet);

    return 0;
}
