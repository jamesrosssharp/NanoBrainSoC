
#pragma once

#include "syntax.h"

void handleNewTopLevel();
void pushType(const char* type);

void handleFunctionDeclaration(char* name);
void handleArguments1();
void handleArguments2();
void handleArguments3();
void handleVariable(char *name);
void handleBlock1();
void handleBlock2();
void handleBlock3();
void handleReturnStatement();
void handleAssignment(char* sym);
void handleDeclaration(char* sym);
void handleWhile1();
void handleWhile2();
void handleExpressionSymbol(char *symbol);
void handleExpressionIntImmediate(int immVal);
void handleExpressionCharLiteral(char* charLit);
void handleExpressionArith(Syntax::BinaryExpressionType type);
void handleExpressionAddition();
void handleExpressionSubtraction();
void handleExpressionMultiplication();
void handleExpressionDivision();
void handleExpressionShiftLeft();
void handleExpressionShiftRight();
void handleExpressionFunctionCall(char* symbol);
void handleParameter1();
void handleParameter2();
void handleParameter3();
void handleAsm1();
void handleAsm2();
void handleStringLiteral1(char* string);
void handleStringLiteral2(char* string);
void handleDecorator(Syntax::DecoratorType type);
void handleDecoratorList1();
void handleDecoratorList2();
void handleDecoratorList3();
void handleReg1(char* reg);
void handleReg2(char* reg, char* sym);
void handleRegList1();
void handleRegList2();
void handleRegList3();
void handleGroupedExpression();

void handleExpressionBitwiseAnd();
void handleExpressionBitwiseOr();
void handleExpressionBitwiseXor();

void handleIf1();
void handleIf2();

void handleIfElse1();
void handleIfElse2();
void handleIfElse3();
void handleIfElse4();

void handleExpressionPostIncrement(char *symbol);
void handleExpressionPostDecrement(char *symbol);
void handleExpressionPreIncrement(char *symbol);
void handleExpressionPreDecrement(char *symbol);

void handleDoWhile();
void handleForLoop1();
void handleForLoop2();

void handleLessThan();
void handleLessThanEqual();
void handleGreaterThan();
void handleGreaterThanEqual();
void handleEqual();
void handleNotEqual();

void handleSwitch();
void handleDefault();
void handleBreak();
void handleCase();

void handleLogicalAnd();
void handleLogicalOr();

void handleLogicalNot();
void handleBitwiseNot();
void handleUnaryNegation();
