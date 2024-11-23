#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>

int main() {
  // initialize SDL
  SDL_Init(SDL_INIT_EVERYTHING);
  // initialize an SDL Window  
  // args: name: "test_window", positon [undefined, undefined], dimensions [1280w, 720h], options: SDL_WINDOW_SHOWN
  SDL_Window *win =SDL_CreateWindow(
    "test_window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);

  if (!win) {
    printf("Failed to create window:\n\tError: %s", SDL_GetError());
  }

  /* 
     draw to window (Software Rendering)

    Note:
     - In SDL CPU/Software rendering abstracts an area, surface, to draw to
     - GPU rendering does not utilize these surfaces
  */

  // initialize the surface to draw to
  SDL_Surface* win_surface = SDL_GetWindowSurface(win);

  if (!win_surface){
    printf("Failed to create drawable surface:\n\tError: %s", SDL_GetError());
    return 1;
  }

  // check if running on MacOS
#if defined(__APPLE__)
  // a 'feature' of MacOS is that the window will not show unless you pump the event queue
  SDL_PumpEvents();
#endif

  // draw to the surface
  SDL_FillRect(win_surface, NULL, SDL_MapRGB(win_surface->format, 255, 90, 120));

  // update the window
  SDL_UpdateWindowSurface(win);

  // wait so we can see the window for 5 seconds
  sleep(5);
  
  // cleanup
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
