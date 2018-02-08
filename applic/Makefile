##
# ARM IP interface - uC/OS-II
#
#  Makefile uC/OS-II voor het testboard
# ============================================================================
# output settings
NAME            = uCOS
ELF             = $(NAME).elf
HEX             = $(NAME).hex
MAP             = $(NAME).map
LNK             = Linkerscript

# path-settings
PREFIX          = @
 # Die '@' hiervoor zorgt ervoor dat het eigenlijke compileer-commando niet zichtbaar is op het scherm
 # Je krijft dan dus alleen de warnings e/o errors te zien; maakt het wat overzichtelijker.
AS              = $(PREFIX)arm-thumb-elf-as
CC              = $(PREFIX)arm-thumb-elf-gcc
CPP             = $(PREFIX)arm-thumb-elf-g++
OBJCOPY         = $(PREFIX)arm-thumb-elf-objcopy
INSIGHT         = $(PREFIX)arm-thumb-elf-insight
READELF         = $(PREFIX)arm-thumb-elf-readelf

# ============================================================================
# Sourcefiles
SRC             = ./../uc-source
PORT_SRC        = ./../ports
DRIVER_SRC      = ./../ports/drivers

# welke sourcesfiles
SRC_FILES		= main.o
SRC_FILES		+= mutex.o
SRC_FILES       += malloc.o

# welke drivers moeten we compileren?
drivers         = exceptions.o
drivers        += vic.o
drivers        += buzzer.o
drivers        += leds.o
drivers        += pll.o
drivers        += uart.o
drivers        += delay.o
drivers        += lcd.o
drivers        += print.o
#drivers        += toetsen.o
#drivers        += timer.o
#drivers        += spi.o
# ============================================================================

# Set compiler options
INCLUDES        = -I $(PORT_SRC) -I $(SRC) -I $(DRIVER_SRC) -I ./
DEFINES         = -D__CPU_MODE__=0
WARNINGSETTINGS = -Wall -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wsign-compare \
                  -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wunused
CFLAGS          = -g -O2 -pipe $(WARNINGSETTINGS) -mcpu=arm7tdmi -mtune=arm7tdmi -mstructure-size-boundary=32 \
                  -fno-builtin $(INCLUDES) $(DEFINES)
LDFLAGS         = -Wl,-Map,$(MAP),-T,$(LNK) -nostartfiles

driver_objects=$(patsubst %.o,$(PORT_SRC)/drivers/%.o,$(drivers))

# ============================================================================
all: $(ELF)
#	$(READELF) -h $(ELF)

$(ELF): $(PORT_SRC)/os_cpu_a.o $(PORT_SRC)/os_cpu_c.o $(SRC)/ucos_ii.o \
        $(PORT_SRC)/bsp.o $(PORT_SRC)/os_dbg.o $(driver_objects) $(SRC_FILES) crt0.o
	$(CC) $(LDFLAGS) $(PORT_SRC)/os_cpu_a.o $(PORT_SRC)/os_cpu_c.o $(PORT_SRC)/os_dbg.o \
	                 $(SRC)/ucos_ii.o $(PORT_SRC)/bsp.o $(driver_objects) $(SRC_FILES) -o $(ELF)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $(ELF) $(HEX)

$(PORT_SRC)/os_cpu_a.o: $(PORT_SRC)/os_cpu_a.s
	$(CC) $(CFLAGS) -c $(PORT_SRC)/os_cpu_a.s -o $(PORT_SRC)/os_cpu_a.o

crt0.o: crt0.s
	$(CC) $(CFLAGS) -c crt0.s

debug: $(ELF)
	$(if $(shell pslist | grep -i OcdLib), , cmd /c start OcdLibRemote --cpu ARM7 --device WIGGLER --speed 1)
	$(INSIGHT) $(ELF)

clean:
	rm -f $(wildcard *.o) $(wildcard $(SRC)/*.o) $(wildcard $(PORT_SRC)/*.o) $(MAP) $(ELF) $(HEX)

realclean:clean
	rm -f $(driver_objects)
