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
int loop();
void destroy();

int main() {
  if (init())
    return 1;
  if (load())
    return 1;

  // delay 5 ms before loading the next 'frame'
  while (!loop())
    SDL_Delay(5);

  destroy();
  return 0;
}

int init() {
  win = SDL_CreateWindow(
    "event_window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);
  if (!win) {
    printf("Error creating window:\n\tError: %s", SDL_GetError());
    return 1;
  }

  win_surface = SDL_GetWindowSurface(win);
  if (!win_surface) {
    printf("Error getting window surface:\n\tError: %s", SDL_GetError());
    return 1;
  }

  // check if running on MacOS
#if defined(__APPLE__)
  // a 'feature' of MacOS is that the window will not show unless you pump the event queue
  //SDL_PumpEvents();
#endif

  return 0;
}

int load() {
  SDL_Surface *tmp0 = SDL_LoadBMP("bitmaps/test.bmp");
  if (!tmp0) {
    printf("Failed to load image:\n\tError: %s", SDL_GetError());
    return 1;
  }

  SDL_Surface *tmp1 = SDL_LoadBMP("bitmaps/test2.bmp");
  if (!tmp1) {
    printf("Failed to load image:\n\tError: %s", SDL_GetError());
    return 1;
  }

  img0 = SDL_ConvertSurface(tmp0, win_surface->format, 0);
  img1 = SDL_ConvertSurface(tmp1, win_surface->format, 0);
  if (!img0 || !img1) {
    printf("Failed to convert image:\n\tError: %s", SDL_GetError());
    return 1;
  }

  SDL_FreeSurface(tmp0);
  SDL_FreeSurface(tmp1);

  return 0;
}

int loop() {
  static int render_img1 = 0;
  SDL_Event e;

  SDL_BlitSurface(img0, NULL, win_surface, NULL);

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_QUIT:
        return 1;
      case SDL_KEYDOWN:
        render_img1 = 1;
        break;
      case SDL_KEYUP:
        render_img1 = 0;
        break;
      case SDL_MOUSEMOTION:
        break;
    }
  }

  if (render_img1) {
    SDL_Rect dest;
    dest.w = 120;
    dest.h = 120;
    dest.x = 320;
    dest.y = 240;
    SDL_BlitScaled(img1, NULL, win_surface, &dest);
  }

  SDL_UpdateWindowSurface(win);

  return 0;
}

void destroy() {
  SDL_FreeSurface(img0);
  SDL_FreeSurface(img1);
  SDL_DestroyWindow(win);
  SDL_Quit();
}
