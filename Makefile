#############################\
General Variables\
#############################
ODIR = build
SDIR = src
IDIR = include
PROJ = main
PORT = COM9
UPLOAD_RATE = 115200
CLOCK = 16000000
DEPS = $(ODIR)/USART.o

#############################\
Compiler Variables\
#############################
CC = avr-gcc
CFLAGS = -Wall
CFLAGS +=  -Os 
CFLAGS +=-mmcu=atmega328p
CFLAGS += -DF_CPU=$(CLOCK)UL

#############################\
Object Copy Flags and Variables\
#############################
OBJ_COPY=avr-objcopy
COPY_FLAGS = -O
COPY_FLAGS += ihex
COPY_FLAGS += -R
COPY_FLAGS += .eeprom

#############################\
AVR-dude (Flashing tool) variables and flags\
#############################
FLASHER = avrdude
FLASH_FLAGS = -F
FLASH_FLAGS += -V
FLASH_FLAGS += -c arduino
FLASH_FLAGS += -p m328p
FLASH_FLAGS += -P $(PORT)
FLASH_FLAGS += -b $(UPLOAD_RATE)
FLASH_FLAGS += -U
FLASH_FILES = flash:w:$(ODIR)/$(PROJ).hex

#############################\
Rules\
#############################

default : all

all: begin clean build flash end

begin:
	@echo ---------Begin---------

end:
	@echo ---------End---------

hexdump: $(ODIR)/$(PROJ).hex ;

flash: $(ODIR)/$(PROJ).hex
	@echo ---------Flashing---------
	sudo $(FLASHER) $(FLASH_FLAGS) $(FLASH_FILES)
	
build: $(ODIR)/$(PROJ).elf ;

dump: $(ODIR)/$(PROJ).dump ;
#	@echo ---------Dumping Assembly to $<---------

$(ODIR)/$(PROJ).hex : $(ODIR)/$(PROJ).elf
	@echo ---------Object Copy to $@---------
	$(OBJ_COPY) $(COPY_FLAGS) $< $@

$(ODIR)/%.o: $(SDIR)/%.c
	@echo ---------Building $@---------
	$(CC) $(CFLAGS) -c -o $@ $<

$(ODIR)/$(PROJ).elf: $(ODIR)/$(PROJ).o  $(DEPS)
	$(CC) $(CFLAGS) $(ODIR)/$(PROJ).o $(DEPS) -o $(ODIR)/$(PROJ).elf

$(ODIR)/$(PROJ).dump:$(ODIR)/$(PROJ).hex
	@echo ---------Dumping Assembly to $@---------
	avr-objdump -D -s -m avr5  $< > $@

clean:
	@echo ---------Cleaning---------
	del /f $(ODIR)\*.o 
	del /f $(ODIR)\*.hex
	del /f $(ODIR)\*.elf
	del /f $(ODIR)\*.dump
	
#############################\
Phony Targets\
#############################
.PHONY: build clean