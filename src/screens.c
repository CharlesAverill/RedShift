#include "types.h"
#include "screens/screens.h"
#include "screens/title_screen.h"
#include "screens/bg.h"
#include "geometry.h"
#include "palette.h"
#include "neslib.h"
#include "nesdoug.h"
#include "sound.h"
#include "objects/ship.h"
#include "string.h"

const val title_screen_palettes[] = {
    BLACK, 0x05, 0x10, WHITE,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

const val play_screen_palettes[] = {
    BLACK, 0x05, 0x03, 0x3C,
    GRAYSCALE,
    0,0,0,0,
    0,0,0,0
};

const val play_sprite_palettes[] = {
    SHIP_PALETTE,
    GRAYSCALE,
    EXPLOSION_PALETTE, // also bullets
    0, 0, 0, 0
};

static const val font_base_tiles[ScreenEnd] = {0xC0, 0xC1, 0xC1};

static val i;
static val x, y;
static unsigned char buf[32];
static unsigned char c;
void write_centered_text_bg(const char *str, val start_y, val len, val ascii_offset)
{
    y = start_y;

    // convert characters to tile indices
    for (i = 0; i < len && str[i]; ++i) {
        c = (unsigned char)str[i];   // safe cast
        if (c == ' ' || c == '\0')
            buf[i] = ascii_offset;    // map space
        else
            buf[i] = (c - ' ' + ascii_offset);
    }

    // compute horizontal centering
    x = (TILE_X_MAX - len) >> 1;

    // enqueue VRAM write
    multi_vram_buffer_horz(buf, len, NTADR_A(x, y));
}

const val press_start[] = "PRESS START\0";
const val charles_averill[] = "CHARLES AVERILL\0";
const val charles_systems[] = "CHARLES.SYSTEMS\0";

const val game_over_str[] = "GAME OVER\0";
const char * const game_over_msgs[] = {
    "THAT'S ALL?\0",
    "WHAT A LIFE\0",
    "THE UNIVERSE DID NOT NOTICE\0",
    "IT ALL KEEPS GOING\0",
    "SOMEWHERE SOMETHING CONTINUES\0",
    "TIME PASSES\0",
    "ALL OUTCOMES CONVERGE\0",
    "YOU MADE IT THIS FAR\0",
    "THIS WAS SUFFICIENT\0",
    "THE STARS PERSIST\0",
    "YOUR PART IS COMPLETE\0",
    "THIS WAS ALWAYS AN OPTION\0",
    "WILL IT EVER BE ENOUGH?\0",
    "WHAT COMES NEXT?\0",
    "A DROP IN THE BUCKET\0",
    // "INPUT NO LONGER REQUIRED\0"
};
const val num_go_msgs = sizeof(game_over_msgs) / sizeof(game_over_msgs[0]);

routine(setup_title_screen) {
    clear_vram_buffer();
    set_vram_buffer();

    ppu_mask(DEFAULT_MASK | RED_MASK);
    bank_bg(0);
    pal_bg(title_screen_palettes);
    vram_adr(NAMETABLE_A);
    vram_unrle(title_screen);
    music_play(BGM_PASSOU);
    pal_bright(4);

    write_centered_text_bg(press_start, TILE_Y_MID + 5, strlen(press_start), font_base_tiles[TitleScreen]);
    write_centered_text_bg(charles_averill, TILE_Y_MID + 12, strlen(charles_averill), font_base_tiles[TitleScreen]);
    write_centered_text_bg(charles_systems, TILE_Y_MID + 14, strlen(charles_systems), font_base_tiles[TitleScreen]);
}

routine(setup_play_screen) {
    clear_vram_buffer();
    set_vram_buffer();

    bank_bg(1);
    pal_bg(play_screen_palettes);
    vram_adr(NAMETABLE_A);
    vram_unrle(bg);

    // Prepare to draw sprites
    bank_spr(0);
    pal_spr(play_sprite_palettes);
}

static char const* msg;
routine(setup_game_over_screen) {
    clear_vram_buffer();
    set_vram_buffer();

    // Draw game over text
    write_centered_text_bg(game_over_str, TILE_Y_MID, strlen(game_over_str), font_base_tiles[GameOverScreen]);
    msg = game_over_msgs[rand8() % num_go_msgs];
    write_centered_text_bg(msg, TILE_Y_MID + 3, strlen(msg), font_base_tiles[GameOverScreen]);

    write_centered_text_bg(press_start, TILE_Y_MID + 9, strlen(press_start), font_base_tiles[GameOverScreen]);
}

void set_screen(Screen s) {
    switch (s) {
        case TitleScreen:
            setup_title_screen();
            break;
        case MainPlayScreen:
            setup_play_screen();
            break;
        case GameOverScreen:
            setup_game_over_screen();
            break;
    }
}
