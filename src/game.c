#include "game.h"

void game_init(game_t *p_game) {
	memset(p_game, 0, sizeof(game_t));
	/* GAME_STATE_MENU is equal to 0, that means we do not need
	   to explicity assign GAME_STATE_MENU to p_game->state */

	/* init SDL2 */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	} else
		SDL_Log("Initialized video");

	p_game->window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                                  SCREEN_W * 2, SCREEN_H * 2, SDL_WINDOW_RESIZABLE);
	if (p_game->window == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	} else
		SDL_Log("Created the window");

	p_game->renderer = SDL_CreateRenderer(p_game->window, -1, SDL_RENDERER_ACCELERATED);
	if (p_game->renderer == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	} else
		SDL_Log("Created the renderer");

	/* we need a separate texture for the screen in order to achieve a true pixelation effect
	   (rotating the sprites didnt pixelate them) */
	p_game->screen = SDL_CreateTexture(p_game->renderer, SDL_PIXELFORMAT_RGBA8888,
	                                   SDL_TEXTUREACCESS_TARGET, SCREEN_W, SCREEN_H);
	if (p_game->screen == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	} else
		SDL_Log("Created the screen texture");

	p_game->screen_rect.w = SCREEN_W;
	p_game->screen_rect.h = SCREEN_H;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	if (SDL_RenderSetLogicalSize(p_game->renderer, SCREEN_W, SCREEN_H) != 0) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	} else
		SDL_Log("Render logical size was set");

	p_game->key_states = SDL_GetKeyboardState(NULL);

	/* other init */
	p_game->map.x = 1;
	p_game->map.y = 1;
	p_game->map.w = SCREEN_W - 2;
	p_game->map.h = SCREEN_H - 2;

	srand(time(NULL));

	/* init assets */
	game_load_asset(p_game, "./res/sprites.bmp",    "sprites");
	game_load_asset(p_game, "./res/font.bmp",       "font");
	game_load_asset(p_game, "./res/font_small.bmp", "font_small");
	game_load_asset(p_game, "./res/font_big.bmp",   "font_big");

	SDL_Rect player_rect = {
		.x = 0, .y = 0,
		.w = PLAYER_W, .h = PLAYER_H
	};
	entity_init(&p_game->player, &player_rect, 6, 2.5, 0.95);
	anim_init(&p_game->player.anim, game_asset(p_game, "sprites")->texture, 0, 3);

	SDL_Log("Initialized player");

	SDL_Rect back_rect_src = {
		.x = 541, .y = 0,
		.w = SCREEN_W, .h = SCREEN_H * 2 /* this will stretch the texture, but it looks better */
	};
	sprite_t back_sprite;
	sprite_init(&back_sprite, game_asset(p_game, "sprites")->texture, &back_rect_src,
	            back_rect_src.w, back_rect_src.h);

	scrolling_init(&p_game->back_layers[0], &back_sprite, SCREEN_W, SCREEN_H);

	back_sprite.src.x = 293;
	scrolling_init(&p_game->back_layers[1], &back_sprite, SCREEN_W, SCREEN_H);

	back_sprite.src.x = 45;
	scrolling_init(&p_game->back_layers[2], &back_sprite, SCREEN_W, SCREEN_H);

	SDL_Log("Initialized background layers");

	font_init(&p_game->font, "?ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:|!_- <>()",
	          game_asset(p_game, "font")->surface, 7, 6, 10);
	font_init(&p_game->font_small, "?ABCDEFGHIJKLMNOPQRSTUVWXYZ ",
	          game_asset(p_game, "font_small")->surface, 7, 6, 6);
	font_init(&p_game->font_big, "?ABCDEFGHIJKLMNOPQRSTUVWXYZ ",
	          game_asset(p_game, "font_big")->surface, 7, 12, 20);

	SDL_Log("Initialized font");

	label_init(&p_game->label, &p_game->font);

	SDL_Log("Finished initialization");

	/* easter egg ;)
	   people have been saying that the asteroids look like cookies */
	p_game->title = rand() % 20 == 1? "Cookie Destroyer" : GAME_TITLE;
}

