CC = avr-gcc
CFLAGS = -Wall -Wextra -Wno-array-bounds -Os -DF_CPU=16000000UL -mmcu=atmega328p
LDFLAGS = -mmcu=atmega328p
DEV = /dev/ttyACM0

uart.hex: uart.elf
	avr-objcopy -O ihex -R .eeprom $^ $@

uart.elf: uart.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	$(RM) uart.o uart.elf uart.hex

dump: uart.elf
	avr-objdump -d $^

flash: uart.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P $(DEV) -b 115200 -U flash:w:$<:i

.PHONY: clean dump flash
