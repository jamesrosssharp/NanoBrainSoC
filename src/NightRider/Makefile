
all: nightrider.vhd

nightrider.vhd: nightrider.bin ../Templates/AlteraRomTemplate.vhd
	bin2vhd -t ../Templates/AlteraRomTemplate.vhd \
			-o nightrider.vhd nightrider.bin

nightrider.bin: nightrider.asm ../../bin/nbasm
	nbasm -t bin -o nightrider.bin nightrider.asm

disas: nightrider.bin
	nbobjdump -t bin nightrider.bin | less

