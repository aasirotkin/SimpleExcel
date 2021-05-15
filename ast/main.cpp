#include "common.h"
#include "formula.h"
#include "FormulaAST.h"
#include "test_runner_p.h"

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

int main() {
    using namespace std::string_literals;

    ASSERT_EQUAL(ExecuteASTFormula("1"s), 1.0);
    ASSERT_EQUAL(ExecuteASTFormula("1+2*3-4/5"s), 6.2);
    try {
        ExecuteASTFormula("1/0"s);
    } catch (const FormulaError& fe) {
        std::cout << fe.what() << std::endl;
    }

    ParseFormulaTest();

    std::cout << "Tests Passed" << std::endl;
    return 0;
}