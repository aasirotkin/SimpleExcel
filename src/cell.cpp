#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

Cell::Cell(SheetInterface& sheet)
    : Cell("", sheet) {
}

Cell::Cell(std::string text, SheetInterface& sheet)
    : sheet_(sheet) {
    Set(std::move(text));
}

Cell::~Cell() {
}

void Cell::Set(std::string text) {
    std::unique_ptr<cell_detail::CellValueInterface> new_cell_value = CreateCell(std::move(text));
    std::unordered_set<const Cell*> visited_cells = { this };
    if (HasCellCircularDependency(this, new_cell_value, visited_cells)) {
        throw CircularDependencyException("Cell has circular dependency exception");
    }
    Clear();
    cell_value_ = std::move(new_cell_value);
    BindingReferencedDependency();
}

void Cell::Clear() {
    if (cell_value_) {
        std::unordered_set<const Cell*> visited_cells = { this };
        InvalidateBindingCache(visited_cells);
        UnbindReferencedDependency();
        cell_value_.reset();
    }
    cell_value_ = std::make_unique<cell_detail::EmptyCellValue>();
}

Cell::Value Cell::GetValue() const {
    return cell_value_->GetValue();
}

Cell::Value Cell::GetRawValue() const {
    return cell_value_->GetRawValue();
}

std::string Cell::GetText() const {
    return cell_value_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
    if (cell_value_->GetCellValueType() == cell_detail::CellValueInterface::CellValueType::Formula) {
        std::unordered_set<const Cell*> visited_cells;
        std::vector<Position> referenced_cells;
        CreateReferencedCellsInPlace(referenced_cells, visited_cells);
        return referenced_cells;
    }
    return {};
}

bool Cell::IsReferenced() const {
    if (cell_value_->GetCellValueType() == cell_detail::CellValueInterface::CellValueType::Formula) {
        return !dynamic_cast<cell_detail::FormulaCellValue*>(cell_value_.get())->GetReferencedCells().empty();
    }
    return false;
}

bool Cell::IsCacheValie() const {
    if (cell_value_->GetCellValueType() == cell_detail::CellValueInterface::CellValueType::Formula) {
        return dynamic_cast<cell_detail::FormulaCellValue*>(cell_value_.get())->IsCacheValid();
    }
    return false;
}

void Cell::CreateReferencedCellsInPlace(std::vector<Position>& referenced_cells, std::unordered_set<const Cell*>& visited_cells) const {
    if (cell_value_->GetCellValueType() == cell_detail::CellValueInterface::CellValueType::Formula) {
        for (const Position& pos : dynamic_cast<cell_detail::FormulaCellValue*>(cell_value_.get())->GetReferencedCells()) {
            const Cell* cell = dynamic_cast<const Cell*>(sheet_.GetCell(pos));
            if (!visited_cells.count(cell)) {
                referenced_cells.push_back(pos);
                visited_cells.insert(cell);
                cell->CreateReferencedCellsInPlace(referenced_cells, visited_cells);
            }
        }
    }
}

bool Cell::HasCellCircularDependency(const Cell* const self, const std::unique_ptr<cell_detail::CellValueInterface>& current_cell_value, std::unordered_set<const Cell*>& visited_cells) const {
    if (current_cell_value->GetCellValueType() == cell_detail::CellValueInterface::CellValueType::Formula) {
        for (const Position& pos : dynamic_cast<cell_detail::FormulaCellValue*>(current_cell_value.get())->GetReferencedCells()) {
            const Cell* cell = dynamic_cast<const Cell*>(sheet_.GetCell(pos));
            if (self == cell) {
                return true;
            }
            if (!cell) {
                sheet_.SetCell(pos, "");
                cell = dynamic_cast<const Cell*>(sheet_.GetCell(pos));
            }
            if (!visited_cells.count(cell)) {
                visited_cells.insert(cell);
                if (cell->HasCellCircularDependency(self, cell->cell_value_, visited_cells)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void Cell::InvalidateBindingCache(std::unordered_set<const Cell*>& visited_cells) const {
    for (const Cell* cell : binding_cells_) {
        if (!visited_cells.count(cell)) {
            visited_cells.insert(cell);
            cell->InvalidateCache();
            cell->InvalidateBindingCache(visited_cells);
        }
    }
}

void Cell::InvalidateCache() const {
    if (cell_value_->GetCellValueType() == cell_detail::CellValueInterface::CellValueType::Formula) {
        dynamic_cast<cell_detail::FormulaCellValue*>(cell_value_.get())->ResetCache();
    }
}

void Cell::UnbindReferencedDependency() const {
    if (cell_value_->GetCellValueType() == cell_detail::CellValueInterface::CellValueType::Formula) {
        for (const Position& pos : dynamic_cast<cell_detail::FormulaCellValue*>(cell_value_.get())->GetReferencedCells()) {
            const Cell* cell = dynamic_cast<const Cell*>(sheet_.GetCell(pos));
            cell->UnbindCell(this);
        }
    }
}

void Cell::BindingReferencedDependency() const {
    if (cell_value_->GetCellValueType() == cell_detail::CellValueInterface::CellValueType::Formula) {
        for (const Position& pos : dynamic_cast<cell_detail::FormulaCellValue*>(cell_value_.get())->GetReferencedCells()) {
            const Cell* cell = dynamic_cast<const Cell*>(sheet_.GetCell(pos));
            cell->BindCell(this);
        }
    }
}

std::unique_ptr<cell_detail::CellValueInterface> Cell::CreateCell(std::string text) {
    if (!text.empty()) {
        if (text.size() > 1 && text.front() == FORMULA_SIGN) {
            return std::make_unique<cell_detail::FormulaCellValue>(std::string(text.begin() + 1, text.end()), this, sheet_);
        }
        else {
            return std::make_unique<cell_detail::TextCellValue>(std::move(text));
        }
    }
    return std::make_unique<cell_detail::EmptyCellValue>();
}
