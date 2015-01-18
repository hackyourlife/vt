#!/bin/make
export	PREFIX	:=	avr-
export	CC	:=	$(PREFIX)gcc
export	LD	:=	$(CC)
export	OBJCOPY	:=	$(PREFIX)objcopy
export	NM	:=	$(PREFIX)nm
export	SIZE	:=	$(PREFIX)size

#-------------------------------------------------------------------------------
.SUFFIXES:
#-------------------------------------------------------------------------------
TARGET	:=	firmware
BUILD	:=	build

MCU	:=	atmega328p
F_CPU	:=	16000000L

CFLAGS	:=	-Os -g0 -funsigned-bitfields -fpack-struct
CFLAGS	+=	-fshort-enums -Wall -std=gnu99
CFLAGS	+=	-ffunction-sections -fdata-sections
CFLAGS	+=	-mmcu=$(MCU)
LDFLAGS	:=	-Wl,-x -Wl,--gc-sections -mmcu=$(MCU)

CFLAGS	+=	-DF_CPU=$(F_CPU)

CFILES	:=	vt.c vt52.c tek4014.c fnt4x6.c ILI9340.c gx.c usart.c main.c

ifneq ($(BUILD),$(notdir $(CURDIR)))
#-------------------------------------------------------------------------------
export	DEPSDIR	:=	$(CURDIR)/$(BUILD)
export	OFILES	:=	$(CFILES:.c=.o)
export	TFILES	:=	$(TARGET).hex $(TARGET).lst $(TARGET).sz
export	VPATH	:=	$(CURDIR)

.PHONY: $(BUILD) clean analysis demo all

$(BUILD):
	@echo compiling...
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

demo:
	gcc -o test -D __DEMO__ vt.c vt52.c tek4014.c fnt4x6.c gx.c demo.c

clean:
	rm -rf $(BUILD) $(TFILES) $(OFILES)

$(TARGET): $(TFILES)

else

#-------------------------------------------------------------------------------
# main target
#-------------------------------------------------------------------------------
.PHONY: all

all: $(TFILES)

$(TARGET).hex: $(TARGET).elf

$(TARGET).elf: $(OFILES)

%.lst: %.elf
	@echo "[NM]    $(notdir $@)"
	@$(NM) --size-sort -S $< > $@

%.sz: %.elf
	@echo "[SIZE]  $(notdir $@)"
	@$(SIZE) $< > $@

%.o: %.c
	@echo "[CC]    $(notdir $@)"
	@$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) -c $< -o $@
	@$(CC) -S $(CFLAGS) -o $(@:.o=.s) $< # create assembly file

%.elf:
	@echo "[LD]    $(notdir $@)"
	@$(LD) $(LDFLAGS) $(OFILES) -o $@ -Wl,-Map=$(@:.elf=.map)

%.hex: %.elf
	@echo "[HEX]   $(notdir $@)"
	@$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

-include $(DEPSDIR)/*.d

#-------------------------------------------------------------------------------
endif
#-------------------------------------------------------------------------------
