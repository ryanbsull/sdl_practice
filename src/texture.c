#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>

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
	SDL_Event e;
	// make sure it persists across multiple runs of the loop
	static int player_dist = 10;

	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++)
			state.pixels[i*SCREEN_WIDTH + j] = 0x00000000;
	}

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_w)
					if (player_dist - 1 >= 1)
						player_dist--;
				if (e.key.keysym.sym == SDLK_s)
					player_dist++;
				break;
			case SDL_QUIT:
				return 1;
		}
	}


	// ray-caster inspired technique to calculate the square height and width
	int line_height = SCREEN_HEIGHT / player_dist;
	int draw_start = (-line_height / 2) + (SCREEN_HEIGHT / 2);
	draw_start = (draw_start < 0) ? 0 : draw_start;
	int draw_end = (line_height / 2) + (SCREEN_HEIGHT / 2);
	draw_end = (draw_end > SCREEN_HEIGHT) ? SCREEN_HEIGHT-1 : draw_end;

	// assuming the square is equal on all sides then there's no reason to recalculate the width just the position
	int pixel_start = (-line_height / 2) + (SCREEN_WIDTH / 2);
	pixel_start = (pixel_start < 0) ? 0 : pixel_start;
	int pixel_end = (line_height / 2) + (SCREEN_WIDTH / 2);
	pixel_end = (pixel_end >= SCREEN_WIDTH) ? SCREEN_WIDTH-1 : pixel_end;

	for (int i = draw_start; i <= draw_end; i ++) {
		for (int j = pixel_start; j <= pixel_end; j++) {
			state.pixels[i*SCREEN_WIDTH + j] = 0xFFFFFFFF;
		}
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
