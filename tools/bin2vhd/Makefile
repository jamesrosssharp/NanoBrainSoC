
all: bin2vhd

bin2vhd: main.cpp
	g++ -o bin2vhd --std=c++11 main.cpp

run: bin2vhd
	./bin2vhd -t AlteraRomTemplate.vhd -o ROM.vhd ../../nbasm/Test.bin

install: bin2vhd 
	cp bin2vhd ../../bin
