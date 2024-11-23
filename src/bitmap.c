#include <unistd.h>
#include <stdio.h>
#include <SDL2/SDL.h>

// define global video variables
SDL_Window* win;
SDL_Surface* win_surface;
SDL_Surface* img0;
SDL_Surface* img1;

// define functionality for main()
int init();
int load();
void destroy();

int main() {
  if (init())
    return 1;
  if (load())
    return 1;

  int blit_result;
  // blit = paste image to the scaled area representing the window
  // blit image to entire window
  blit_result = SDL_BlitSurface(img0, NULL, win_surface, NULL);  
  if (blit_result < 0) {
    printf("Image0 blitting failed:\n\tError: %s", SDL_GetError());
    return 1;
  }

  SDL_Rect dest;
	dest.x = 160;
	dest.y = 120;
	dest.w = 320;
	dest.h = 320;
  blit_result = SDL_BlitSurface(img1, NULL, win_surface, &dest);
  if (blit_result < 0) {
    printf("Image1 blitting failed:\n\tError: %s", SDL_GetError());
    return 1;
  }

  SDL_UpdateWindowSurface(win);

  sleep(10);

  destroy();

  return 0;
}

int init() {
  win = SDL_CreateWindow(
    "test_window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);

  if (!win) {
    printf("Failed to create window:\n\tError: %s", SDL_GetError());
    return 1;
  }

  win_surface = SDL_GetWindowSurface(win);
  if (!win_surface) {
    printf("Failed to get window surface:\n\tError: %s", SDL_GetError());
    return 1;
  }
  // check if running on MacOS
#if defined(__APPLE__)
  // a 'feature' of MacOS is that the window will not show unless you pump the event queue
  SDL_PumpEvents();
#endif

  return  0;
}

int load() {
  // load bitmap from test
  img0 = SDL_LoadBMP("bitmaps/test.bmp");
  if (!img0) {
    printf("Failed to load image:\n\tError: %s", SDL_GetError());
    return 1;
  }

  // load bitmap from test2
  img1 = SDL_LoadBMP("bitmaps/test2.bmp");
  if (!img1) {
    printf("Failed to load image:\n\tError: %s", SDL_GetError());
    return 1;
  }

  return 0;
}

void destroy() {
  SDL_DestroyWindow(win);
  SDL_Quit();
}
