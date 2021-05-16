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
    using ColToPtrValue = std::map<int, const CellInterface*>;
    using SheetListValues = std::map<int, ColToPtrValue>;

private:
    void CheckPosInPlace(Position pos) const;
    void ResizeSheetList(Position pos);
    Size CreatePrintableSize() const;

private:
    SheetList sheet_list_;
};

// Создаёт готовую к работе пустую таблицу.
std::unique_ptr<SheetInterface> CreateSheet();
