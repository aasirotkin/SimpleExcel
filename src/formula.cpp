#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <stdexcept>

using namespace std::literals;

// -----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}

// -----------------------------------------------------------------------------

namespace {

class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression)
        : ast_(FormulaCreator(std::move(expression)))
        , referenced_cells_(ast_.GetCells().begin(), ast_.GetCells().end()) {
    }

    Value Evaluate(const SheetInterface& sheet) const override {
        try {
            return ast_.Execute([&sheet](const Position& pos) {
                //TODO: look through all cases
                return 0.0;
                });
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

    std::vector<Position> GetReferencedCells() const override {
        return referenced_cells_;
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
    std::vector<Position> referenced_cells_;
};

}  // namespace

// -----------------------------------------------------------------------------

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}