void game_run(game_t *p_game) {
	p_game->running = true;

	size_t fps_timer = 0;

	do {
		/* calculate fps */
		size_t now   = SDL_GetTicks();
		size_t delta = now - fps_timer;

		p_game->fps = 1000 / delta;
		fps_timer   = now;

		char title[32] = {0};
		snprintf(title, sizeof(title), GAME_TITLE" FPS: %zu", p_game->fps);

		SDL_SetWindowTitle(p_game->window, title);

		game_update(p_game);
		game_render(p_game);

		++ p_game->tick;

		SDL_Delay(1000 / FPS_CAP);
	} while (p_game->running);
}

void game_finish(game_t *p_game) {
	/* free resources */
	for (size_t i = 0; i < p_game->assets_count; ++ i) {
		SDL_DestroyTexture(p_game->assets[i].texture);
		SDL_FreeSurface(p_game->assets[i].surface);
	}

	label_free(&p_game->label);

	SDL_DestroyTexture(p_game->screen);
	SDL_Log("Destroyed the screen");

	SDL_Log("Destroyed textures");

	SDL_DestroyRenderer(p_game->renderer);
	SDL_Log("Destroyed the renderer");

	SDL_DestroyWindow(p_game->window);
	SDL_Log("Destroyed the window");

	SDL_Quit();
}

