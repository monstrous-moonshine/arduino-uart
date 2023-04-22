CC = avr-gcc
CFLAGS = -Wall -Wextra -Wno-array-bounds -Os -DF_CPU=16000000UL -mmcu=atmega328p

uart.hex: uart.elf
	avr-objcopy -O ihex -R .eeprom $^ $@

uart.elf: uart.o
	$(CC) -o $@ $^

clean:
	$(RM) uart.o uart.elf uart.hex

dump: uart.hex
	avr-objdump -b ihex -m avr5 -D $^

flash: uart.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:$<:i

.PHONY: clean dump flash
