#include "cell.h"
#include "common.h"
#include "formula.h"
#include "FormulaAST.h"
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
    }
    catch (const FormulaError& fe) {
        ASSERT_EQUAL(fe.what(), "DIV/0");
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
    std::cout << std::get<FormulaError>(switch_text->GetValue()) << std::endl;
}

int main() {
    TestRunner tr;

    RUN_TEST(tr, AstFormulaTest);
    RUN_TEST(tr, ParseFormulaTest);
    RUN_TEST(tr, CellTest);

    std::cout << "Tests Passed" << std::endl;

    return 0;
}
