#pragma once

#include "common.h"
#include "FormulaAST.h"
#include "test_runner_p.h"

#include <memory>
#include <variant>

// �������, ����������� ��������� � ��������� �������������� ���������.
// �������������� �����������:
// * ������� �������� �������� � �����, ������: 1+2*3, 2.5*(2+3.5/7)
class FormulaInterface {
public:
    using Value = std::variant<double, FormulaError>;

    virtual ~FormulaInterface() = default;

    // ���������� ����������� �������� ������� ���� ������. �� ������ �����
    // �� ������� ������ 1 ��� ������ -- ������� �� 0.
    virtual Value Evaluate() const = 0;

    // ���������� ���������, ������� ��������� �������.
    // �� �������� �������� � ������ ������.
    virtual std::string GetExpression() const = 0;
};

// ������ ���������� ��������� � ���������� ������ �������.
// ������� FormulaException � ������, ���� ������� ������������� �����������.
std::unique_ptr<FormulaInterface> ParseFormula(std::string expression);
