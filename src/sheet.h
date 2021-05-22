#pragma once

#include "cell.h"
#include "common.h"
#include "position.h"

#include <functional>
#include <vector>

class Sheet : public SheetInterface {
public:
    using SheetList = std::vector<std::vector<std::unique_ptr<CellInterface>>>;

    Sheet();

    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:
    void CheckPosInPlace(Position pos) const;
    void ResizeSheetList(Position pos);
    Size CreatePrintableSize() const;

private:
    template <typename Func>
    void Printer(std::ostream& output, Func func) const {
        Size sz = GetPrintableSize();
        for (int r = 0; r < sz.rows; ++r) {
            bool is_first = true;
            for (int c = 0; c < sz.cols; ++c) {
                if (!is_first) {
                    output << '\t';
                }
                is_first = false;
                if (const auto* ptr_value = GetCell({ r, c })) {
                    func(ptr_value);
                }
            }
            output << '\n';
        }
    }

private:
    SheetList sheet_list_;
};

// -----------------------------------------------------------------------------

// Создаёт готовую к работе пустую таблицу.
std::unique_ptr<SheetInterface> CreateSheet();
