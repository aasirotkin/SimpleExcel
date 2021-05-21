#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::Sheet() {
}

Sheet::~Sheet() {
}

void Sheet::SetCell(Position pos, std::string text) {
    CheckPosInPlace(pos);

    if (auto* cell = GetCell(pos)) {
        dynamic_cast<Cell*>(cell)->Set(std::move(text));
    }
    else {
        ResizeSheetList(pos);
        sheet_list_.at(pos.row).at(pos.col) = std::make_unique<Cell>(std::move(text), *this);
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    return const_cast<Sheet*>(this)->GetCell(pos);
}
CellInterface* Sheet::GetCell(Position pos) {
    CheckPosInPlace(pos);
    if (sheet_list_.size() <= static_cast<size_t>(pos.row)) {
        return nullptr;
    }
    if (sheet_list_.at(pos.row).size() <= static_cast<size_t>(pos.col)) {
        return nullptr;
    }
    return sheet_list_.at(pos.row).at(pos.col).get();
}

void Sheet::ClearCell(Position pos) {
    CheckPosInPlace(pos);
    if (GetCell(pos)) {
        sheet_list_.at(pos.row).at(pos.col).reset();
    }
}

Size Sheet::GetPrintableSize() const {
    return CreatePrintableSize();
}

void Sheet::PrintValues(std::ostream& output) const {
    Size sz = GetPrintableSize();
    for (int r = 0; r < sz.rows; ++r) {
        bool is_first = true;
        for (int c = 0; c < sz.cols; ++c) {
            if (!is_first) {
                output << '\t';
            }
            is_first = false;
            if (const auto* ptr_value = GetCell({ r, c })) {
                std::visit([&](const auto& x) { output << x; }, ptr_value->GetValue());
            }
        }
        output << '\n';
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    Size sz = GetPrintableSize();
    for (int r = 0; r < sz.rows; ++r) {
        bool is_first = true;
        for (int c = 0; c < sz.cols; ++c) {
            if (!is_first) {
                output << '\t';
            }
            is_first = false;
            if (const auto* ptr_value = GetCell({ r, c })) {
                output << ptr_value->GetText();
            }
        }
        output << '\n';
    }
}

void Sheet::CheckPosInPlace(Position pos) const {
    if (!pos.IsValid()) {
        using namespace std;
        std::stringstream ss;
        ss << "Position is invalid "s << pos.ToString();
        throw InvalidPositionException(ss.str());
    }
}

void Sheet::ResizeSheetList(Position pos) {
    if (sheet_list_.size() <= static_cast<size_t>(pos.row)) {
        int new_row = pos.row + 1;
        sheet_list_.resize(new_row);
    }
    if (sheet_list_.at(pos.row).size() <= static_cast<size_t>(pos.col)) {
        int new_col = pos.col + 1;
        sheet_list_.at(pos.row).resize(new_col);
    }
}

Size Sheet::CreatePrintableSize() const {
    int row = 0;
    int col = 0;

    int local_row = 1;
    for (const auto& row_value : sheet_list_) {
        int local_col = 1;
        for (const auto& col_value : row_value) {
            if (col_value) {
                if (local_col > col) {
                    col = local_col;
                }
                row = local_row;
            }
            local_col++;
        }
        local_row++;
    }

    return { row, col };
}

// -----------------------------------------------------------------------------

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}
