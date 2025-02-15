//
// Created by nikita on 15.02.2025.
//

#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL_video.h>

namespace eng {
    class Window {
    public:
        explicit Window(SDL_Window *window) : window(window) {
        }

        ~Window() {
            SDL_DestroyWindow(window);
        }

    private:
        SDL_Window *window;
    };
} // eng

#endif //WINDOW_H
