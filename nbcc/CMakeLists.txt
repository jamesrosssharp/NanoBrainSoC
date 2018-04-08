project(nbcc)
cmake_minimum_required(VERSION 2.8)

set(SRC_LIST nbcc.y nbcc.l main.cpp syntaxstack.cpp syntax.cpp parsehandlers.cpp
             codegenerator.cpp expression.cpp codegendefs.h intrep.cpp expressionhelper.cpp
             types.h variablestore.cpp functionstore.cpp intrepcompiler.cpp)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(BISON)
bison_target(parser nbcc.y ${CMAKE_CURRENT_BINARY_DIR}/gen-nbcc-parser.cpp)

find_package(FLEX)
flex_target(lexer nbcc.l ${CMAKE_CURRENT_BINARY_DIR}/gen-nbcc-lexer.cpp)

ADD_FLEX_BISON_DEPENDENCY(lexer parser)

include_directories(${CMAKE_CURRENT_BINARY_DIR} "./")

add_custom_target(edit_test SOURCES test.c README.md)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${BISON_parser_OUTPUTS} ${FLEX_lexer_OUTPUTS})


