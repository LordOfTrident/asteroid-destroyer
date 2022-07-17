#ifndef GAME_H__HEADER_GUARD__
#define GAME_H__HEADER_GUARD__

#include <stdlib.h>  /* exit, EXIT_FAILURE, EXIT_SUCCESS, NULL, size_t, rand, srand */
#include <stdio.h>   /* snprintf */
#include <string.h>  /* strcmp, memset, strlen */
#include <stdbool.h> /* bool, true, false */
#include <math.h>    /* sin */
#include <assert.h>  /* assert */
#include <time.h>    /* time, time_t */
#include <SDL2/SDL.h>

#include "anim.h"
#include "entity.h"
#include "text.h"
#include "particles.h"

/* cheats for debugging */
/* #define GAME_PLAYER_INVINCIBLE */

#define GAME_TITLE "Asteroid Destroyer"

#define SCREEN_W 248
#define SCREEN_H 204

#define FPS_CAP 60

#define PLAYER_W 15
#define PLAYER_H 24

#define MAX_ASSETS    8
#define MAX_BULLETS   16
#define MAX_ASTEROIDS 32

typedef struct {
	SDL_Texture *texture;
	SDL_Surface *surface;

	const char *key;
} asset_t;

typedef enum {
	GAME_STATE_MENU = 0,
	GAME_STATE_INGAME,
	GAME_STATE_DIED,
	GAME_STATE_FAILED
} game_state_t;

typedef struct {
	SDL_Window   *window;
	SDL_Renderer *renderer;
	SDL_Texture  *screen;
	SDL_Rect      screen_rect;

	const char *title;

	SDL_Event    event;
	const Uint8 *key_states;

	bool   running;
	size_t tick, fps;
	game_state_t state;

	/* i do not need to malloc all of these lists, a static array will do it */
	asset_t assets[MAX_ASSETS];
	size_t  assets_count;

	SDL_Rect map;
	scrolling_t back_layers[3];

	font_t  font, font_small, font_big;
	label_t label;

	bool blink, paused;

	int score;

	entity_t player;
	int      player_shoot_timer;
	bool     player_shoot_from_left;
	entity_t bullets[MAX_BULLETS];
	size_t   bullets_count;

	entity_t asteroids[MAX_ASTEROIDS];
	size_t   asteroids_count;

	emitter_t particle_emitter;
} game_t;

void game_init(game_t *p_game);
void game_run(game_t *p_game);
void game_finish(game_t *p_game);

void game_load_asset(game_t *p_game, const char *p_path, const char *p_key);
asset_t *game_asset(game_t *p_game, const char *p_key);

void game_get_asteroid_sprite_info(int p_sprite, int *p_row, int *p_w, int *p_h);
int  game_get_asteroid_type(entity_t *p_asteroid);

void game_delete_asteroid(game_t *p_game, size_t p_idx);
void game_delete_bullet(game_t *p_game, size_t p_idx);

void game_reset_values(game_t *p_game);
void game_reset_player_values(game_t *p_game);

void game_emit_from(game_t *p_game, int p_x, int p_y, sprite_t *p_sprite, size_t p_amount,
                    bool p_random_size, float p_velocity, float p_friction, size_t p_lifetime);
void game_break_player(game_t *p_game);
void game_break_asteroid(game_t *p_game, entity_t *p_asteroid);

void game_explosion_at(game_t *p_game, int p_x, int p_y);
void game_explosion_at_player(game_t *p_game);
void game_explosion_at_asteroid(game_t *p_game, entity_t *p_asteroid);

void game_update(game_t *p_game);

void game_handle_events(game_t *p_game);
void game_handle_events_menu(game_t *p_game);
void game_handle_events_ingame(game_t *p_game);

void game_update_menu(game_t *p_game);
void game_update_back(game_t *p_game);
void game_update_entities(game_t *p_game);
void game_update_collision(game_t *p_game);

void game_render(game_t *p_game);
void game_render_menu(game_t *p_game);
void game_render_hud(game_t *p_game);
void game_render_back(game_t *p_game);
void game_render_entities(game_t *p_game);

#endif
