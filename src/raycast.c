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
	return 0;
}

int loop() {
	return 1;
}

int destroy() {
	return 0;
}