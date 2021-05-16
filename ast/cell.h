#pragma once

#include "common.h"
#include "formula.h"

class Impl : public CellInterface {
public:
    void Set(std::string) override {
    }
};

class EmptyImpl : public Impl {
public:
    Value GetValue() const  override {
        return Value{};
    }

    std::string GetText() const override {
        return std::string();
    }
};

class TextImpl : public Impl {
public:
    TextImpl(std::string text)
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

struct ValueConverter {
    CellInterface::Value operator() (double d) {
        return d;
    }

    CellInterface::Value operator() (FormulaError fe) {
        return fe;
    }
};

class FormulaImpl : public Impl {
public:
    FormulaImpl(std::string text)
        : formula_(ParseFormula(std::move(text))) {
    }

    Value GetValue() const  override {
        return std::visit(ValueConverter{}, formula_->Evaluate());
    }

    std::string GetText() const override {
        return '=' + formula_->GetExpression();
    }

private:
    std::unique_ptr<FormulaInterface> formula_;
};

class Cell : public CellInterface {
public:
    Cell();

    ~Cell();

    void Set(std::string text);

    void Clear();

    Value GetValue() const override;

    std::string GetText() const override;

private:
    std::unique_ptr<Impl> impl_;
};
