#pragma once

#include <optional>

#include "common.h"
#include "formula.h"
#include <unordered_set>

class Cell;

namespace cell_detail {

class CellValueInterface {
public:
    using Value = CellInterface::Value;

    enum class CellValueType {
        Empty,
        Text,
        Formula
    };

public:
    explicit CellValueInterface(CellValueType type)
        : type_(type) {
    }
    virtual ~CellValueInterface() = default;
    virtual Value GetValue() const = 0;
    virtual Value GetRawValue() const = 0;
    virtual std::string GetText() const = 0;
    CellValueType GetCellValueType() const {
        return type_;
    }

private:
    CellValueType type_;
};

class EmptyCellValue : public CellValueInterface {
public:
    EmptyCellValue()
        : CellValueInterface(CellValueInterface::CellValueType::Empty) {
    }

    Value GetValue() const override {
        return 0.0;
    }

    Value GetRawValue() const override {
        return 0.0;
    }

    std::string GetText() const override {
        return std::string();
    }
};

class TextCellValue : public CellValueInterface {
public:
    TextCellValue(std::string text)
        : CellValueInterface(CellValueInterface::CellValueType::Text)
        , text_(std::move(text)) {
    }

    Value GetValue() const  override {
        return (!text_.empty() && text_.front() == ESCAPE_SIGN) ? std::string(text_.begin() + 1, text_.end()) : text_;
    }

    Value GetRawValue() const override {
        return text_;
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
    FormulaCellValue(std::string text, Cell* self, SheetInterface& sheet)
        : CellValueInterface(CellValueInterface::CellValueType::Formula)
        , formula_(ParseFormula(std::move(text)))
        , sheet_(sheet) {
    }

    Value GetValue() const  override {
        if (!cache_value_) {
            cache_value_ = std::visit(CellValueConverter{}, formula_->Evaluate(sheet_));
        }
        return *cache_value_;
    }

    Value GetRawValue() const override {
        return GetValue();
    }

    std::string GetText() const override {
        return '=' + formula_->GetExpression();
    }

    void ResetCache() const {
        cache_value_.reset();
    }

    std::vector<Position> GetReferencedCells() const {
        return formula_->GetReferencedCells();
    }

    bool IsCacheValid() const {
        return cache_value_.has_value();
    }

private:
    std::unique_ptr<FormulaInterface> formula_;
    SheetInterface& sheet_;
    mutable std::optional<Value> cache_value_;
};

} // namespace cell_detail

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet);

    Cell(std::string text, SheetInterface& sheet);

    ~Cell();

    void Set(std::string text);

    void Clear();

    Value GetValue() const override;

    Value GetRawValue() const;

    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;

    bool IsReferenced() const;

    bool IsCacheValie() const;

private:
    void CreateReferencedCellsInPlace(std::vector<Position>& referenced_cells, std::unordered_set<const Cell*>& visited_cells) const;

    bool HasCellCircularDependency(const Cell* const self, const std::unique_ptr<cell_detail::CellValueInterface>& current_cell_value, std::unordered_set<const Cell*>& visited_cells) const;

    void InvalidateBindingCache(std::unordered_set<const Cell*>& visited_cells) const;

    void InvalidateCache() const;

    void UnbindReferencedDependency() const;

    void UnbindCell(const Cell* cell) const {
        binding_cells_.erase(cell);
    }

    void BindCell(const Cell* cell) const {
        binding_cells_.insert(cell);
    }

    void BindingReferencedDependency() const;

private:
    std::unique_ptr<cell_detail::CellValueInterface> CreateCell(std::string text);

private:
    SheetInterface& sheet_;
    std::unique_ptr<cell_detail::CellValueInterface> cell_value_;
    mutable std::unordered_set<const Cell*> binding_cells_;
};
