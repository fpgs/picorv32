# femtosoc firmware 

# path to gcc
TOOLCHAIN_PREFIX = /opt/riscv32i/bin/riscv32-unknown-elf-

femto_fw: main.c femto.lds femto.S efb.h i2c.c i2c.h ufm.c ufm.h disp.c disp.h 
	$(TOOLCHAIN_PREFIX)gcc -march=rv32ic -Wl,-Bstatic,-T,femto.lds,--strip-debug -ffreestanding -nostdlib -o femto_fw.elf femto.S main.c i2c.c ufm.c disp.c
	$(TOOLCHAIN_PREFIX)objcopy -O binary femto_fw.elf femto_fw.bin
	python3 ../../../firmware/makehex.py femto_fw.bin 32768 > femto_fw.mem

.PHONY: clean
clean:
	rm -f femto_fw.elf femto_fw.bin
	rm -f counter.elf counter.bin

