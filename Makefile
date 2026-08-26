.PHONY: all clean build yychr famitracker mesen fceux nesst
.SUFFIXES:
MAKEFLAGS += -r

NAME := RedShift
build: $(NAME).nes $(CC) $(AS) $(LD) $(CL)

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

CC=$(TOOLS)/cc65/bin/cc65
AS=$(TOOLS)/cc65/bin/ca65
LD=$(TOOLS)/cc65/bin/ld65
CL=$(TOOLS)/cc65/bin/cl65

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

C_FILES := $(call rwildcard,$(SRC),%.c)
S_FILES := $(filter-out $(SRC)/crt0.S,$(wildcard $(SRC)/*.S))
O_FILES := $(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(C_FILES))

ASSET_FILES := $(wildcard $(ASSETS)/*)
BGM := $(BUILD)/bgm.S
SFX := $(BUILD)/sfx.S

# Flags
INCLUDE_DIRS := $(INC) $(LIB)/neslib $(LIB)/famitone
INCLUDE := $(foreach dir, $(INCLUDE_DIRS), -I$(dir))

CFLAGS := $(INCLUDE) -Oirs --add-source -W +error -W -pointer-sign --standard cc65 -g
ASFLAGS := -g $(INCLUDE)
LDFLAGS := -C $(CFG) $(BUILD)/crt0.o $(O_FILES) nes.lib \
		   -Ln $(BUILD)/labels.txt --dbgfile $(BUILD)/dbg.txt

$(NAME).nes: $(O_FILES) $(BUILD)/crt0.o $(CFG) $(ASSET_FILES)
	$(LD) $(LDFLAGS) --dbgfile $(NAME).dbg -o $(NAME).nes

$(BUILD)/crt0.o: $(CL) $(wildcard $(LIB)/neslib/*.s $(LIB)/neslib/*.sinc) $(BGM) $(SFX) $(ASSET_FILES)
	$(CL) -t nes -Oisr -c $(SRC)/crt0.S
	@mv $(SRC)/crt0.o $(BUILD)

$(BUILD)/%.S: $(SRC)/%.c $(CC)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD)/%.o: $(BUILD)/%.S $(AS)
	$(AS) $(ASFLAGS) $< -o $@

$(BGM): $(ASSETS)/music/bgm.txt
	$(WINE) $(LIB)/famitone/text2data/text2vol5_2025.exe $< -ca65
	mv $(ASSETS)/music/bgm.s $@

$(SFX): $(ASSETS)/sfx/sfx.nsf
	$(WINE) $(LIB)/famitone/nsf2data/nsf2data5.exe $< -ca65
	mv $(ASSETS)/sfx/sfx.s $@

yychr:
	cd $(YYCHR) && $(WINE) YYCHR.exe

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

cc65: $(CC) $(AS) $(LD) $(CL)
$(CC) $(AS) $(LD) $(CL):
	cd $(TOOLS)/cc65 && make -j

all: build mesen
