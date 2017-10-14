#pragma once

#include <cstdint>

#include "Symbol.h"

enum class ExpressionElementType : uint32_t
{
    kNone = 0,
    kLeftParenthesis,
    kRightParenthesis,
    kPlus,
    kMinus,
    kDiv,
    kMult,
    kShiftLeft,
    kShiftRight,
    kAnd,
    kOr,
    kXor,
    kNot,
    kInt,
    kUInt,
    kString,
    kStringLiteral,
    kCharLiteral,
};

struct ExpressionElement
{
    ExpressionElementType elem;
    union
    {
        char* string;
        unsigned int uval;
        int     sval;
        float   fval;
    } v;
};

struct Expression
{
    void addElement(ExpressionElement& e)
    {
        elements.push_back(e);
    }

    std::vector<ExpressionElement> elements;

    void reset()
    {
        elements.clear();
        lineNum = 0;
    }

    uint32_t lineNum;

    bool evaluate(int32_t& value, SymbolTable& syms);

    int32_t doPlusMinus(int32_t accu, ExpressionElementType& op, int32_t value);

    Expression doMultDiv();
};

