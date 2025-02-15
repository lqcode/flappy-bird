//
// Created by nikita on 15.02.2025.
//

#ifndef GAME_H
#define GAME_H
#include <cstdint>

namespace game {
    class Game {
    public:
        void init();

        bool update(uint64_t delta);

        void render();
    };
} // game

#endif //GAME_H
