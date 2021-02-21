target ?= segment.elf
src = main.c bcd.c pcf8523.c sep.c tlc59108.c twi.c
obj = $(src:%c=obj/%o)

CC = avr-gcc
CFLAGS = -mmcu=attiny412 -gdwarf -Os -DF_CPU=20000000ul -std=gnu11 -Wall -Werror
LDFLAGS = -mmcu=attiny412 -gdwarf -Os
AVRDUDE_FLAGS = -p t412 -c jtag3updi

$(target): $(obj)
	$(CC) $(LDFLAGS) -o $@ $^
	avr-objdump -P mem-usage $@

obj/%.o: %.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(obj) $(target)

reset:
	avrdude $(AVRDUDE_FLAGS)

flash: $(target)
	avrdude $(AVRDUDE_FLAGS) -e -U flash:w:$<

fuse:
	false
	#avrdude $(AVRDUDE_FLAGS) -U fuse2:w:0x1:m

dis: $(target)
	avr-objdump -S $< | less

dwarf: $(target)
	avr-objdump --dwarf=info $< | less

.PHONY: clean reset flash fuse dis dwarf
