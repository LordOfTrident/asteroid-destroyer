#ifndef SPRITE_H__HEADER_GUARD__
#define SPRITE_H__HEADER_GUARD__

#include <SDL2/SDL.h>

typedef struct {
	int w, h;

	SDL_Rect     src;
	SDL_Texture *sheet;
} sprite_t;

void sprite_init(sprite_t *p_sprite, SDL_Texture *p_sheet, SDL_Rect *p_src, int p_w, int p_h);

#endif
