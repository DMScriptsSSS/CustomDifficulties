#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include "DifficultyManager.hpp"

using namespace geode::prelude;

class $modify(CustomDifficultyCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);

        if (!level || !m_mainLayer)
            return;

        int value = DifficultyManager::get(level->m_levelID);

        if (value < 0 || value >= DifficultyManager::count())
            return;

        auto label = CCLabelBMFont::create(
            DifficultyManager::getShortName(value),
            "bigFont.fnt"
        );

        if (!label) return;

        label->setScale(0.4f);
        label->setPosition({25, 20});
        label->setID("custom-difficulty-label"_spr);

        m_mainLayer->addChild(label, 100);
    }
};
