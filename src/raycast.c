#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	400

#define ANGULAR_SPEED	1
#define NUM_RAYS	256

enum direction {
	LEFT = 1,
	RIGHT = -1,
	FWD = 1,
	BACK = -1,
};

typedef uint32_t 	u32;
typedef uint8_t 	u8;

typedef struct {
	float x,y;
} vec2;

typedef struct {
	vec2 position;
	vec2 direction;
} player;

struct {
	SDL_Window* win;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
	player player;
	u8 quit;
} state;

int map[] = {
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,
		1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

void clear_pixels() {
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
		state.pixels[i] = 0x0;
}

void draw_line(vec2* start, vec2* end) {
	float dx = end->x - start->x;
	float dy = end->y - start->y;
	float len = sqrt(dx*dx + dy*dy);
	float angle = atan2(dy, dx);
	int x, y;

	for (float i = 0; i < len; i++) {
		x = (int)(start->x + i * cos(angle))  + 1;
		y = (int)(start->y + i * sin(angle)) + 1;
		state.pixels[x * SCREEN_WIDTH + y] = 0xFFFFFFFF;
	}
}

int init();
int loop();
int destroy();

int main() {
	if (init())
		return 1;
	while (!loop())
		SDL_Delay(5);
	destroy();
	return 0;
}

int init() {
	state.win = SDL_CreateWindow(
		"raycast_test",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
		);
	if (!state.win) {
		printf("Failed to create window:\n\tError: %s", SDL_GetError());
		return 1;
	}

	state.renderer = SDL_CreateRenderer(state.win, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!state.renderer) {
		printf("Failed to create renderer:\n\tError: %s", SDL_GetError());
		return 1;
	}

	state.texture = SDL_CreateTexture(
		state.renderer, 
		SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT
		);
	if (!state.texture) {
		printf("Failed to create texture:\n\tError: %s", SDL_GetError());
		return 1;
	}

	state.player.position.x = 10;
	state.player.position.y = 10;
	state.player.direction.x = 1;
	state.player.direction.y = 0;
	return 0;
}

void move(player* p, int dir) {
	float angle = atan2(p->direction.y, p->direction.x);
	p->position.x += cos(angle) * dir;
	p->position.y += sin(angle) * dir;
}

void rotate(player* p, int dir) {
	float angle = atan2(p->direction.y, p->direction.x);
	angle += ANGULAR_SPEED;
	p->direction.x = cos(angle) * dir;
	p->direction.y = sin(angle) * dir;
}

void render_screen() {
	SDL_UpdateTexture(
		state.texture,
		NULL,
		state.pixels, SCREEN_WIDTH * sizeof(u32)
	);
	SDL_RenderCopyEx(
		state.renderer,
		state.texture,
		NULL, NULL,
		0.0, NULL,
		SDL_FLIP_VERTICAL
	);
	
}

void raycast(player* p, int ray_pos) {
	
}

int loop() {
	clear_pixels();
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT:
				return 1;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
					case SDLK_w:
						move(&state.player, FWD);
						break;
					case SDLK_s:
						move(&state.player, BACK);
						break;
					case SDLK_a:
						rotate(&state.player, LEFT);
						break;
					case SDLK_d:
						rotate(&state.player, RIGHT);
						break;
				}
				break;
		}
	}

	printf("PLAYER_STATE:\n\tPosition: [%f,%f]\n\tDirection: [%f,%f]\n",
	       		state.player.position.x, state.player.position.y, 
	       		state.player.direction.x, state.player.direction.y);

	for (int i = 0; i < NUM_RAYS; i++) {
		raycast(&state.player, i);
	}

	render_screen();
	return 0;
}

int destroy() {
	SDL_DestroyTexture(state.texture);
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.win);
	SDL_Quit();
	return 0;
}
