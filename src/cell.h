#pragma once

#include <optional>

#include "common.h"
#include "formula.h"

namespace cell_detail {

class CellValueInterface {
public:
    using Value = CellInterface::Value;
    virtual ~CellValueInterface() = default;
    virtual Value GetValue() const = 0;
    virtual std::string GetText() const = 0;
};

class EmptyCellValue : public CellValueInterface {
public:
    Value GetValue() const override {
        return 0.0;
    }

    std::string GetText() const override {
        return std::string();
    }
};

class TextCellValue : public CellValueInterface {
public:
    TextCellValue(std::string text)
        : text_(std::move(text)) {
    }

    Value GetValue() const  override {
        return (!text_.empty() && text_.front() == ESCAPE_SIGN) ? std::string(text_.begin() + 1, text_.end()) : text_;
    }

    std::string GetText() const override {
        return text_;
    }

private:
    std::string text_;
};

struct CellValueConverter {
    CellInterface::Value operator() (double d) {
        return d;
    }

    CellInterface::Value operator() (FormulaError fe) {
        return fe;
    }
};

class FormulaCellValue : public CellValueInterface {
public:
    FormulaCellValue(std::string text, const SheetInterface& sheet)
        : formula_(ParseFormula(std::move(text)))
        , sheet_(sheet) {
        //TODO: fill referenced_cells_ and binding_cells_
    }

    Value GetValue() const  override {
        if (!cache_value_) {
            cache_value_ = std::visit(CellValueConverter{}, formula_->Evaluate(sheet_));
        }
        return *cache_value_;
    }

    std::string GetText() const override {
        return '=' + formula_->GetExpression();
    }

    void ResetCache() const {
        cache_value_.reset();
    }

    const std::vector<Position>& GetReferencedCells() const {
        return referenced_cells_;
    }

    const std::vector<Position>& GetBindingCells() const {
        return binding_cells_;
    }

private:
    std::unique_ptr<FormulaInterface> formula_;
    const SheetInterface& sheet_;
    std::vector<Position> referenced_cells_;
    std::vector<Position> binding_cells_;
    mutable std::optional<Value> cache_value_;
};

} // namespace cell_detail

class Cell : public CellInterface {
public:
    Cell(const SheetInterface& sheet);

    Cell(std::string text, const SheetInterface& sheet);

    ~Cell();

    void Set(std::string text) override;

    void Clear();

    Value GetValue() const override;

    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override {
        if (const cell_detail::FormulaCellValue* formula_ptr = dynamic_cast<const cell_detail::FormulaCellValue*>(cell_value_.get())) {
            return formula_ptr->GetReferencedCells();
        }
        return {};
    }

    bool IsReferenced() const {
        return !GetReferencedCells().empty();
    }

private:
    std::unique_ptr<cell_detail::CellValueInterface> CreateCell(std::string text);

private:
    const SheetInterface& sheet_;
    std::unique_ptr<cell_detail::CellValueInterface> cell_value_;
};
