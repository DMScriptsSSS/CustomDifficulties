#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LocalLevelManager.hpp>
#include <Geode/modify/LevelTools.hpp>

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
    if (value < -1 || value >= 3)
        return;

    Mod::get()->setSavedValue<int>(
        fmt::format("level-{}", id), value
    );
}

// Carrega os dados da fase personalizada
class $modify(CustomLocalLevelManager, LocalLevelManager) {
    gd::string getMainLevelString(int id) {
        if (id != 23)
            return LocalLevelManager::getMainLevelString(id);

        auto file = CCString::createWithFormat(
            "levels/level%i.txt"_spr,
            id
        );

        if (!file)
            return "";

        auto content = CCString::createWithContentsOfFile(
            file->getCString()
        );

        if (!content)
            return "";

        return gd::string(content->getCString());
    }
};

// Registra os dados da fase
class $modify(CustomLevelTools, LevelTools) {
    GJGameLevel* getLevel(int levelID, bool loaded) {
        if (levelID != 23)
            return LevelTools::getLevel(levelID, loaded);

        auto level = GJGameLevel::create();

        if (!level)
            return nullptr;

        level->m_levelName = "iSpyWithMyLittleEye";
        level->m_audioTrack = 0;

        if (!loaded) {
            level->m_levelString =
                LocalLevelManager::sharedState()
                    ->getMainLevelString(levelID);
        }

        level->m_levelID = levelID;
        level->m_levelType = GJLevelType::Saved;

        return level;
    }

    bool verifyLevelIntegrity(
        gd::string verifyString,
        int levelID
    ) {
        if (levelID == 23)
            return true;

        return LevelTools::verifyLevelIntegrity(
            verifyString,
            levelID
        );
    }
};

// Sistema de dificuldades personalizadas
class $modify(CustomLevelInfo, LevelInfoLayer) {
    struct Fields {
        int levelID = 0;
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge))
            return false;

        if (!level)
            return true;

        m_fields->levelID = level->m_levelID;

        auto menu = CCMenu::create();

        auto label = CCLabelBMFont::create(
            "CUSTOM DIFFICULTY",
            "bigFont.fnt"
        );

        if (!menu || !label)
            return true;

        label->setScale(0.35f);

        auto button = CCMenuItemLabel::create(
            label,
            this,
            menu_selector(CustomLevelInfo::changeDifficulty)
        );

        if (!button)
            return true;

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
        auto oldLabel = this->getChildByID(
            "custom-difficulty-info-label"_spr
        );

        if (oldLabel)
            oldLabel->removeFromParent();

        int value = getDifficulty(m_fields->levelID);

        if (value < 0 || value >= 3)
            return;

        auto label = CCLabelBMFont::create(
            names[value],
            "bigFont.fnt"
        );

        if (!label || !m_difficultySprite)
            return;

        label->setScale(0.45f);
        label->setPosition(
            m_difficultySprite->getPosition()
        );

        label->setID("custom-difficulty-info-label"_spr);

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

        if (!level || !m_mainLayer)
            return;

        auto oldLabel = m_mainLayer->getChildByID(
            "custom-difficulty-label"_spr
        );

        if (oldLabel)
            oldLabel->removeFromParent();

        int value = getDifficulty(level->m_levelID);

        if (value < 0 || value >= 3)
            return;

        auto container = m_mainLayer->getChildByID(
            "difficulty-container"
        );

        if (!container)
            return;

        auto original = container->getChildByID(
            "difficulty-sprite"
        );

        if (!original)
            return;

        original->setVisible(false);

        auto label = CCLabelBMFont::create(
            shortNames[value],
            "bigFont.fnt"
        );

        if (!label)
            return;

        label->setScale(0.4f);
        label->setPosition(original->getPosition());
        label->setID("custom-difficulty-label"_spr);

        container->addChild(label);
    }
};

#include <Geode/modify/BoomScrollLayer.hpp>

class $modify(CustomLevelList, BoomScrollLayer) {
    static BoomScrollLayer* create(
        CCArray* pages,
        int unk1,
        bool unk2,
        CCArray* levels,
        DynamicScrollDelegate* delegate
    ) {
        if (
            delegate &&
            levels &&
            exact_cast<LevelSelectLayer*>(delegate)
        ) {
            bool exists = false;

            for (unsigned int i = 0; i < levels->count(); i++) {
                auto level = typeinfo_cast<GJGameLevel*>(
                    levels->objectAtIndex(i)
                );

                if (level && level->m_levelID == 23) {
                    exists = true;
                    break;
                }
            }

            if (!exists) {
                auto customLevel =
                    GameLevelManager::get()->getMainLevel(23, 0);

                if (customLevel)
                    levels->addObject(customLevel);
            }
        }

        return BoomScrollLayer::create(
            pages,
            unk1,
            unk2,
            levels,
            delegate
        );
    }
};
