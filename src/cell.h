#pragma once

#include "common.h"
#include "formula.h"

namespace cell_detail {

class CellValue : public CellInterface {
public:
    void Set(std::string) override {
    }
};

class EmptyCellValue : public CellValue {
public:
    Value GetValue() const  override {
        return Value{};
    }

    std::string GetText() const override {
        return std::string();
    }
};

class TextCellValue : public CellValue {
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

class FormulaCellValue : public CellValue {
public:
    FormulaCellValue(std::string text)
        : formula_(ParseFormula(std::move(text))) {
    }

    Value GetValue() const  override {
        return std::visit(CellValueConverter{}, formula_->Evaluate());
    }

    std::string GetText() const override {
        return '=' + formula_->GetExpression();
    }

private:
    std::unique_ptr<FormulaInterface> formula_;
};

} // namespace cell_detail

class Cell : public CellInterface {
public:
    Cell();

    Cell(std::string text);

    ~Cell();

    void Set(std::string text) override;

    void Clear();

    Value GetValue() const override;

    std::string GetText() const override;

private:
    std::unique_ptr<cell_detail::CellValue> CreateCell(std::string text);

private:
    std::unique_ptr<cell_detail::CellValue> cell_value_;
};
