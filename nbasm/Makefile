
all: nbasm

nbasm:	nbasm.tab.c lex.yy.c AST.cpp AST.h Expression.h Expression.cpp Statement.cpp Statement.h Symbol.h ../common/types.h Assembly.h Assembly.cpp
	g++ nbasm.tab.c lex.yy.c AST.cpp Expression.cpp Statement.cpp Assembly.cpp -std=c++11 -lfl -I../common -o nbasm

nbasm.tab.c: nbasm.y
	bison -d nbasm.y

lex.yy.c: nbasm.lex
	flex nbasm.lex

install: nbasm
	cp nbasm ../bin
