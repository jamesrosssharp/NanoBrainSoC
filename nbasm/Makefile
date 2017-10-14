
all: TestAsm

TestAsm:	TestAsm.tab.c lex.yy.c AST.cpp AST.h Expression.h Expression.cpp Statement.cpp Statement.h Symbol.h types.h
	g++ TestAsm.tab.c lex.yy.c AST.cpp Expression.cpp Statement.cpp -std=c++11 -lfl -o TestAsm

TestAsm.tab.c: TestAsm.y
	bison -d TestAsm.y

lex.yy.c: TestAsm.l
	flex TestAsm.l


