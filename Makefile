.PHONY: all clean build yychr famitracker mesen fceux nesst
.SUFFIXES:
MAKEFLAGS += -r

NAME := RedShift

# Paths
TOOLS := tools
YYCHR := $(TOOLS)/yychr
SRC := src
INC := include
LIB := lib
BUILD := build
CFG := nes.cfg
ASSETS := assets

# Tools
WINE := wine
MESEN := mesen
FCEUX := fceux
NESST := $(TOOLS)/NESst

CC=cc65
AS=ca65
LD=ld65
CL=cl65

C_FILES := $(wildcard $(SRC)/*.c)
O_FILES := $(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(C_FILES))

ASSETS := $(wildcard $(ASSETS)/*)

# Flags
INCLUDE_DIRS := $(INC) $(LIB)/neslib $(LIB)/famitone
INCLUDE := $(foreach dir, $(INCLUDE_DIRS), -I$(dir))

CFLAGS := $(INCLUDE) -Oirs --add-source -W -pointer-sign --standard cc65
ASFLAGS := -g $(INCLUDE)
LDFLAGS := -C $(CFG) $(BUILD)/crt0.o $(O_FILES) nes.lib \
		   -Ln $(BUILD)/labels.txt --dbgfile $(BUILD)/dbg.txt

build: $(NAME).nes
$(NAME).nes: $(O_FILES) $(BUILD)/crt0.o $(CFG) $(ASSETS)
	$(LD) $(LDFLAGS) -o $(NAME).nes

$(BUILD)/crt0.o: $(wildcard $(LIB)/neslib/*.s $(LIB)/neslib/*.sinc)
	$(CL) -t nes -Oisr -c $(SRC)/crt0.S
	@mv $(SRC)/crt0.o $(BUILD)

$(BUILD)/%.o: $(BUILD)/%.S
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD)/%.S: $(SRC)/%.c
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $< -o $@

yychr:
	$(WINE) $(YYCHR)/YYCHR.exe

famitracker:
	$(WINE) $(TOOLS)/FamiTracker.exe

mesen: $(NAME).nes
	$(MESEN) $(NAME).nes

fceux: $(NAME).nes
	$(FCEUX) $(NAME).nes

nesst:
	$(WINE) $(NESST)/NESst.exe

nesifier:
	$(WINE) $(TOOLS)/NESIFIER.exe

clean:
	rm -rf build

all: build mesen
