#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

Cell::Cell()
    : impl_(std::make_unique<EmptyImpl>()) {
}

Cell::~Cell() {
}

void Cell::Set(std::string text) {
    if (!text.empty()) {
        if (impl_.get()) {
            impl_.release();
        }
        if (text.size() > 1 && text.front() == FORMULA_SIGN) {
            impl_ = std::make_unique<FormulaImpl>(std::string(text.begin() + 1, text.end()));
        }
        else {
            impl_ = std::make_unique<TextImpl>(std::move(text));
        }
    }
    else {
        Clear();
    }
}

void Cell::Clear() {
    impl_.release();
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}
