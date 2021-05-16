#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

Cell::Cell()
    : impl_(std::make_unique<EmptyImpl>()) {
}

Cell::Cell(std::string text)
    : impl_(CreateCell(std::move(text))) {
}

Cell::~Cell() {
}

void Cell::Set(std::string text) {
    impl_ = CreateCell(std::move(text));
}

void Cell::Clear() {
    if (auto* cell_ptr = impl_.release()) {
        delete cell_ptr;
    }
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

std::unique_ptr<Impl> Cell::CreateCell(std::string text) {
    if (!text.empty()) {
        if (text.size() > 1 && text.front() == FORMULA_SIGN) {
            return std::make_unique<FormulaImpl>(std::string(text.begin() + 1, text.end()));
        }
        else {
            return std::make_unique<TextImpl>(std::move(text));
        }
    }
    return std::make_unique<EmptyImpl>();
}
