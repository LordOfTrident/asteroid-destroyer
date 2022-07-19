#include "text.h"

void font_init(font_t *p_font, const char *p_chs, SDL_Surface *p_sheet,
               size_t p_row_len, int p_ch_w, int p_ch_h) {
	p_font->chs     = p_chs;
	p_font->sheet   = p_sheet;
	p_font->row_len = p_row_len;
	p_font->ch_w    = p_ch_w;
	p_font->ch_h    = p_ch_h;
}

void font_get_ch_pos(font_t *p_font, char p_ch, int *p_x, int *p_y) {
	if (isalpha(p_ch))
		p_ch = toupper(p_ch);

	bool found = false;
	size_t i = 0;
	for (; p_font->chs[i] != '\0'; ++ i) {
		if (p_ch == p_font->chs[i]) {
			found = true;

			break;
		}
	}
	if (!found)
		i = 0;

	*p_x = i % p_font->row_len * p_font->ch_w;
	*p_y = i / p_font->row_len * p_font->ch_h;
}

void label_init(label_t *p_label, font_t *p_font) {
	p_label->font = p_font;
	p_label->textures_count = 0;
}

void label_free(label_t *p_label) {
	for (size_t i = 0; i < p_label->textures_count; ++ i) {
		SDL_DestroyTexture(p_label->textures[i].texture);
		free(p_label->textures[i].text);
	}
}

text_texture_t *label_get_texture(label_t *p_label, SDL_Renderer *p_renderer, const char *p_text) {
	bool found = false;
	size_t i = 0;
	for (; i < p_label->textures_count; ++ i) {
		bool same_font = p_label->textures[i].font == p_label->font;
		if (strcmp(p_label->textures[i].text, p_text) == 0 && same_font) {
			found = true;

			break;
		}
	}

	if (!found) {
		i = p_label->textures_count;
		++ p_label->textures_count;

		if (p_label->textures_count >= TEXTURES_CACHE_LIMIT) {
			label_free(p_label);

			i = 0;
			p_label->textures_count = 1;
		}

		size_t text_len = strlen(p_text);

		p_label->textures[i].text = malloc(text_len + 1);
		strcpy(p_label->textures[i].text, p_text);

		SDL_Surface *surface = SDL_CreateRGBSurface(0, text_len * p_label->font->ch_w,
		                                            p_label->font->ch_h, 32, 0, 0, 0, 0);
		if (surface == NULL) {
			SDL_Log("%s", SDL_GetError());

			exit(EXIT_FAILURE);
		}

		Uint32 color_key = SDL_MapRGB(surface->format, 0, 0, 0);
		SDL_SetColorKey(surface, true, color_key);

		SDL_Rect rect_src;
		rect_src.w = p_label->font->ch_w;
		rect_src.h = p_label->font->ch_h;

		p_label->textures[i].w = text_len * rect_src.w;
		p_label->textures[i].h = rect_src.h;

		SDL_Rect rect_dest = rect_src;
		rect_dest.y = 0;

		for (size_t j = 0; j < text_len; ++ j) {
			rect_dest.x = j * rect_dest.w;

			font_get_ch_pos(p_label->font, p_label->textures[i].text[j], &rect_src.x, &rect_src.y);

			SDL_BlitSurface(p_label->font->sheet, &rect_src, surface, &rect_dest);
		}

		p_label->textures[i].texture = SDL_CreateTextureFromSurface(p_renderer, surface);
		if (p_label->textures[i].texture == NULL) {
			SDL_Log("%s", SDL_GetError());

			exit(EXIT_FAILURE);
		}

		p_label->textures[i].font = p_label->font;

		SDL_FreeSurface(surface);
	}

	return &p_label->textures[i];
}

void label_render(label_t *p_label, SDL_Renderer *p_renderer,
                  const char *p_text, int p_x, int p_y) {
	text_texture_t *texture = label_get_texture(p_label, p_renderer, p_text);

	SDL_Rect rect = {
		.x = p_x, .y = p_y,
		.w = texture->w, .h = texture->h
	};

	SDL_RenderCopy(p_renderer, texture->texture, NULL, &rect);
}
