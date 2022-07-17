#include "entity.h"

void entity_init(entity_t *p_entity, SDL_Rect *p_rect, int p_health,
                 float p_speed, float p_friction) {
	p_entity->x = p_rect->x;
	p_entity->y = p_rect->y;
	p_entity->w = p_rect->w;
	p_entity->h = p_rect->h;

	p_entity->collision_box = (SDL_Rect){
		.x = p_entity->x, .y = p_entity->y,
		.w = p_entity->w, .h = p_entity->h
	};

	p_entity->angle  = 0;
	p_entity->health = p_health;
	p_entity->state  = 0;

	p_entity->speed    = p_speed;
	p_entity->friction = p_friction;
	p_entity->velocity = 0;
}

void entity_render(entity_t *p_entity, SDL_Renderer *p_renderer) {
	SDL_Rect rect = {
		.x = p_entity->x, .y = p_entity->y,
		.w = p_entity->w, .h = p_entity->h
	};

	SDL_Rect frame = rect;
	frame.x = p_entity->anim.frame * p_entity->w;
	frame.y = p_entity->anim.row;

	SDL_RenderCopyEx(p_renderer, p_entity->anim.sheet, &frame, &rect,
	                 p_entity->angle, NULL, SDL_FLIP_NONE);

#ifdef ENTITY_DEBUG_HITBOX
	SDL_SetRenderDrawColor(p_renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(p_renderer, &p_entity->collision_box);
#endif
}

void entity_rotate(entity_t *p_entity, float p_by) {
	if (p_entity->angle < 360)
		p_entity->angle += p_by;

	while (p_entity->angle >= 360)
		p_entity->angle -= 360;
}

void entity_set_angle(entity_t *p_entity, float p_angle) {
	p_entity->angle = p_angle;
}

void entity_change_velocity(entity_t *p_entity, float p_by) {
	if (p_entity->velocity < p_entity->speed && p_entity->velocity > -p_entity->speed)
		p_entity->velocity += p_by;
	else {
		if (p_entity->velocity > p_entity->speed)
			p_entity->velocity = p_entity->speed;
		else if (p_entity->velocity < -p_entity->speed)
			p_entity->velocity = -p_entity->speed;
	}
}

void entity_move(entity_t *p_entity, float p_angle) {
	if (p_angle == DIR_ENTITY_ANGLE)
		p_angle = p_entity->angle;

	p_entity->x += cos((p_angle - 90) * (M_PI / 180)) * p_entity->velocity;
	p_entity->y += sin((p_angle - 90) * (M_PI / 180)) * p_entity->velocity;

	p_entity->velocity *= p_entity->friction;

	p_entity->collision_box.x = p_entity->x;
	p_entity->collision_box.y = p_entity->y;
}

