#include <unistd.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720
#define BUFFER_SIZE   100

// define global video variables
SDL_Window* win;
SDL_Renderer* renderer;

int init();
int loop();
int destroy();

typedef struct {
  int x0;
  int y0;
  int x1;
  int y1;
} line_pos;

line_pos line_buf[BUFFER_SIZE];

int main() {
  if (init())
    return 1;
  while (!loop())
    SDL_Delay(5); //wait 5ms before rendering the next frame

  destroy();
  return 0;
}

int init() {
  win = SDL_CreateWindow(
    "render_test",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    SCREEN_WIDTH, SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN);

  if (!win) {
    printf("Failed to create window:\n\tError: %s", SDL_GetError());
    return 1;
  }
  // check if running on MacOS
#if defined(__APPLE__)
  // a 'feature' of MacOS is that the window will not show unless you pump the event queue
  SDL_PumpEvents();
#endif

  // initialize renderer for window win, with idx -1 meaning just get first rendering driver compatible with flags: SDL_RENDERER_PRESENTVSYNC
  renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    printf("Failed to create renderer:\n\tError: %s", SDL_GetError());
    return 1;
  }
  // set background color to black and clear the rendere's buffer
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  // set renderer draw mode to blend mode so we can set transparency with alpha
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);

  return 0;
}

int loop() {
  SDL_Event e;
  static line_pos my_line = {-1, -1, -1, -1};
  static int line_idx = 0;
  line_pos* curr_line;

  // clear the renderer initially
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_QUIT:
        return 1;
      case SDL_MOUSEBUTTONDOWN:
        my_line.x0 = e.button.x;
        my_line.y0 = e.button.y;
        break;
      case SDL_MOUSEMOTION:
        if (my_line.x0 < 0 || e.button.x < 0)
          break;
        my_line.x1 = e.button.x;
        my_line.y1 = e.button.y;
        break;
      case SDL_MOUSEBUTTONUP:
        curr_line = &line_buf[line_idx];
        curr_line->x0 = my_line.x0;
        curr_line->y0 = my_line.y0;
        curr_line->x1 = my_line.x1;
        curr_line->y1 = my_line.y1;
        my_line.x0 = -1;
        my_line.y0 = -1;
        my_line.x1 = -1;
        my_line.y1 = -1;
        line_idx = (line_idx + 1) % BUFFER_SIZE;
        break;
    }
  }

  // set draw color to white
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  if (my_line.x1 > 0 &&  my_line.y1 > 0)
    SDL_RenderDrawLine(renderer, my_line.x0, my_line.y0, my_line.x1, my_line.y1);
  for (int i = 0; i < BUFFER_SIZE; i++)
    SDL_RenderDrawLine(renderer, line_buf[i].x0, line_buf[i].y0, line_buf[i].x1, line_buf[i].y1);

  SDL_RenderPresent(renderer);

  return 0;
}

int destroy() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindowSurface(win);
  SDL_Quit();
  return 0;
}
