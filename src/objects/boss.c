#include "types.h"
#include "objects/boss.h"
#include "objects/celestial_bodies.h"
#include "objects/ship.h"
#include "objects/bullets.h"
#include "events.h"
#include "sound.h"
#include "utils.h"
#include "neslib.h"

extern const unsigned char boss_metasprite[];
extern const val* const explosion_list[];   // ship's explosion, reused for boss death

bool start_boss_encounter = false;
bool boss_active = false;
bool boss_invincible = false;

static val music_timer;
static bigval boss_x, boss_y;
static sbigval boss_vx, boss_vy;
static val boss_bounce_count;
static val boss_pause_timer;
static bool bounced;

static val boss_health;
static val boss_hit_timer;
static bigval boss_speed;
static bool boss_entering;
static bool boss_dying;
static bool boss_coasting;
static val boss_death_timer;
static val boss_anim;

static Rect boss_rect, other_rect;
static val boss_ms_buf[17];

#define BOSS_W        16
#define BOSS_H        16
#define SCREEN_W      256
#define SCREEN_H      240
#define BOSS_X_MAX    ((bigval)(SCREEN_W - BOSS_W) << 8)
#define BOSS_Y_MAX    ((bigval)(SCREEN_H - BOSS_H) << 8)
#define BOSS_SPEED    0x0140    // starting speed, 1.25 px/frame
#define BOSS_SPEEDUP  0x0060    // added to speed magnitude per hit

#define BOUNCES_PER_PAUSE 4
#define BOSS_PAUSE_FRAMES 90    // ~1.5s at 60fps

#define BOSS_MAX_HEALTH   3
#define BOSS_HIT_FRAMES   60    // ~1s invincibility / palette cycle after a hit
#define BOSS_DEATH_FRAMES 64    // length of the death explosion (~1s)
#define BOSS_DECEL        4     // speed lost per frame while coasting to a stop

#define BOSS_ENTER_X     ((bigval)120 << 8)
#define BOSS_ENTER_Y     ((bigval)40 << 8)
#define BOSS_ENTER_SPEED 0x0060

// Apply the current speed magnitude, preserving each axis's direction
static void boss_apply_speed(void) {
    boss_vx = (boss_vx < 0) ? -(sbigval)boss_speed : (sbigval)boss_speed;
    boss_vy = (boss_vy < 0) ? -(sbigval)boss_speed : (sbigval)boss_speed;
}

static void boss_new_direction(void) {
    boss_vx = (rand8() & 1) ? (sbigval)boss_speed : -(sbigval)boss_speed;
    boss_vy = (rand8() & 1) ? (sbigval)boss_speed : -(sbigval)boss_speed;
}

// Enter the invincibility window
static void boss_take_hit_window(void) {
    boss_hit_timer = BOSS_HIT_FRAMES;
    boss_invincible = true;
}

routine(Boss_init) {
    boss_active = false;
    boss_invincible = false;
    boss_entering = false;
    boss_dying = false;
    boss_coasting = false;
    start_boss_encounter = false;
    boss_x = BOSS_ENTER_X;
    boss_y = (bigval)16 << 8;
    boss_speed = BOSS_SPEED;
    boss_vx = boss_speed;
    boss_vy = boss_speed;
    boss_bounce_count = 0;
    boss_pause_timer = 0;
    boss_health = BOSS_MAX_HEALTH;
    boss_hit_timer = 0;
    boss_death_timer = 0;
    boss_anim = 0;
}

routine(trigger_boss_encounter) {
    start_boss_encounter = false;
    boss_active = true;
    destroy_all_bodies();

    enable_bgm = false;
    dim_bgm = true;
    music_timer = 128;

    disable_asteroid_spawns();

    // Cinematic entrance
    boss_entering = true;
    boss_invincible = true;
    boss_dying = false;
    boss_coasting = false;
    boss_x = BOSS_ENTER_X;
    boss_y = (bigval)(-((sbigval)BOSS_H << 8));
    boss_health = BOSS_MAX_HEALTH;
    boss_bounce_count = 0;
    boss_pause_timer = 0;
    boss_hit_timer = 0;
    boss_death_timer = 0;
}

