#include "anim.h"

void anim_init(anim_t *p_anim, SDL_Texture *p_sheet, int p_row, size_t p_frames) {
	p_anim->sheet  = p_sheet;
	p_anim->row    = p_row;
	p_anim->frames = p_frames;
	p_anim->frame  = 0;
}

void anim_next(anim_t *p_anim, size_t p_min, size_t p_max) {
	++ p_anim->frame;
	if (p_anim->frame > p_max)
		p_anim->frame = p_min;
}

void anim_set(anim_t *p_anim, size_t p_frame) {
	p_anim->frame = p_frame;
}

void scrolling_init(scrolling_t *p_scroll, sprite_t *p_sprite, int p_screen_w, int p_screen_h) {
	p_scroll->sprite   = *p_sprite;
	p_scroll->screen_w =  p_screen_w;
	p_scroll->screen_h =  p_screen_h;
	p_scroll->scroll   =  p_scroll->sprite.h - p_screen_h;
}

void scrolling_scroll(scrolling_t *p_scroll, float p_by) {
	p_scroll->scroll += p_by;

	if (p_scroll->scroll < 0)
		p_scroll->scroll = p_scroll->sprite.h - 1;
	else if (p_scroll->scroll > p_scroll->sprite.h)
		p_scroll->scroll = 0;
}

void scrolling_render(scrolling_t *p_scroll, SDL_Renderer *p_renderer, int p_x, int p_y) {
	SDL_Rect rect = {
		.x = p_x, .y = p_y - p_scroll->scroll,
		.w = p_scroll->sprite.w, .h = p_scroll->sprite.h
	};

	SDL_RenderCopy(p_renderer, p_scroll->sprite.sheet, &p_scroll->sprite.src, &rect);

	if (p_scroll->scroll > p_scroll->sprite.h - p_scroll->screen_h) {
		rect.y = p_scroll->sprite.h - p_scroll->scroll - 1;

		SDL_RenderCopy(p_renderer, p_scroll->sprite.sheet, &p_scroll->sprite.src, &rect);
	}
}
