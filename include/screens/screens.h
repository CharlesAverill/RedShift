#ifndef SCREENS
#define SCREENS

#include "types.h"

typedef enum Screen {
    TitleScreen,
    MainPlayScreen,
    GameOverScreen,
    ScreenEnd
} Screen;

/**
 * @brief Set up a new screen. Assumes CNROM banking has already been performed
 * 
 * @param s Screen type to transition to
 */
void set_screen(Screen s);

#endif
