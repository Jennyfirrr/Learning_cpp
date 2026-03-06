#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {

  bool running = true;

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window =
      SDL_CreateWindow("fractal", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);

  SDL_Renderer *render =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_SetRenderDrawColor(render, 10, 10, 26, 255);

  double centerX = -0.5, centerY = 0.0, zoom = 1.0;

  while (running == true) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
      if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mx = event.button.x;
        int my = event.button.y;

        centerX = centerX + (mx / 800.0 - 0.5) * (3.5 / zoom);
        centerY = centerY + (my / 600.0 - 0.5) * (2.0 / zoom);
      }
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_r) {
          centerX = -0.5;
          centerY = 0.0;
          zoom = 1.0;
        }
        if (event.key.keysym.sym == SDLK_EQUALS) {
          zoom *= 2.0;
        }
        if (event.key.keysym.sym == SDLK_MINUS) {
          zoom *= 0.5;
        }
      }
    }

    for (int px = 0; px < 800; px++) {
      for (int py = 0; py < 600; py++) {
        double viewWidth = 3.5 / zoom;
        double viewHeight = 2.0 / zoom;

        double cx = centerX + (px / 800.0 - 0.5) * viewWidth;
        double cy = centerY + (py / 600.0 - 0.5) * viewHeight;

        double zx = 0, zy = 0;
        int iter = 0;

        while (zx * zx + zy * zy < 4.0 && iter < 100) {
          double new_zx = zx * zx - zy * zy + cx;
          zy = 2 * zx * zy + cy;
          zx = new_zx;
          iter++;
        }

        if (iter == 100) {
          SDL_SetRenderDrawColor(render, 10, 10, 26, 255);
        } else {
          SDL_SetRenderDrawColor(render, iter * 5 % 255, iter * 3 % 255,
                                 iter * 7 % 255, 255);
        }
        SDL_RenderDrawPoint(render, px, py);
      }
    }
    SDL_RenderPresent(render);
  }

  SDL_DestroyRenderer(render);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
