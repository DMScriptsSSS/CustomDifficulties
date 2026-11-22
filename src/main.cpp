
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelCell.hpp>

using namespace geode::prelude;

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

static int getDifficulty(int id) {
    return Mod::get()->getSavedValue<int>(
        fmt::format("level-{}", id), -1
    );
}

static void saveDifficulty(int id, int value) {
    Mod::get()->setSavedValue<int>(
        fmt::format("level-{}", id), value
    );
}

class $modify(CustomLevelInfo, LevelInfoLayer) {
    struct Fields {
        int levelID = 0;
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge))
            return false;

        if (!level) return true;

        m_fields->levelID = level->m_levelID;

        auto menu = CCMenu::create();
        auto label = CCLabelBMFont::create(
            "CUSTOM DIFFICULTY", "bigFont.fnt"
        );

        label->setScale(0.35f);

        auto button = CCMenuItemLabel::create(
            label,
            this,
            menu_selector(CustomLevelInfo::changeDifficulty)
        );

        menu->addChild(button);
        menu->setPosition({
            CCDirector::sharedDirector()->getWinSize().width / 2,
            45
        });

        this->addChild(menu, 100);

        updateDifficultyLabel();

        return true;
    }

    void updateDifficultyLabel() {
        int value = getDifficulty(m_fields->levelID);
        if (value < 0 || value >= 3) return;

        auto label = CCLabelBMFont::create(
            names[value], "bigFont.fnt"
        );

        if (!label || !m_difficultySprite) return;

        label->setScale(0.45f);
        label->setPosition(m_difficultySprite->getPosition());

        this->addChild(label, 100);
        m_difficultySprite->setOpacity(0);
    }

    void changeDifficulty(CCObject*) {
        int value = getDifficulty(m_fields->levelID);
        value = (value + 1) % 3;

        saveDifficulty(m_fields->levelID, value);

        FLAlertLayer::create(
            "Custom Difficulties",
            fmt::format(
                "Saved: <cy>{}</c>",
                names[value]
            ),
            "OK"
        )->show();

        updateDifficultyLabel();
    }
};

class $modify(CustomLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);

        if (!level) return;

        int value = getDifficulty(level->m_levelID);

        if (value < 0 || value >= 3) return;

        auto container =
            m_mainLayer->getChildByID("difficulty-container");

        if (!container) return;

        auto original = container->getChildByID(
            "difficulty-sprite"
        );

        if (!original) return;

        original->setVisible(false);

        auto label = CCLabelBMFont::create(
            shortNames[value], "bigFont.fnt"
        );

        if (!label) return;

        label->setScale(0.4f);
        label->setPosition(original->getPosition());
        label->setID("custom-difficulty-label"_spr);

        container->addChild(label);
    }
};

