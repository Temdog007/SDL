#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

SDL_Window *sharedCtxWindow = NULL;
SDL_GLContext sharedCtx = NULL;

SDL_Window *window = NULL;
SDL_GLContext windowCtx = NULL;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Failed to intiialize SDL: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    window = SDL_CreateWindow("Example", 640, 480, SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    windowCtx = SDL_GL_CreateContext(window);
    if (windowCtx == NULL)
    {
        fprintf(stderr, "Failed to create context: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 1);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 1);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_EMSCRIPTEN_CANVAS_ID_STRING, "#shared");
    sharedCtxWindow = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);
    if (sharedCtxWindow == NULL)
    {
        fprintf(stderr, "Failed to create shared window: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    sharedCtx = SDL_GL_CreateContext(sharedCtxWindow);
    if (sharedCtx == NULL)
    {
        fprintf(stderr, "Failed to create shared context: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_GL_MakeCurrent(sharedCtxWindow, sharedCtx))
    {
        fprintf(stderr, "Failed to change current context: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

    if (!SDL_GL_MakeCurrent(window, windowCtx))
    {
        fprintf(stderr, "Failed to change current context: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_GL_DestroyContext(sharedCtx);
    SDL_DestroyWindow(sharedCtxWindow);

    SDL_GL_DestroyContext(windowCtx);
    SDL_DestroyWindow(window);
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_GL_SwapWindow(window);
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