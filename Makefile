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

build:
	mkdir -p build

# -------------------------
# longMode + Kernel (Stage 2)
# -------------------------
build/longMode.o: src/x86_16/bootloader/longmode.asm | build
	$(AS64) src/x86_16/bootloader/longmode.asm -o build/longMode.o


build/kernel.o: src/x86_64/kernel/main.cpp | build
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/main.cpp -o build/kernel.o


build/display.o: src/x86_64/kernel/modules/core/code/display.cpp | build
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/display.cpp -o build/display.o


build/lowlevelaccess.o: src/x86_64/kernel/modules/core/code/lowLevelAccess.cpp | build
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/lowLevelAccess.cpp -o build/lowlevelaccess.o


build/io.o: src/x86_64/kernel/modules/core/code/io.cpp | build
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/io.cpp -o build/io.o


build/console.o: src/x86_64/kernel/modules/core/code/console.cpp | build
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/console.cpp -o build/console.o


build/string.o: src/x86_64/kernel/modules/core/code/string.cpp | build
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/string.cpp -o build/string.o


build/allocator.o: src/x86_64/kernel/modules/core/code/allocator.cpp | build
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/allocator.cpp -o build/allocator.o


build/filesystem.o: src/x86_64/kernel/modules/core/code/fileSystem.cpp | build
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/fileSystem.cpp -o build/filesystem.o


build/math.o: src/x86_64/kernel/modules/core/code/math.cpp | build
	$(CC) $(CXXFLAGS) -c src/x86_64/kernel/modules/core/code/math.cpp -o build/math.o



build/longMode.elf: build/longMode.o build/kernel.o build/display.o build/lowlevelaccess.o build/io.o build/console.o build/string.o build/allocator.o build/filesystem.o build/math.o linker/longMode.ld | build
	$(LD64) -T linker/longMode.ld -o build/longMode.elf \
		build/longMode.o build/kernel.o build/display.o build/lowlevelaccess.o build/allocator.o build/io.o build/console.o build/string.o build/filesystem.o build/math.o


build/longMode.bin: build/longMode.elf | build
	objcopy -O binary build/longMode.elf build/longMode.bin

#(filesize(bytes)+511)/512 = sectors
LONGMODE_SIZE = $(shell stat -c%s build/longMode.bin)
LONGMODE_SECTORS = $(shell echo $$(( ($(LONGMODE_SIZE) + 511) / 512 )))


# -------------------------
# Bootloader (Stage 1)
# -------------------------

build/boot.o: src/x86_16/bootloader/boot.asm build/longMode.bin | build
	$(AS32) src/x86_16/bootloader/boot.asm -DLONGMODE_SECTORS=${LONGMODE_SECTORS} -o build/boot.o


build/boot.elf: build/boot.o linker/boot.ld | build
	$(LD32) -T linker/boot.ld -o build/boot.elf build/boot.o


build/boot.bin: build/boot.elf | build
	objcopy -O binary build/boot.elf build/boot.bin


# -------------------------
# Disk image layout
# -------------------------
myos.img: build/boot.bin build/longMode.bin
	dd if=/dev/zero of=myos.img bs=1M count=4
	dd if=build/boot.bin     of=myos.img conv=notrunc
	dd if=build/longMode.bin of=myos.img seek=1 conv=notrunc


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
	rm -rf build *.img