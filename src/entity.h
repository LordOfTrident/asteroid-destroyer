#ifndef PLAYER_H__HEADER_GUARD_
#define PLAYER_H__HEADER_GUARD_

#include <stdlib.h> /* size_t */
#include <math.h>   /* cos, sin */
#include <SDL2/SDL.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include "anim.h"

/* #define ENTITY_DEBUG_HITBOX */

#define DIR_UP     0
#define DIR_LEFT  -90
#define DIR_DOWN   180
#define DIR_RIGHT  90
#define DIR_ENTITY_ANGLE (unsigned short)-1

typedef struct {
	float x, y;
	int   w, h;
	float angle;
	int   health, state;

	SDL_Rect collision_box;

	float velocity, speed; /* pixels per frame */
	float friction;

	anim_t anim;
} entity_t;

void entity_init(entity_t *p_entity, SDL_Rect *p_rect, int p_health,
                 float p_speed, float p_friction);
void entity_render(entity_t *p_entity, SDL_Renderer *p_renderer);
void entity_rotate(entity_t *p_entity, float p_by);
void entity_set_angle(entity_t *p_entity, float p_angle);
void entity_change_velocity(entity_t *p_entity, float p_by);
void entity_move(entity_t *p_entity, float p_dir);

#endif
