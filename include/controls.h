#ifndef CONTROLS_H
#define CONTROLS_H

#include "types.h"
#include "neslib.h"

extern val btn_down, btn_new;

#define PLAYER1 0
#define PLAYER2 1

#define read_inputs() { btn_new = pad_trigger(PLAYER1); btn_down = pad_state(PLAYER1); }
#define reset_inputs() { btn_new = 0; btn_down = 0; }

#define down(BTN) (btn_down & (PAD_ ## BTN))
#define triggered(BTN) (btn_new & (PAD_ ## BTN))

#endif
