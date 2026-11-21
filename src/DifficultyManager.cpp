#include "DifficultyManager.hpp"

using namespace geode::prelude;

namespace DifficultyManager {

    static const char* names[] = {
        "Insane+",
        "Extreme+",
        "Impossible"
    };

    static const char* shortNames[] = {
        "I+",
        "E+",
        "IMP"
    };

    int get(int levelID) {
        return Mod::get()->getSavedValue<int>(
            fmt::format("level-{}", levelID), -1
        );
    }

    void set(int levelID, int difficulty) {
        if (difficulty < -1 || difficulty >= 3)
            return;

        Mod::get()->setSavedValue<int>(
            fmt::format("level-{}", levelID),
            difficulty
        );
    }

    const char* getName(int difficulty) {
        if (difficulty < 0 || difficulty >= 3)
            return "Unclassified";

        return names[difficulty];
    }

    const char* getShortName(int difficulty) {
        if (difficulty < 0 || difficulty >= 3)
            return "";

        return shortNames[difficulty];
    }

    int count() {
        return 3;
    }
}
