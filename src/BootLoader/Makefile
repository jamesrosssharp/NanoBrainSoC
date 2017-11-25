
all: bootloader.vhd

bootloader.vhd: bootloader.bin AlteraRomTemplate.vhd
	bin2vhd -t AlteraRomTemplate.vhd -o bootloader.vhd bootloader.bin

bootloader.bin: bootloader.asm ../../bin/nbasm
	nbasm -t bin -o bootloader.bin bootloader.asm

disas: bootloader.bin
	nbobjdump -t bin bootloader.bin | less

