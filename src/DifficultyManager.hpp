#pragma once

#include <Geode/Geode.hpp>

namespace DifficultyManager {
    enum Difficulty {
        NONE = -1,
        INSANE_PLUS = 0,
        EXTREME_PLUS = 1,
        IMPOSSIBLE = 2
    };

    int get(int levelID);
    void set(int levelID, int difficulty);

    const char* getName(int difficulty);
    const char* getShortName(int difficulty);

    int count();
}
