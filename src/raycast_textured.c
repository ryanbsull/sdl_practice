#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define SCREEN_WIDTH		640
#define SCREEN_HEIGHT		400
#define TEXTURE_WIDTH		128
#define TEXTURE_HEIGHT	128
#define NUM_TEXTURES		1

#define WHITE						0xFFFFFFFF
#define GREY						0x88888888

enum dir {
	LEFT = -1,
	RIGHT = 1,
	FWD = 1,
	BACK = -1,
};

enum side {
	x_side,
	y_side
};

typedef uint32_t 	u32;
typedef uint8_t 	u8;

typedef struct {
	float x,y;
} vec2;

typedef struct {
	int x,y;
} int_vec2;

typedef struct {
	vec2 pos;
	vec2 dir;
	vec2 cam;
} player;

struct {
	SDL_Window* win;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
	u32 textures[TEXTURE_WIDTH * TEXTURE_HEIGHT * NUM_TEXTURES];
	player player;
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

void validate_location(vec2* loc) {
	loc->x = (loc->x < 0) ? 0 : loc->x;
	loc->y = (loc->y < 0) ? 0 : loc->y;
	loc->x = (loc->x > SCREEN_WIDTH-1) ? SCREEN_WIDTH - 1 : loc->x;
	loc->y = (loc->y > SCREEN_HEIGHT-1) ? SCREEN_HEIGHT - 1 : loc->y;
}

void draw_line(vec2* start, vec2* end, u32 c) {
	validate_location(start);
	validate_location(end);
	float dx = end->x - start->x;
	float dy = end->y - start->y;
	float len = sqrt(dx*dx + dy*dy);
	float angle = atan2(dy, dx);
	int x, y;

	for (float i = 0; i < len; i++) {
		x = (int)(start->x + i * cos(angle))  + 1;
		y = (int)(start->y + i * sin(angle)) + 1;
		state.pixels[y * SCREEN_WIDTH + x] = c;
	}
}

void draw_textured_line(vec2* start, vec2* end, int tex_idx, int tex_slice, int side) {
	validate_location(start);
	validate_location(end);
	float dx = end->x - start->x;
	float dy = end->y - start->y;
	float len = sqrt(dx*dx + dy*dy);
	float angle = atan2(dy, dx);
	int x, y, tex_y;
	u32 color;
	float step = TEXTURE_HEIGHT / dy;
	float tex_pos = (start->y - ((float)SCREEN_HEIGHT / 2) + (dy / 2)) * step;

	for (float i = 0; i < len; i++) {
		x = (int)(start->x + i * cos(angle))  + 1;
		y = (int)(start->y + i * sin(angle)) + 1;
		tex_y = (int)tex_pos & (TEXTURE_HEIGHT - 1); // masking with texture height only works since log2(TEXTURE_HEIGHT) is an integer so 1 less has a binary value with 1's in all positions ***
		tex_pos += step;
		color = state.textures[tex_idx * NUM_TEXTURES + TEXTURE_HEIGHT * tex_y + tex_slice];
		color = (side == x_side) ? (color >> 1) & 8355711 : color;
		state.pixels[y * SCREEN_WIDTH + x] = color;
	}
}

void init_textures() {
	for (int x = 0; x < TEXTURE_WIDTH; x++) {
		for (int y = 0; y < TEXTURE_HEIGHT; y++) {
			state.textures[x * TEXTURE_HEIGHT + y] = 0xFF1101AF * (x % 16 && y % 16); //red bricks
		}
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
		"raycast_textured_test",
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

	state.player.pos.x = 10;
	state.player.pos.y = 10;
	state.player.dir.x = 1;
	state.player.dir.y = 0;
	// define camera plane where the view will be projected onto
	state.player.cam.x = 0;
	state.player.cam.y = 0.66;

	init_textures();

	return 0;
}

void move(player* p, int dir, float speed) {
	float angle = atan2(p->dir.y, p->dir.x);

	if (map[(int)(p->pos.x + cos(angle) * dir) * 16 + (int)(p->pos.y + sin(angle) * dir)] == 1)
		return;
	
	if (p->pos.x + cos(angle) * dir < 15 && p->pos.x + cos(angle) * dir >= 0.66) {
		p->pos.x += cos(angle) * dir * speed;
	}
	if (p->pos.y + sin(angle) * dir < 15 && p->pos.y + sin(angle) * dir >= 0.66) {
		p->pos.y += sin(angle) * dir * speed;
	}
}

void rotate(player* p, int dir, float speed) {
	float angle = atan2(p->dir.y, p->dir.x);
	angle += speed * dir;
	p->dir.x = cos(angle);
	p->dir.y = sin(angle);
	// ensure the camera plane is always perpendicular to our direction vector
	p->cam.x = -p->dir.y;
	p->cam.y = p->dir.x;
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

	SDL_RenderPresent(state.renderer);
}

void raycast(player* p, int slice) {
	float cam_x = 2*(float)slice / SCREEN_WIDTH - 1;
	vec2 slice_start, slice_end, // vectors that will be used for drawing our vertical slices across the screen
	side_dist, // tracks the length of the ray from the current positiojn to the next x or y side
	ray_dir = {p->dir.x + p->cam.x * cam_x, p->dir.y + p->cam.y * cam_x}, // calculate ray direction from the camera plane
	delta_dist = {(ray_dir.x == 0) ? 1e30 : fabs(1 / ray_dir.x), (ray_dir.y == 0) ? 1e30 : fabs(1 / ray_dir.y)};
	int_vec2 map_pos = {(int)p->pos.x, (int)p->pos.y}; // track integer map position
	float dist; // track distance until a wall is encountered for this ray
	int hit = map[map_pos.x * 16 + map_pos.y]; // track if the ray has hit a wall
	enum side s;
	int step_x, step_y;

	if (ray_dir.x < 0) {
		step_x = -1;
		side_dist.x = (p->pos.x - (float) map_pos.x) * delta_dist.x;
	} else {
		step_x = 1;
		side_dist.x = ((float) map_pos.x + 1 - p->pos.x) * delta_dist.x;
	}
	if (ray_dir.y < 0) {
		step_y = -1;
		side_dist.y = (p->pos.y - (float) map_pos.y) * delta_dist.y;
	} else {
		step_y = 1;
		side_dist.y = ((float) map_pos.y + 1 - p->pos.y) * delta_dist.y;
	}

	while (hit == 0) {
		if (side_dist.x < side_dist.y) {
			side_dist.x += delta_dist.x;
			map_pos.x += step_x;
			s = x_side;
		} else {
			side_dist.y += delta_dist.y;
			map_pos.y += step_y;
			s = y_side;
		}
		hit = map[map_pos.x * 16 + map_pos.y];
	}
	switch(s) {
		case x_side:
			dist = (side_dist.x - delta_dist.x);
			break;
		case y_side:
			dist = (side_dist.y - delta_dist.y);
			break;
	}
	int tex_idx = hit - 1;
	double wall_hit_x = 
			(s == x_side) ? 
			      p->pos.y + dist * p->dir.y : 
			      p->pos.x + dist * p->dir.x;
	wall_hit_x -= trunc(wall_hit_x);
	int tex_slice = (int)(wall_hit_x * (double)TEXTURE_WIDTH);
	tex_slice = TEXTURE_WIDTH - tex_slice - 1;

	int line_height = (int)(SCREEN_HEIGHT / dist);
	int draw_start = (SCREEN_HEIGHT - line_height) / 2;
	int draw_end = (SCREEN_HEIGHT + line_height) / 2;

	slice_start.x = slice;
	slice_end.x = slice;

	slice_start.y = draw_start;
	slice_end.y = draw_end;
	//draw_line(&slice_start, &slice_end, WHITE);
	draw_textured_line(&slice_start, &slice_end, tex_idx, tex_slice, s);
}

int loop() {
	static float time = 0, prev_time = 0;
	prev_time = time;
	time = SDL_GetTicks();
	float dt = (time - prev_time) / 1000.0;
	float move_speed = 10 * dt, rot_speed = 10 * dt;
	
	clear_pixels();
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT:
				return 1;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
					case SDLK_w:
						move(&state.player, FWD, move_speed);
						break;
					case SDLK_s:
						move(&state.player, BACK, move_speed);
						break;
					case SDLK_a:
						rotate(&state.player, LEFT, rot_speed);
						break;
					case SDLK_d:
						rotate(&state.player, RIGHT, rot_speed);
						break;
					case SDLK_p:
						printf("Player Info:\n\tPosition: [%f,%f]\n\tDirection: [%f,%f]\n", state.player.pos.x, state.player.pos.y, state.player.dir.x, state.player.dir.y);
						break;
				}
				break;
		}
	}

	for (int i = 0; i < SCREEN_WIDTH; i++)
		raycast(&state.player, i);

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
