AS32 = nasm -f elf32
AS64 = nasm -f elf64
CC = g++
LD32 = ld -m elf_i386
LD64 = ld -m elf_x86_64

CXXFLAGS = \
-m64 -mno-red-zone -mcmodel=kernel \
-ffreestanding -fno-builtin \
-fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit \
-fno-stack-protector -fno-pic -fno-pie \
-O2 -g -fno-omit-frame-pointer \
-Wall -Wextra -Wshadow -Wconversion -Wundef \
-fno-use-cxa-atexit \
-std=c++23

all: myos.img

# -------------------------
# Bootloader (Stage 1)
# -------------------------
boot.o: src/x86_16/bootloader/boot.asm
	$(AS32) src/x86_16/bootloader/boot.asm -o boot.o

boot.elf: boot.o linker/boot.ld
	$(LD32) -T linker/boot.ld -o boot.elf boot.o

boot.bin: boot.elf
	objcopy -O binary boot.elf boot.bin


# -------------------------
# longMode + Kernel (Stage 2)
# -------------------------
longMode.o: src/x86_16/bootloader/longmode.asm
	$(AS64) src/x86_16/bootloader/longmode.asm -o longMode.o

kernel.o: src/x86_64/kernel/main.cpp
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/main.cpp -o kernel.o

display.o: src/x86_64/kernel/modules/core/code/display.cpp
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/display.cpp -o display.o

lowlevelaccess.o: src/x86_64/kernel/modules/core/code/lowLevelAccess.cpp
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/lowLevelAccess.cpp -o lowlevelaccess.o

io.o: src/x86_64/kernel/modules/core/code/io.cpp
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/io.cpp -o io.o

console.o: src/x86_64/kernel/modules/core/code/console.cpp
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/console.cpp -o console.o

string.o: src/x86_64/kernel/modules/core/code/string.cpp
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/string.cpp -o string.o

allocator.o: src/x86_64/kernel/modules/core/code/allocator.cpp
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/allocator.cpp -o allocator.o

filesystem.o: src/x86_64/kernel/modules/core/code/filesystem.cpp
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/filesystem.cpp -o filesystem.o

longMode.elf: longMode.o kernel.o display.o lowlevelaccess.o io.o console.o string.o allocator.o filesystem.o linker/longMode.ld
	$(LD64) -T linker/longMode.ld -o longMode.elf \
		longMode.o kernel.o display.o lowlevelaccess.o allocator.o io.o console.o string.o filesystem.o

longMode.bin: longMode.elf
	objcopy -O binary longMode.elf longMode.bin

#(filesize(bytes)+511)/512 = sectors

# -------------------------
# Disk image layout
# -------------------------
myos.img: boot.bin longMode.bin
	cat boot.bin longMode.bin > myos.img
	dd if=/dev/zero of=myos.img bs=1M count=4
	dd if=boot.bin     of=myos.img conv=notrunc
	dd if=longMode.bin of=myos.img seek=1 conv=notrunc

	make clean


# -------------------------
# Run in QEMU
# -------------------------
run:
	qemu-system-x86_64 \
	-machine q35,accel=tcg \
	-cpu qemu64 \
	-m 512M \
	-drive format=raw,file=myos.img \
	-display sdl


# -------------------------
# Cleanup
# -------------------------
clear:
	rm -f *.o *.bin *.elf *.img
clean:
	rm -f *.o *.elf
