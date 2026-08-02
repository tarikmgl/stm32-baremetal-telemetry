CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 -mthumb -O0 -g -ffreestanding -nostdlib
LDFLAGS = -T linker.ld -nostdlib

SRCS = startup.s main.c uart.c telemetry.c

all: firmware.bin

firmware.elf: $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary $< $@

flash: firmware.elf
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program firmware.elf verify reset exit"

clean:
	rm -f *.o *.elf *.bin