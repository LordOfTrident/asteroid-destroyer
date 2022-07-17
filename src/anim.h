/* structures to handle scrolling backgrounds and entity animations */

#ifndef ANIM_H__HEADER_GUARD__
#define ANIM_H__HEADER_GUARD__

#include <stdlib.h> /* size_t */
#include <SDL2/SDL.h>

#include "sprite.h"

typedef struct {
	size_t frames, frame;
	int row;

	SDL_Texture *sheet;
} anim_t;

void anim_init(anim_t *p_anim, SDL_Texture *p_sheet, int p_row, size_t p_frames);
void anim_next(anim_t *p_anim, size_t p_min, size_t p_max);
void anim_set(anim_t *p_anim, size_t p_frame);

typedef struct {
	float scroll;
	int   screen_w, screen_h;

	sprite_t sprite;
} scrolling_t;

void scrolling_init(scrolling_t *p_scroll, sprite_t *p_sprite, int p_screen_w, int p_screen_h);
void scrolling_scroll(scrolling_t *p_scroll, float p_by);
void scrolling_render(scrolling_t *p_scroll, SDL_Renderer *p_renderer, int p_x, int p_y);

#endif