void game_load_asset(game_t *p_game, const char *p_path, const char *p_key) {
	if (p_game->assets_count >= MAX_ASSETS) {
		SDL_Log("Reached max assets limit of %i", MAX_ASSETS);

		exit(EXIT_FAILURE);
	}

	SDL_Surface *surface = SDL_LoadBMP(p_path);
	if (surface == NULL) {
		SDL_Log("%s", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to load asset", SDL_GetError(), 0);

		exit(EXIT_FAILURE);
	} else
		SDL_Log("Loaded asset '%s'", p_path);

	Uint32 color_key = SDL_MapRGB(surface->format, 123, 0, 123);
	SDL_SetColorKey(surface, true, color_key);

	p_game->assets[p_game->assets_count].key     = p_key;
	p_game->assets[p_game->assets_count].surface = surface;
	p_game->assets[p_game->assets_count].texture = SDL_CreateTextureFromSurface(p_game->renderer,
	                                                                            surface);
	++ p_game->assets_count;
}

asset_t *game_asset(game_t *p_game, const char *p_key) {
	for (size_t i = 0; i < p_game->assets_count; ++ i) {
		if (strcmp(p_game->assets[i].key, p_key) == 0)
			return &p_game->assets[i];
	}

	SDL_Log("Asset '%s' was not found", p_key);

	exit(EXIT_FAILURE);
}

void game_get_asteroid_sprite_info(int p_sprite, int *p_row, int *p_w, int *p_h) {
	switch (p_sprite) {
	case 0:
		*p_row = 38;
		*p_w   = 38;
		*p_h   = 31;

		break;

	case 1:
		*p_row = 69;
		*p_w   = 35;
		*p_h   = 26;

		break;

	case 2:
		*p_row = 95;
		*p_w   = 37;
		*p_h   = 28;

		break;

	default:
		*p_row = 0;
		*p_w   = 0;
		*p_h   = 0;

		break;
	}
}

int game_get_asteroid_type(entity_t *p_asteroid) {
	switch (p_asteroid->anim.row) {
	case 38: return  0;
	case 69: return  1;
	case 95: return  2;
	default: return -1;
	}
}

void game_delete_asteroid(game_t *p_game, size_t p_idx) {
	for (size_t i = p_idx; i < p_game->asteroids_count - 1; ++ i)
		p_game->asteroids[i] = p_game->asteroids[i + 1];

	-- p_game->asteroids_count;
}

void game_delete_bullet(game_t *p_game, size_t p_idx) {
	for (size_t i = p_idx; i < p_game->bullets_count - 1; ++ i)
		p_game->bullets[i] = p_game->bullets[i + 1];

	-- p_game->bullets_count;
}

void game_reset_values(game_t *p_game) {
	p_game->bullets_count   = 0;
	p_game->asteroids_count = 0;
	p_game->score  = 0;
	p_game->tick   = 0;
	p_game->paused = false;
	p_game->player.health = 6;

	for (size_t i = 0; i < sizeof(p_game->back_layers) / sizeof(scrolling_t); ++ i)
		p_game->back_layers[i].scroll = 0;

	game_reset_player_values(p_game);
}

void game_reset_player_values(game_t *p_game) {
	p_game->player.velocity = 0;
	entity_set_angle(&p_game->player, 0);
}

void game_emit_from(game_t *p_game, int p_x, int p_y, sprite_t *p_sprite, size_t p_amount,
                    bool p_random_size, float p_velocity, float p_friction, size_t p_lifetime) {

	emitter_emit(&p_game->particle_emitter, p_amount, p_sprite, p_random_size, p_velocity,
	             p_friction, p_x, p_y, p_lifetime);
}

void game_break_player(game_t *p_game) {
	SDL_Rect src_rect = {
		.x = 0, .y = 123,
		.w = 9, .h = 7
	};

	sprite_t particle_sprite;
	sprite_init(&particle_sprite, game_asset(p_game, "sprites")->texture, &src_rect,
	            src_rect.w, src_rect.h);

	game_emit_from(p_game, p_game->player.x + p_game->player.w / 2 - particle_sprite.w / 2,
	               p_game->player.y + p_game->player.h / 2 - particle_sprite.h / 2,
	               &particle_sprite, 20, true, 2, 0.995, 3 * 60);
}

void game_break_asteroid(game_t *p_game, entity_t *p_asteroid) {
	int type = game_get_asteroid_type(p_asteroid);

	SDL_Rect src_rect = {
		.x = 0, .y = 147 + 11 * type,
		.w = 10, .h = 11
	};

	sprite_t particle_sprite;
	sprite_init(&particle_sprite, game_asset(p_game, "sprites")->texture, &src_rect,
	            src_rect.w, src_rect.h);

	game_emit_from(p_game, p_asteroid->x + p_asteroid->w / 2 - particle_sprite.w / 2,
	               p_asteroid->y + p_asteroid->h / 2 - particle_sprite.h / 2, &particle_sprite,
	               20, true, 2, 0.995, 3 * 60);
}

void game_explosion_at(game_t *p_game, int p_x, int p_y) {
	SDL_Rect src_rect = {
		.x = 0, .y = 130,
		.w = 19, .h = 17
	};

	sprite_t explosion_sprite;
	sprite_init(&explosion_sprite, game_asset(p_game, "sprites")->texture, &src_rect,
	            src_rect.w, src_rect.h);

	game_emit_from(p_game, p_x - src_rect.w / 2, p_y - src_rect.h / 2, &explosion_sprite,
	               5, false, 2, 0.95, 0.12 * 60);
}

void game_explosion_at_player(game_t *p_game) {
	game_explosion_at(p_game, p_game->player.x + p_game->player.w / 2,
	                  p_game->player.y + p_game->player.h / 2);
}

void game_explosion_at_asteroid(game_t *p_game, entity_t *p_asteroid) {
	game_explosion_at(p_game, p_asteroid->x + p_asteroid->w / 2, p_asteroid->y + p_asteroid->h / 2);
}

void game_update(game_t *p_game) {
	game_handle_events(p_game);

	if (p_game->paused)
		return;

	switch (p_game->state) {
	case GAME_STATE_MENU:
		if (p_game->key_states[SDL_SCANCODE_RETURN]) {
			p_game->player.x = SCREEN_W / 2 - PLAYER_W / 2;
			p_game->player.y = SCREEN_H - PLAYER_H * 1.5;

			game_reset_values(p_game);

			p_game->state = GAME_STATE_INGAME;
		}

		game_update_back(p_game);
		game_update_menu(p_game);

		break;

	case GAME_STATE_INGAME:
		if (p_game->key_states[SDL_SCANCODE_RIGHT]) {
			entity_change_velocity(&p_game->player, 0.2);

			if (p_game->player.angle < 15)
				entity_rotate(&p_game->player, 2);
		} else if (p_game->key_states[SDL_SCANCODE_LEFT]) {
			entity_change_velocity(&p_game->player, -0.2);

			if (p_game->player.angle > -15)
				entity_rotate(&p_game->player, -2);
		} else if (p_game->player.angle != 0) {
			if (p_game->player.angle < 0)
				entity_rotate(&p_game->player, 2);
			else if (p_game->player.angle > 0)
				entity_rotate(&p_game->player, -2);
		}

		if (p_game->player_shoot_timer == 0 && p_game->key_states[SDL_SCANCODE_LCTRL]) {
			SDL_Rect bullet_rect = {
				.x = p_game->player_shoot_from_left?
				     p_game->player.x + 2 : p_game->player.x + 10,
				.y = p_game->player.y + 1,
				.w = 3, .h = 14
			};

			entity_t *bullet = &p_game->bullets[p_game->bullets_count];

			entity_init(bullet, &bullet_rect, 0, 7, 1);
			anim_init(&bullet->anim, game_asset(p_game, "sprites")->texture, 24, 4);

			entity_set_angle(bullet, p_game->player.angle);
			bullet->velocity = bullet->speed;

			++ p_game->bullets_count;

			p_game->player_shoot_timer = 15;
			p_game->player_shoot_from_left = !p_game->player_shoot_from_left;
		}

		/* fall through */

	case GAME_STATE_DIED:
	case GAME_STATE_FAILED:
		game_update_back(p_game);
		game_update_entities(p_game);

		break;

	default: break;
	}
}

void game_handle_events(game_t *p_game) {
	while (SDL_PollEvent(&p_game->event)) {
		switch (p_game->event.type) {
		case SDL_QUIT: p_game->running = false; break;
		default: break;
		}

		switch (p_game->state) {
		case GAME_STATE_MENU: game_handle_events_menu(p_game); break;
		case GAME_STATE_DIED:
		case GAME_STATE_FAILED:
		case GAME_STATE_INGAME: game_handle_events_ingame(p_game); break;
		default: break;
		}
	}
}

void game_handle_events_menu(game_t *p_game) {
	switch (p_game->event.type) {
	case SDL_KEYDOWN:
		switch (p_game->event.key.keysym.sym) {
		case SDLK_q: p_game->running = false; break;
		default: break;
		}

		break;

	default: break;
	}
}

void game_handle_events_ingame(game_t *p_game) {
	switch (p_game->event.type) {
	case SDL_KEYDOWN:
		switch (p_game->event.key.keysym.sym) {
		case SDLK_SPACE:
			if (p_game->state == GAME_STATE_INGAME)
				p_game->paused = !p_game->paused;

			break;

		case SDLK_q:
			p_game->state = GAME_STATE_MENU;

			game_reset_values(p_game);

			break;

		default: break;
		}

		break;

	default: break;
	}
}

void game_update_menu(game_t *p_game) {
	if (p_game->tick % 30 == 0)
		p_game->blink = !p_game->blink;
}

void game_update_back(game_t *p_game) {
	if (p_game->paused)
		return;

	scrolling_scroll(&p_game->back_layers[0], -1.5);
	scrolling_scroll(&p_game->back_layers[1], -2);
	scrolling_scroll(&p_game->back_layers[2], -2.5);
}

void game_update_entities(game_t *p_game) {
	if (p_game->paused)
		return;

	if (p_game->state == GAME_STATE_INGAME && p_game->tick % 50 == 0) {
		entity_t *asteroid = &p_game->asteroids[p_game->asteroids_count];

		int row, w, h;
		game_get_asteroid_sprite_info(rand() % 3, &row, &w, &h);

		assert(row != 0 && w != 0 && h != 0);

		SDL_Rect asteroid_rect = {
			.x = rand() % (p_game->map.w - 42) + 2, .y = -h,
			.w = w, .h = h
		};

		entity_init(asteroid, &asteroid_rect, 2, 1, 1);
		anim_init(&asteroid->anim, game_asset(p_game, "sprites")->texture, row, 1);

		asteroid->state    = rand() % 4;
		asteroid->velocity = asteroid->speed;

		++ p_game->asteroids_count;
	}

	/* update the player */
	entity_move(&p_game->player, DIR_RIGHT);

	if (p_game->player_shoot_timer > 0)
		-- p_game->player_shoot_timer;

	if (p_game->tick % 8 == 0)
		anim_next(&p_game->player.anim, 0, 2);

	/* update the asteroids */
	for (size_t i = 0; i < p_game->asteroids_count; ++ i) {
		entity_move(&p_game->asteroids[i], DIR_DOWN);

		float rotate_by = -0.7;
		switch (p_game->asteroids[i].state) {
		case 1: rotate_by =  0.4; break;
		case 2: rotate_by = -0.4; break;
		case 3: rotate_by =  0.7; break;
		default: break;
		}

		entity_rotate(&p_game->asteroids[i], rotate_by);

		if (p_game->state != GAME_STATE_DIED) {
			if (p_game->asteroids[i].y >= p_game->map.h) {
				game_delete_asteroid(p_game, i);
				-- i;

#ifndef GAME_PLAYER_INVINCIBLE
				if (p_game->player.health > 0) {
					p_game->player.health -= 2;

					if (p_game->player.health <= 0) {
						p_game->state = GAME_STATE_FAILED;
						game_reset_player_values(p_game);

						if (p_game->paused)
							p_game->paused = false;
					}
				}
#endif
			} else if (SDL_HasIntersection(&p_game->player.collision_box,
			                               &p_game->asteroids[i].collision_box)) {
				game_break_asteroid(p_game, &p_game->asteroids[i]);
				game_explosion_at_asteroid(p_game, &p_game->asteroids[i]);

				game_delete_asteroid(p_game, i);
				-- i;

				if (p_game->state == GAME_STATE_INGAME)
					++ p_game->score;

#ifndef GAME_PLAYER_INVINCIBLE
				if (p_game->player.health > 0) {
					-- p_game->player.health;

					if (p_game->player.health <= 0) {
						p_game->state = GAME_STATE_DIED;
						game_reset_player_values(p_game);

						game_break_player(p_game);
						game_explosion_at_player(p_game);

						if (p_game->paused)
							p_game->paused = false;
					}
				}
#endif
			}
		}
	}

	/* update the bullets */
	for (size_t i = 0; i < p_game->bullets_count; ++ i) {
		entity_move(&p_game->bullets[i], DIR_ENTITY_ANGLE);

		if (p_game->bullets[i].y + p_game->bullets[i].h <= 0) {
			game_delete_bullet(p_game, i);
			-- i;
		} else {
			for (size_t j = 0; j < p_game->asteroids_count; ++ j) {
				if (SDL_HasIntersection(&p_game->bullets[i].collision_box,
				                        &p_game->asteroids[j].collision_box)) {
					game_delete_bullet(p_game, i);
					-- i;

					-- p_game->asteroids[j].health;
					if (p_game->asteroids[j].health <= 0) {
						game_break_asteroid(p_game, &p_game->asteroids[j]);
						game_explosion_at_asteroid(p_game, &p_game->asteroids[j]);

						game_delete_asteroid(p_game, j);

						++ p_game->score;

						/* j does not need to be decremented, we are breaking out
						    of the loop anyways */
					}

					break;
				}
			}
		}
	}

	emitter_update(&p_game->particle_emitter);

	game_update_collision(p_game);
}

void game_update_collision(game_t *p_game) {
	if (p_game->player.x + p_game->player.w > p_game->map.w) {
		p_game->player.x = p_game->map.w - p_game->player.w;

		game_reset_player_values(p_game);
	} else if (p_game->player.x < 0) {
		p_game->player.x = 0;

		game_reset_player_values(p_game);
	}
}

void game_render(game_t *p_game) {
	SDL_SetRenderDrawColor(p_game->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(p_game->renderer);

	SDL_SetRenderTarget(p_game->renderer, p_game->screen);
	SDL_RenderSetViewport(p_game->renderer, NULL);

	SDL_SetRenderDrawColor(p_game->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	SDL_Rect border_rect = {
		.x = 0, .y = 0,
		.w = SCREEN_W, .h = SCREEN_H
	};
	SDL_RenderDrawRect(p_game->renderer, &border_rect);

	SDL_RenderSetViewport(p_game->renderer, &p_game->map);

	switch (p_game->state) {
	case GAME_STATE_MENU:
		game_render_back(p_game);
		game_render_menu(p_game);

		break;

	case GAME_STATE_DIED:
	case GAME_STATE_FAILED:
	case GAME_STATE_INGAME:
		game_render_back(p_game);
		game_render_entities(p_game);
		game_render_hud(p_game);

		break;

	default: break;
	}

	SDL_SetRenderTarget(p_game->renderer, NULL);
	SDL_RenderCopy(p_game->renderer, p_game->screen, NULL, &p_game->screen_rect);
	SDL_RenderPresent(p_game->renderer);
}

void game_render_menu(game_t *p_game) {
	p_game->label.font = &p_game->font_big;

	text_texture_t *texture = label_get_texture(&p_game->label, p_game->renderer, p_game->title);
	SDL_Rect rect = {
		.x = p_game->map.w / 2 - strlen(p_game->title) * p_game->font_big.ch_w / 2, .y = 40,
		.w = texture->w, .h = texture->h
	};

	SDL_RenderCopyEx(p_game->renderer, texture->texture, NULL, &rect,
	                 sin((float)p_game->tick / 20) * 2, NULL, SDL_FLIP_NONE);

	p_game->label.font = &p_game->font_small;

	int pos = p_game->map.w / 2 - strlen("space     fire") * 6 / 2;

	label_render(&p_game->label, p_game->renderer, "ctrl      fire", pos, SCREEN_H / 2 + 35);
	label_render(&p_game->label, p_game->renderer, "arrows    move", pos, SCREEN_H / 2 + 45);
	label_render(&p_game->label, p_game->renderer, "space    pause", pos, SCREEN_H / 2 + 55);
	label_render(&p_game->label, p_game->renderer, "q         quit", pos, SCREEN_H / 2 + 65);

	p_game->label.font = &p_game->font;

	if (p_game->blink) {
		label_render(&p_game->label, p_game->renderer, "Press enter to play!",
		             p_game->map.w / 2 - strlen("Press enter to play!") * p_game->font.ch_w / 2,
		             SCREEN_H / 2);
	}
}

void game_render_hud(game_t *p_game) {
	char text[32] = {0};
	snprintf(text, 32, "SCORE: %i", p_game->score);
	label_render(&p_game->label, p_game->renderer, text, 2, 2);

	strcpy(text, "HEALTH: ()()()");
	if (p_game->player.health > 0) {
		for (size_t i = 0; i < (size_t)p_game->player.health; ++ i)
			text[i + 8] = i % 2 == 0? '<' : '>';
	}

	label_render(&p_game->label, p_game->renderer, text,
	             p_game->map.w - p_game->font.ch_w * 14 - 3, 2);

	const char *message = NULL;

	switch (p_game->state) {
	case GAME_STATE_DIED:   message = "You died";        break;
	case GAME_STATE_FAILED: message = "You have failed"; break;
	default:
		if (p_game->paused)
			message = "Paused";

		break;
	}

	if (message != NULL) {
		p_game->label.font = &p_game->font_small;

		label_render(&p_game->label, p_game->renderer, message,
		             p_game->map.w / 2 - strlen(message) * p_game->font_small.ch_w / 2,
		             p_game->map.h / 2);

		p_game->label.font = &p_game->font;
	}
}

void game_render_back(game_t *p_game) {
	for (size_t i = 0; i < sizeof(p_game->back_layers) / sizeof(scrolling_t); ++ i)
		scrolling_render(&p_game->back_layers[i], p_game->renderer, -1, -1);
}

void game_render_entities(game_t *p_game) {
	/* draw the bullets */
	for (size_t i = 0; i < p_game->bullets_count; ++ i)
		entity_render(&p_game->bullets[i], p_game->renderer);

	/* draw the player */
	if (p_game->state != GAME_STATE_DIED)
		entity_render(&p_game->player, p_game->renderer);

	/* draw the asteroids */
	for (size_t i = 0; i < p_game->asteroids_count; ++ i)
		entity_render(&p_game->asteroids[i], p_game->renderer);

	emitter_render(&p_game->particle_emitter, p_game->renderer);
}
