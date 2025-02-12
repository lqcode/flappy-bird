#ifndef MAIN_H
#define MAIN_H

#include <SDL3/SDL.h>

#define APP_NAME "Flappy Bird"
#define APP_VERSION "1.0"
#define APP_ID "ru.lqcode.flappybird"

#define WINDOW_TITLE "Flappy Bird"
#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 1280

#define GET_STATE AppState *state = (AppState*)appstate;

typedef struct {
    SDL_Window *window;
} AppState;

#endif //MAIN_H
