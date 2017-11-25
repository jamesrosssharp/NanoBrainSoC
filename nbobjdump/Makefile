TOOL= nbobjdump
SOURCES= main.cpp BinFileDisassembler.cpp Disassembler.cpp
HEADERS= BinFileDisassembler.h Disassembler.h ../common/nbInstructionSet.h

all: $(TOOL)

$(TOOL): $(SOURCES) $(HEADERS)
	g++ -o $(TOOL) --std=c++11 -I../common $(SOURCES) 

run: $(TOOL)
	./$(TOOL) -t bin ../nbasm/Test.bin

install: $(TOOL) 
	cp $(TOOL) ../bin