routine(Boss_update) {
    if (!boss_active)
        return;

    if (music_timer && --music_timer == 0) {
        enable_bgm = true;
        music_play(BGM_KNIGHTS);
    }

    // Coast to a stop after the fatal hit
    if (boss_coasting) {
        boss_x += boss_vx;
        boss_y += boss_vy;

        // Bounce off the walls so it can't drift off-screen while slowing.
        if (boss_vx < 0) {
            if (boss_x > BOSS_X_MAX) { boss_x = 0; boss_vx = -boss_vx; }
        } else {
            if (boss_x >= BOSS_X_MAX) { boss_x = BOSS_X_MAX; boss_vx = -boss_vx; }
        }
        if (boss_vy < 0) {
            if (boss_y > BOSS_Y_MAX) { boss_y = 0; boss_vy = -boss_vy; }
        } else {
            if (boss_y >= BOSS_Y_MAX) { boss_y = BOSS_Y_MAX; boss_vy = -boss_vy; }
        }

        // Decelerate each axis toward zero.
        if (boss_speed > BOSS_DECEL)
            boss_speed -= BOSS_DECEL;
        else
            boss_speed = 0;
        boss_apply_speed();

        if (boss_speed == 0) {
            // Stopped: switch to the stationary explosion.
            boss_coasting = false;
            boss_dying = true;
            boss_death_timer = BOSS_DEATH_FRAMES;
            boss_anim = 0;
			sfx_play(SFX_EXPLOSION, SFX_CHANNEL);
        }
        return;
    }

    if (boss_dying) {
        if (--boss_death_timer == 0)
            end_boss_encounter();
        return;
    }

    // Entrance
    if (boss_entering) {
        if (music_timer == 0) {
            boss_y += BOSS_ENTER_SPEED;
            if ((sbigval)boss_y >= (sbigval)BOSS_ENTER_Y) {
                boss_y = BOSS_ENTER_Y;
                boss_entering = false;
                boss_invincible = false;
                boss_new_direction();
            }
        }
        return;
    }

    // Count down the invincibility window
    if (boss_hit_timer && --boss_hit_timer == 0)
        boss_invincible = false;

    // Hold still, then head off in a new random direction
    if (boss_pause_timer) {
        if (--boss_pause_timer == 0) {
            boss_new_direction();
            boss_bounce_count = 0;
        }
        return;
    }

    // Move
    boss_x += boss_vx;
    boss_y += boss_vy;

    // Bounce off the walls
    bounced = false;

    if (boss_vx < 0) {
        if (boss_x > BOSS_X_MAX) {
            boss_x = 0;
            boss_vx = (sbigval)boss_speed;
            bounced = true;
        }
    } else {
        if (boss_x >= BOSS_X_MAX) {
            boss_x = BOSS_X_MAX;
            boss_vx = -(sbigval)boss_speed;
            bounced = true;
        }
    }

    if (boss_vy < 0) {
        if (boss_y > BOSS_Y_MAX) {
            boss_y = 0;
            boss_vy = (sbigval)boss_speed;
            bounced = true;
        }
    } else {
        if (boss_y >= BOSS_Y_MAX) {
            boss_y = BOSS_Y_MAX;
            boss_vy = -(sbigval)boss_speed;
            bounced = true;
        }
    }

    if (bounced && ++boss_bounce_count >= BOUNCES_PER_PAUSE)
        boss_pause_timer = BOSS_PAUSE_FRAMES;

    // Collision
    boss_rect.x = boss_x >> 8;
    boss_rect.y = boss_y >> 8;
    boss_rect.width = BOSS_W;
    boss_rect.height = BOSS_H;

    // Player collision
    other_rect.x = (ship_x >> 8) + 8;
    other_rect.y = (ship_y >> 8) + 8;
    other_rect.width = 16;
    other_rect.height = 16;
    if (check_collision(&boss_rect, &other_rect)) {
        ship_damage();
        if (!boss_invincible)
            boss_take_hit_window();
    }

    // Bullet collision
    if (!boss_invincible) {
        val b;
        for (b = 0; b < n_bullets; ++b) {
            other_rect.x = bullets[b].x >> 8;
            other_rect.y = bullets[b].y >> 8;
            other_rect.width = 8;
            other_rect.height = 8;
            if (check_collision(&boss_rect, &other_rect)) {
                // consume the bullet
                bullets[b] = bullets[n_bullets - 1];
                --n_bullets;

                sfx_play(SFX_EXPLOSION, SFX_CHANNEL);

                if (--boss_health == 0) {
                    // Fatal hit: fade the music now, then coast to a stop
                    // before the explosion. No more hits.
                    boss_coasting = true;
                    boss_invincible = true;
                    enable_bgm = false;
                    dim_bgm = true;
                } else {
                    // Hurt but alive: flash and speed up.
                    boss_take_hit_window();
                    boss_speed += BOSS_SPEEDUP;
                    boss_apply_speed();
                }
                break;
            }
        }
    }

    // TODO: attacks
}

routine(end_boss_encounter) {
    boss_active = false;
    boss_invincible = false;
    boss_entering = false;
    boss_dying = false;
    boss_coasting = false;

    enable_asteroid_spawns();

    dim_bgm = false;
    enable_bgm = true;
    music_play(BGM_PASSOU);
}

render_routine(Boss) {
    val pal, k, oy;

    if (!boss_active)
        return sprid;

    if (boss_dying) {
        return oam_meta_spr((boss_x >> 8) + 4, boss_y >> 8, sprid,
                            explosion_list[++boss_anim % 32 < 16]);
    }

    oy = boss_y >> 8;

    if (boss_entering && oy >= (SCREEN_H - BOSS_H))
        return sprid;

    if (boss_hit_timer) {
        // Cycle sprite subpalettes 0..3 to flicker
        pal = (boss_hit_timer >> 2) & 3;
        for (k = 0; boss_metasprite[k] != 128; k += 4) {
            boss_ms_buf[k+0] = boss_metasprite[k+0];
            boss_ms_buf[k+1] = boss_metasprite[k+1];
            boss_ms_buf[k+2] = boss_metasprite[k+2];
            boss_ms_buf[k+3] = (boss_metasprite[k+3] & 0xfc) | pal;
        }
        boss_ms_buf[k] = 128;
        return oam_meta_spr(boss_x >> 8, oy, sprid, boss_ms_buf);
    }

    return oam_meta_spr(boss_x >> 8, oy, sprid, boss_metasprite);
}

const unsigned char boss_metasprite[]={
     8,  0,0x97,0|OAM_FLIP_H|3,
     8,  8,0xa7,0|OAM_FLIP_H|3,
     0,  8,0xa7,0|3,
     0,  0,0x97,0|3,
    128
};
