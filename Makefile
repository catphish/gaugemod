all: bootloader.bin program.bin pc_loader
bootloader.bin: bootloader.s bootloader.ld
	m68hc11-gcc -O0 -nostdlib bootloader.s -T bootloader.ld -o bootloader.elf
	m68hc11-objcopy -Obinary bootloader.elf bootloader.bin
program.bin: program.c program.h program.ld
	m68hc11-gcc -nostartfiles -fomit-frame-pointer program.c -T program.ld -o program.elf
	m68hc11-objcopy -Obinary program.elf program.bin
pc_loader: pc_loader.c
	gcc pc_loader.c -o pc_loader
install: bootloader.bin pc_loader
	./pc_loader /dev/ttyUSB0 bootloader.bin program.bin
clean:
	rm -f *.elf *.bin pc_loader
