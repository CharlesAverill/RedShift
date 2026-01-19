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

CC=~/Desktop/llvm-mos/bin/mos-nes-cnrom-clang
CA=ca65

C_FILES := $(wildcard $(SRC)/*.c)
S_FILES := $(filter-out $(SRC)/crt0.S,$(wildcard $(SRC)/*.S))

ASSET_FILES := $(wildcard $(ASSETS)/*)
BGM := $(BUILD)/bgm.o
SFX := $(BUILD)/sfx.o

# Flags
INCLUDE_DIRS := $(INC)
INCLUDE := $(foreach dir, $(INCLUDE_DIRS), -I$(dir))

CFLAGS := $(INCLUDE) -Os -flto -lneslib -lfamitone2 -lnesdoug
# LDFLAGS := 

build: $(NAME).nes
$(NAME).nes: $(C_FILES) $(ASSET_FILES) $(BGM) $(SFX)
	$(CC) $(CFLAGS) $(C_FILES) $(S_FILES) $(BGM) $(SFX) -o $@

$(BGM): $(ASSETS)/music/bgm.txt
	mkdir -p $(BUILD)
	$(WINE) $(LIB)/famitone/text2data/text2vol5_2025.exe $< -ca65
	mv $(ASSETS)/music/bgm.s $(BUILD)
	sed -i 's/bgm_music_data:/.export bgm_music_data\nbgm_music_data:/g' $(BUILD)/bgm.s
	$(CA) $(BUILD)/bgm.s -o $@

$(SFX): $(ASSETS)/sfx/sfx.nsf
	mkdir -p $(BUILD)
	$(WINE) $(LIB)/famitone/nsf2data/nsf2data5.exe $< -ca65
	mv $(ASSETS)/sfx/sfx.s $(BUILD)
	sed -i 's/sounds:/.export sounds\nsounds:/g' $(BUILD)/sfx.s
	$(CA) $(BUILD)/sfx.s -o $@

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
