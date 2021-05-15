#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <stdexcept>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}

namespace {

class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression)
        : ast_(FormulaCreator(std::move(expression))) {
    }

    Value Evaluate() const override {
        try {
            return ast_.Execute();
        }
        catch (FormulaError& e) {
            return std::move(e);
        }
        catch (...) {
            throw;
        }
    }

    std::string GetExpression() const override {
        std::ostringstream out;
        ast_.PrintFormula(out);
        return out.str();
    }

private:
    FormulaAST FormulaCreator(std::string expression) {
        try {
            std::istringstream in(std::move(expression));
            return ParseFormulaAST(in);
        }
        catch (const std::exception& e) {
            throw FormulaException(e.what());
        }
    }

private:
    FormulaAST ast_;
};

}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}
