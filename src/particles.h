#ifndef PARTICLES_H__HEADER_GUARD__
#define PARTICLES_H__HEADER_GUARD__

#include <stdlib.h>  /* exit, EXIT_FAILURE, size_t */
#include <assert.h>  /* assert */
#include <math.h>    /* cos, sin */
#include <stdbool.h> /* bool, true, false */
#include <SDL2/SDL.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include "sprite.h"

#define MAX_PARTICLES 256

typedef struct {
	float x, y;
	float angle, visual_angle;

	size_t life_timer; /* in frames */

	float velocity; /* pixels per frame */
	float friction;

	sprite_t sprite;
} particle_t;

void particle_init(particle_t *p_particle, sprite_t *p_sprite, float p_velocity, float p_friction,
                   int p_x, int p_y, size_t p_lifetime);
void particle_update(particle_t *p_particle);
void particle_render(particle_t *p_particle, SDL_Renderer *p_renderer);

typedef struct {
	particle_t particles[MAX_PARTICLES]; /* we will use a pool here instead */
} emitter_t;

void emitter_init(emitter_t *p_emitter);
void emitter_emit(emitter_t *p_emitter, size_t p_amount, sprite_t *p_sprite, bool p_random_size,
                  float p_velocity, float p_friction, int p_x, int p_y, size_t p_lifetime);
void emitter_update(emitter_t *p_emitter);
void emitter_render(emitter_t *p_emitter, SDL_Renderer *p_renderer);

#endif
