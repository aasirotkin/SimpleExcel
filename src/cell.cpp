#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

Cell::Cell(const SheetInterface& sheet)
    : sheet_(sheet)
    , cell_value_(std::make_unique<cell_detail::EmptyCellValue>()) {
}

Cell::Cell(std::string text, const SheetInterface& sheet)
    : sheet_(sheet)
    , cell_value_(CreateCell(std::move(text))) {
}

Cell::~Cell() {
}

void Cell::Set(std::string text) {
    cell_value_ = CreateCell(std::move(text));
}

void Cell::Clear() {
    cell_value_.reset();
    cell_value_ = std::make_unique<cell_detail::EmptyCellValue>();
}

Cell::Value Cell::GetValue() const {
    return cell_value_->GetValue();
}

std::string Cell::GetText() const {
    return cell_value_->GetText();
}

std::unique_ptr<cell_detail::CellValueInterface> Cell::CreateCell(std::string text) {
    if (!text.empty()) {
        if (text.size() > 1 && text.front() == FORMULA_SIGN) {
            return std::make_unique<cell_detail::FormulaCellValue>(std::string(text.begin() + 1, text.end()), sheet_);
        }
        else {
            return std::make_unique<cell_detail::TextCellValue>(std::move(text));
        }
    }
    return std::make_unique<cell_detail::EmptyCellValue>();
}
