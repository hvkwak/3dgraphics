#include "display.h"
#include <SDL2/SDL_image.h>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

static uint32_t* framebuffer = NULL;

static SDL_Texture* framebuffer_texture = NULL;

static uint16_t window_width = 800;
static uint16_t window_height = 800;

// Save Variables
static int save_width = 0;
static int save_height = 0;
static Uint8* save_pixels = NULL;
static int save_pitch = 0;
static SDL_Surface *save_surface = NULL;
static SDL_Texture *save_texture = NULL;
static bool is_export = false;
static int capture_idx = 0;
static int capture_max = 500;

void set_export(bool isExport){
    is_export = isExport;
}

bool create_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }

  window = SDL_CreateWindow("Triangle rasterization", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, 0);
  if (!window) {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    fprintf(stderr, "Error creating SDL renderer.\n");
    return false;
  }

  framebuffer = malloc(sizeof(uint32_t) * SCREEN_WIDTH * SCREEN_HEIGHT);
  if (!framebuffer) {
    fprintf(stderr, "Error allocating memory for the framebuffer.\n");
    return false;
  }
  
  framebuffer_texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_RGBA32,
    SDL_TEXTUREACCESS_STREAMING,
    SCREEN_WIDTH,
    SCREEN_HEIGHT
  );

  // initialize Saver
  // Create an SDL texture that is used to save
  // Allocate downsized pixel buffer (ARGB8888 = 4 bytes per pixel) for PNG
  // export
  save_width = window_width / 2;
  save_height = window_height / 2;
  save_pitch = save_width * 4; // ARGB8888 => 4 bytes per pixel

  // allocate save pixels
  save_pixels = (Uint8 *)malloc(save_pitch * save_height);
  if (!save_pixels) {
    fprintf(stderr, "Memory allocation failed\n");
    return false;
  }

  // Better downscale quality
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"); // "1" linear, "2" best available

  save_texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_TARGET, save_width, save_height);
  if (!save_texture) {
    fprintf(stderr, "CreateTexture failed: %s\n", SDL_GetError());
    return false;
  }

  return true;
}

void draw_pixel(uint8_t x, uint8_t y, uint32_t color) {
  if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
    return;
  }
  framebuffer[(SCREEN_WIDTH * y) + x] = color;
}

void render_framebuffer(void) {
  SDL_UpdateTexture(
    framebuffer_texture,
    NULL,
    framebuffer,
    (int)(SCREEN_WIDTH * sizeof(uint32_t))
  );

  // Export in PNG
  if (is_export && SDL_GetTicks() > 3000 && capture_idx < capture_max) {

    // Render full-res texture into small_rt at half size
    SDL_SetRenderTarget(renderer, save_texture);
    SDL_RenderClear(renderer);
    SDL_Rect dst = {0, 0, save_width, save_height};
    SDL_RenderCopy(renderer, framebuffer_texture, NULL, &dst);
    SDL_RenderFlush(renderer);

    // Read pixels from renderer: renderer -> save_pixels
    if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, save_pixels, save_pitch) != 0) {
      fprintf(stderr, "SDL_RenderReadPixels failed: %s\n", SDL_GetError());
      return;
    }

    // Wrap pixel data in SDL_Surface
    save_surface = SDL_CreateRGBSurfaceFrom(save_pixels, save_width,
                                            save_height, 32, save_pitch,
                                            0x00FF0000, // R
                                            0x0000FF00, // G
                                            0x000000FF, // B
                                            0xFF000000  // A
    );

    if (!save_surface) {
      fprintf(stderr, "SDL_CreateRGBSurfaceFrom failed: %s\n", SDL_GetError());
      return;
    }

    // Save to PNG
    char path[256];
    snprintf(path, sizeof(path), "../captures-tr/frame_%04d.png", ++capture_idx);
    if (IMG_SavePNG(save_surface, path) == 0) {
      printf("[cap] %s\n", path);
    } else {
      fprintf(stderr, "IMG_SavePNG failed: %s\n", SDL_GetError());
    }

    // Restore render target back to window
    SDL_SetRenderTarget(renderer, NULL);

    // Free save_surface
    SDL_FreeSurface(save_surface);
  }

  SDL_RenderCopy(renderer, framebuffer_texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void clear_framebuffer(uint32_t color) {
  for (uint16_t i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    framebuffer[i] = color;
  }
}

void fix_framerate(void) {
  static int32_t previous_frame_time = 0;
  int time_ellapsed = SDL_GetTicks() - previous_frame_time;
  int time_to_wait = MILLISECS_PER_FRAME - time_ellapsed;
  if (time_to_wait > 0 && time_to_wait < MILLISECS_PER_FRAME) {
    SDL_Delay(time_to_wait);
  }
  previous_frame_time = SDL_GetTicks();
}

void destroy_window(void) {
  if (save_pixels != NULL) {
    free(save_pixels);
  }
  free(framebuffer);
  SDL_DestroyTexture(framebuffer_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
