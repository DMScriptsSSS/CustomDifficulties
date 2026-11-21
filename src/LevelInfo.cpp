#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "DifficultyManager.hpp"

using namespace geode::prelude;

class $modify(CustomDifficultyInfo, LevelInfoLayer) {
    struct Fields {
        int levelID = 0;
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge))
            return false;

        if (!level) return true;

        m_fields->levelID = level->m_levelID;

        auto label = CCLabelBMFont::create(
            "CUSTOM DIFFICULTY", "bigFont.fnt"
        );
        label->setScale(0.4f);

        auto button = CCMenuItemLabel::create(
            label,
            this,
            menu_selector(CustomDifficultyInfo::changeDifficulty)
        );

        auto menu = CCMenu::create();
        menu->addChild(button);
        menu->setPosition({
            CCDirector::sharedDirector()->getWinSize().width / 2,
            45
        });

        this->addChild(menu, 100);

        return true;
    }

    void changeDifficulty(CCObject*) {
        int value = DifficultyManager::get(m_fields->levelID);
        value = (value + 1) % DifficultyManager::count();

        DifficultyManager::set(m_fields->levelID, value);

        FLAlertLayer::create(
            "Custom Difficulties",
            fmt::format(
                "Classification: <cy>{}</c>",
                DifficultyManager::getName(value)
            ),
            "OK"
        )->show();
    }
};
