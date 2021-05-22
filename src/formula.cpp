#include "formula.h"

#include "cell.h"
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

struct FormulaValueGetter {
    double operator() (const std::string& value) {
        try {
            return std::stod(value);
        }
        catch (...) {
            throw FormulaError(FormulaError::Category::Value);
        }
    }

    double operator() (double value) {
        return value;
    }

    double operator() (const FormulaError& value) {
        throw value;
    }
};

class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression)
        : ast_(FormulaCreator(std::move(expression)))
        , referenced_cells_(ast_.GetCells().begin(), ast_.GetCells().end()) {
    }

    Value Evaluate(const SheetInterface& sheet) const override {
        try {
            return ast_.Execute(
                [&sheet](const Position& pos) {
                    const auto* cell = dynamic_cast<const Cell*>(sheet.GetCell(pos));
                    return std::visit(FormulaValueGetter{}, cell->GetRawValue());
                }
            );
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
        std::istringstream in(std::move(expression));
        return ParseFormulaAST(in);
    }

private:
    FormulaAST ast_;
    std::vector<Position> referenced_cells_;
};

class FormulaRefError : public FormulaInterface {
public:
    Value Evaluate(const SheetInterface& sheet) const override {
        return FormulaError(FormulaError::Category::Ref);
    }

    std::string GetExpression() const override {
        return {};
    }

    std::vector<Position> GetReferencedCells() const override {
        return {};
    }
};

}  // namespace

// -----------------------------------------------------------------------------

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch (const FormulaError& e) {
        return std::make_unique<FormulaRefError>();
    }
    catch (const std::exception& e) {
        throw FormulaException(e.what());
    }
}
