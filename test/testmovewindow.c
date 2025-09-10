#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

#define SPEED 10
int velX = SPEED;
int velY = SPEED;
int scale = SPEED / 2;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Failed to intiialize SDL: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 320);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 240);
    SDL_SetFloatProperty(props, SDL_PROP_WINDOW_CREATE_EMSCRIPTEN_SYNC_WINDOW_RATE_FLOAT, 16);
    window = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
    	fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  	SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
	static Uint64 last_update = 0;
	const Uint64 now = SDL_GetTicks();

	if (now - last_update > 10) {
		SDL_DisplayID displayID = SDL_GetDisplayForWindow(window);

		SDL_Rect bounds;
		SDL_GetDisplayBounds(displayID, &bounds);

		SDL_Rect rect;
		SDL_GetWindowPosition(window, &rect.x, &rect.y);
		SDL_GetWindowSize(window, &rect.w, &rect.h);

		rect.x += velX;
		rect.y += velY;
		rect.w += scale;
		rect.h += scale;

		if (rect.w > bounds.w || rect.h > bounds.h) {
			scale = SPEED / -2;
		} else if (rect.w < 10 || rect.h < 10) {
			scale = SPEED / 2;
		}

		if (rect.x < bounds.x) {
			velX = SPEED;
		} else if (rect.x + rect.w > bounds.x + bounds.w) {
			velX = -SPEED;
		}

		if (rect.y < bounds.y) {
			velY = SPEED;
		} else if (rect.y + rect.h > bounds.y + bounds.h) {
			velY = -SPEED;
		}

		SDL_SetWindowPosition(window, rect.x, rect.y);
		SDL_SetWindowSize(window, rect.w, rect.h);

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderClear(renderer);
    	SDL_RenderPresent(renderer);

    	last_update = now;
	}
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}