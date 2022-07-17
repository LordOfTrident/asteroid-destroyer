#include "particles.h"

void particle_init(particle_t *p_particle, sprite_t *p_sprite, float p_velocity, float p_friction,
                   int p_x, int p_y, size_t p_lifetime) {
	p_particle->sprite = *p_sprite;

	p_particle->x = p_x;
	p_particle->y = p_y;

	p_particle->angle = 0;
	p_particle->visual_angle = 0;

	p_particle->life_timer = p_lifetime;
	p_particle->velocity   = p_velocity;
	p_particle->friction   = p_friction;
}

void particle_update(particle_t *p_particle) {
	if (p_particle->life_timer <= 0)
		return;

	p_particle->x += cos((p_particle->angle - 90) * (M_PI / 180)) * p_particle->velocity;
	p_particle->y += sin((p_particle->angle - 90) * (M_PI / 180)) * p_particle->velocity;

	p_particle->visual_angle += p_particle->velocity;
	while (p_particle->visual_angle >= 360)
		p_particle->visual_angle -= 360;

	p_particle->velocity *= p_particle->friction;

	-- p_particle->life_timer;
}

void particle_render(particle_t *p_particle, SDL_Renderer *p_renderer) {
	SDL_Rect rect = {
		.x = p_particle->x, .y = p_particle->y,
		.w = p_particle->sprite.w, .h = p_particle->sprite.h
	};

	SDL_RenderCopyEx(p_renderer, p_particle->sprite.sheet, &p_particle->sprite.src, &rect,
	                 p_particle->visual_angle, NULL, SDL_FLIP_NONE);
}

void emitter_init(emitter_t *p_emitter) {
	for (size_t i = 0; i < MAX_PARTICLES; ++ i)
		p_emitter->particles[i].life_timer = 0;
}

void emitter_emit(emitter_t *p_emitter, size_t p_amount, sprite_t *p_sprite, bool p_random_size,
                  float p_velocity, float p_friction, int p_x, int p_y, size_t p_life_timer) {
	float    angle = 0, inc = 360 / p_amount;
	sprite_t sprite = *p_sprite;

	for (size_t i = 0; i < MAX_PARTICLES; ++ i) {
		if (p_emitter->particles[i].life_timer == 0) {
			float velocity_offset = (float)(rand() % 10 - 5) / 10;
			float angle_offset    = rand() % 10 - 5;

			if (p_random_size) {
				sprite.w = p_sprite->w;
				sprite.h = p_sprite->h;

				sprite.w *= (float)(rand() % 28 - 10) / 10;
				sprite.h *= (float)(rand() % 28 - 10) / 10;
			}

			particle_init(&p_emitter->particles[i], &sprite,
			              p_velocity + velocity_offset, p_friction, p_x, p_y, p_life_timer);

			p_emitter->particles[i].angle = angle + angle_offset;
			angle += inc;

			-- p_amount;
			if (p_amount <= 0)
				break;
		}
	}

	/* i know i dont watch for the buffer overflow of other arrays, but its like whatever for now */
	if (p_amount > 0) {
		SDL_Log("Out of free particles");

		exit(EXIT_FAILURE);
	}
}

void emitter_update(emitter_t *p_emitter) {
	for (size_t i = 0; i < MAX_PARTICLES; ++ i)
		particle_update(&p_emitter->particles[i]);
}

void emitter_render(emitter_t *p_emitter, SDL_Renderer *p_renderer) {
	for (size_t i = 0; i < MAX_PARTICLES; ++ i) {
		if (p_emitter->particles[i].life_timer != 0)
			particle_render(&p_emitter->particles[i], p_renderer);
	}
}
