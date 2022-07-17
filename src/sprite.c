#include "sprite.h"

void sprite_init(sprite_t *p_sprite, SDL_Texture *p_sheet, SDL_Rect *p_src, int p_w, int p_h) {
	p_sprite->sheet =  p_sheet;
	p_sprite->src   = *p_src;

	p_sprite->w = p_w;
	p_sprite->h = p_h;
}
