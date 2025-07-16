#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_render.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 400

#define MVMT_SPEED 10

enum dir {
  LEFT = -1,
  RIGHT = 1,
  FWD = 1,
  BACK = -1,
};

enum side { x_side, y_side };

typedef uint32_t u32;
typedef uint8_t u8;

typedef struct {
  float x, y;
} vec2;

typedef struct {
  int x, y;
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
  player player;
} state;

int map[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0,
    1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

void clear_pixels() {
  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) state.pixels[i] = 0x0;
}

void validate_location(vec2* loc) {
  loc->x = (loc->x < 0) ? 0 : loc->x;
  loc->y = (loc->y < 0) ? 0 : loc->y;
  loc->x = (loc->x > SCREEN_WIDTH - 1) ? SCREEN_WIDTH - 1 : loc->x;
  loc->y = (loc->y > SCREEN_HEIGHT - 1) ? SCREEN_HEIGHT - 1 : loc->y;
}

void draw_line(vec2* start, vec2* end, u32 c) {
  validate_location(start);
  validate_location(end);
  float dx = end->x - start->x;
  float dy = end->y - start->y;
  float len = sqrt(dx * dx + dy * dy);
  float angle = atan2(dy, dx);
  int x, y;

  for (float i = 0; i < len; i++) {
    x = (int)(start->x + i * cos(angle)) + 1;
    y = (int)(start->y + i * sin(angle)) + 1;
    state.pixels[y * SCREEN_WIDTH + x] = c;
  }
}

int init();
int loop();
int destroy();

int main() {
  if (init()) return 1;
  while (!loop()) SDL_Delay(5);
  destroy();
  return 0;
}

int init() {
  state.win = SDL_CreateWindow("raycast_test", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!state.win) {
    printf("Failed to create window:\n\tError: %s", SDL_GetError());
    return 1;
  }

  state.renderer = SDL_CreateRenderer(state.win, -1, SDL_RENDERER_PRESENTVSYNC);
  if (!state.renderer) {
    printf("Failed to create renderer:\n\tError: %s", SDL_GetError());
    return 1;
  }

  state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888,
                                    SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH,
                                    SCREEN_HEIGHT);
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

  return 0;
}

void render_screen() {
  SDL_UpdateTexture(state.texture, NULL, state.pixels,
                    SCREEN_WIDTH * sizeof(u32));
  SDL_RenderCopyEx(state.renderer, state.texture, NULL, NULL, 0.0, NULL,
                   SDL_FLIP_VERTICAL);

  SDL_RenderPresent(state.renderer);
}

void move(int dir) {
  player* p = &state.player;
  float angle = atan(state.player.dir.x / state.player.dir.y);

  if (dir == FWD || dir == BACK) {
    p->pos.x += cos((float)angle * M_PI / 180) * dir * MVMT_SPEED;
    p->pos.y += sin((float)angle * M_PI / 180) * dir * MVMT_SPEED;
  } else {
    p->pos.x += cos((float)(angle + 90 * (dir / 2)) * M_PI / 180) * MVMT_SPEED;
    p->pos.y += sin((float)(angle + 90 * (dir / 2)) * M_PI / 180) * MVMT_SPEED;
  }
}

int loop() {
  static float time = 0, prev_time = 0;
  prev_time = time;
  time = SDL_GetTicks();
  float dt = (time - prev_time) / 1000.0;

  clear_pixels();
  SDL_Event e;

  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_QUIT:
        return 1;
      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
          case SDLK_a:
            move(LEFT);
          case SDLK_s:
            move(BACK);
          case SDLK_d:
            move(RIGHT);
          case SDLK_w:
            move(FWD);
        }
        break;
    }
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
