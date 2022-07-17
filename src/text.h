/* yes, a custom text rendering system because SDL_TTF is bloat :) */

#ifndef TEXT_H__HEADER_GUARD__
#define TEXT_H__HEADER_GUARD__

#include <stdlib.h>  /* exit, EXIT_FAILURE, size_t */
#include <string.h>  /* strlen, strcmp, strcpy */
#include <ctype.h>   /* isalpha, toupper, isdigit */
#include <stdbool.h> /* bool, true, false */
#include <SDL2/SDL.h>

#define TEXTURES_CACHE_LIMIT 128

typedef struct {
	size_t row_len;
	int ch_w, ch_h;

	const char *chs;
	SDL_Surface *sheet;
} font_t;

void font_init(font_t *p_font, const char *p_chs, SDL_Surface *p_sheet,
               size_t p_row_len, int p_ch_w, int p_ch_h);
void font_get_ch_idx(font_t *p_font, char p_ch, int *p_x, int *p_y);

typedef struct {
	char *text;

	SDL_Texture *texture;
	int w, h;

	font_t *font;
} text_texture_t;

typedef struct {
	text_texture_t textures[TEXTURES_CACHE_LIMIT];
	size_t textures_count;

	font_t *font;
} label_t;

void label_init(label_t *p_label, font_t *p_font);
void label_free(label_t *p_label);
text_texture_t *label_get_texture(label_t *p_label, SDL_Renderer *p_renderer, const char *p_text);
void label_render(label_t *p_label, SDL_Renderer *p_renderer, const char *p_text, int p_x, int p_y);

#endif
