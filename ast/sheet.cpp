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
        cell->Set(std::move(text));
    }
    else {
        ResizeSheetList(pos);
        sheet_list_.at(pos.row).at(pos.col) = std::make_unique<Cell>(std::move(text));
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    return GetCellImpl(pos);
}
CellInterface* Sheet::GetCell(Position pos) {
    return GetCellImpl(pos);
}

void Sheet::ClearCell(Position pos) {
    CheckPosInPlace(pos);
    if (GetCell(pos)) {
        delete sheet_list_.at(pos.row).at(pos.col).release();
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
        sheet_list_.resize(pos.row + 1);
    }
    if (sheet_list_.at(pos.row).size() <= static_cast<size_t>(pos.col)) {
        sheet_list_.at(pos.row).resize(pos.col + 1);
    }
}

CellInterface* Sheet::GetCellImpl(Position pos) const {
    CheckPosInPlace(pos);
    if (sheet_list_.size() <= pos.row) {
        return nullptr;
    }
    if (sheet_list_.at(pos.row).size() <= pos.col) {
        return nullptr;
    }
    return sheet_list_.at(pos.row).at(pos.col).get();
}

Size Sheet::CreatePrintableSize() const {
    int row = 0;
    int col = 0;

    int local_row = 0;
    for (const auto& row_value : sheet_list_) {
        local_row++;
        int local_col = 0;
        for (const auto& col_value : row_value) {
            local_col++;
            if (col_value) {
                if (local_col > col) {
                    col = local_col;
                }
                row = local_row;
            }
        }
    }

    return { row, col };
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}
