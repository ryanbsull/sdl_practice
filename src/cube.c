#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	400

typedef uint32_t 	u32;
typedef uint8_t 	u8;

struct {
	SDL_Window* win;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
	u8 quit;
} state;

typedef struct {
	float x, y, z;
} vec3;

typedef struct {
	float x, y;
} vec2;

typedef struct {
	vec3 vertex[8];
	vec2 edge[12];
	vec3 center;
} cube;

void clear_pixels() {
	for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
		state.pixels[i] = 0x0;
}

void rotate(vec3* p, float x, float y, float z) {
	p->y = (cos(x) * p->y) - (sin(x) * p->z);
	p->z = (cos(x) * p->z) + (sin(x) * p->y);

	p->x = (cos(y) * p->x) + (sin(y) * p->z);
	p->z = (cos(y) * p->z) - (sin(y) * p->x);

	p->x = (cos(z) * p->x) - (sin(z) * p->y);
	p->y = (cos(z) * p->y) + (sin(z) * p->x);
}

void get_center(cube* c, int len) {
	c->center.x = 0; c->center.y = 0; c->center.z = 0;
	for (int i = 0; i < len; i++) {
		c->center.x += c->vertex[i].x;
		c->center.y += c->vertex[i].y;
		c->center.z += c->vertex[i].z;
	}

	c->center.x /= len;
	c->center.y /= len;
	c->center.z /= len;
}

void draw_line(vec3* start, vec3* end) {
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
		"texture_test",
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

	return 0;
}

int loop() {
	clear_pixels();
	SDL_Event e;

	static cube c = {
		{
			{200, 200, 0},
			{300, 200, 0},
			{300, 300, 0},
			{200, 300, 0},

			{200, 200, 100},
			{300, 200, 100},
			{300, 300, 100},
			{200, 300, 100},
		},
		{
			{0,4},
			{1,5},
			{2,6},
			{3,7},

			{0,1},
			{1,2},
			{2,3},
			{3,0},

			{4,5},
			{5,6},
			{6,7},
			{7,4},
		},
		{250,250,50},
	};

	for (int i = 0; i < 8; i++) {
		c.vertex[i].x -= c.center.x;
		c.vertex[i].y -= c.center.y;
		c.vertex[i].z -= c.center.z;
		rotate(&c.vertex[i], 0.001, 0.002, 0.003);
		c.vertex[i].x += c.center.x;
		c.vertex[i].y += c.center.y;
		c.vertex[i].z += c.center.z;
	}

	for (int i = 0; i < 12; i++)
		draw_line(&c.vertex[(int)c.edge[i].x], &c.vertex[(int)c.edge[i].y]);
	
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT)
			return 1;
	}

	SDL_UpdateTexture(state.texture, NULL, state.pixels, SCREEN_WIDTH * sizeof(u32));
	SDL_RenderCopyEx(
		state.renderer,
		state.texture,
		NULL,
		NULL,
		0.0,
		NULL,
		SDL_FLIP_VERTICAL
		);

	SDL_RenderPresent(state.renderer);
	return 0;
}

int destroy() {
	SDL_DestroyTexture(state.texture);
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.win);
	SDL_Quit();
	return 0;
}
