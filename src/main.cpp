#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LocalLevelManager.hpp>
#include <Geode/modify/LevelTools.hpp>
#include <unordered_set>

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

class $modify(CustomLocalLevelManager, LocalLevelManager) {
    std::string getMainLevelString(int id) {
        log::info("[CD] getMainLevelString chamado: {}", id);

        if (id != 23)
            return LocalLevelManager::getMainLevelString(id);

        auto file = CCString::createWithFormat(
            "levels/level%i.txt"_spr,
            id
        );

        if (!file) {
            log::error("[CD] Caminho do arquivo falhou");
            return "";
        }

        auto content = CCString::createWithContentsOfFile(
            file->getCString()
        );

        if (!content) {
            log::error("[CD] Arquivo da fase 23 nao encontrado: {}",
                file->getCString());
            return "";
        }

        log::info("[CD] Fase 23 carregada!");
        return std::string(content->getCString());
    }
};

class $modify(CustomLevelTools, LevelTools) {
    static std::unordered_set<int> getLevelList() {
        log::info("[CD] getLevelList chamado");

        auto levels = LevelTools::getLevelList();
        levels.insert(23);

        log::info("[CD] Lista modificada. Total: {}", levels.size());
        return levels;
    }

    static GJGameLevel* getLevel(int levelID, bool loaded) {
        log::info("[CD] getLevel chamado: {}", levelID);

        if (levelID != 23)
            return LevelTools::getLevel(levelID, loaded);

        auto level = GJGameLevel::create();

        if (!level) {
            log::error("[CD] Nao foi possivel criar GJGameLevel");
            return nullptr;
        }

        level->m_levelName = "iSpyWithMyLittleEye";
        level->m_audioTrack = 0;

        if (!loaded) {
            level->m_levelString =
                LocalLevelManager::sharedState()
                    ->getMainLevelString(levelID);
        }

        level->m_levelID = levelID;
        level->m_levelType = GJLevelType::Saved;

        log::info("[CD] Objeto da fase 23 criado");
        return level;
    }

    static bool verifyLevelIntegrity(
        std::string verifyString,
        int levelID
    ) {
        if (levelID == 23) {
            log::info("[CD] Verificacao da fase 23");
            return true;
        }

        return LevelTools::verifyLevelIntegrity(
            verifyString,
            levelID
        );
    }
};

class $modify(CustomLevelInfo, LevelInfoLayer) {
    struct Fields {
        int levelID = 0;
    };

    bool init(GJGameLevel* level, bool challenge) {
        log::info("[CD] LevelInfoLayer::init entrou");

        if (!LevelInfoLayer::init(level, challenge)) {
            log::error("[CD] LevelInfoLayer::init retornou false");
            return false;
        }

        log::info("[CD] LevelInfoLayer original inicializado");

        if (!level) {
            log::error("[CD] Level recebido e nulo");
            return true;
        }

        m_fields->levelID = level->m_levelID;

        auto menu = CCMenu::create();

        auto label = CCLabelBMFont::create(
            "CUSTOM DIFFICULTY",
            "bigFont.fnt"
        );

        if (!menu || !label) {
            log::error("[CD] Falha ao criar menu ou texto");
            return true;
        }

        label->setScale(0.35f);

        auto button = CCMenuItemLabel::create(
            label,
            this,
            menu_selector(CustomLevelInfo::changeDifficulty)
        );

        if (!button) {
            log::error("[CD] Falha ao criar botao");
            return true;
        }

        menu->addChild(button);

        auto size = CCDirector::sharedDirector()->getWinSize();

        menu->setPosition({
            size.width / 2,
            45
        });

        this->addChild(menu, 100);

        log::info("[CD] Botao adicionado. Level ID: {}",
            m_fields->levelID);

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
        label->setPosition(m_difficultySprite->getPosition());
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
            fmt::format("Saved: <cy>{}</c>", names[value]),
            "OK"
        )->show();

        updateDifficultyLabel();
    }
};

class $modify(CustomLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        log::info("[CD] LevelCell::loadFromLevel chamado");

        LevelCell::loadFromLevel(level);

        if (!level || !m_mainLayer)
            return;

        int value = getDifficulty(level->m_levelID);

        if (value < 0 || value >= 3)
            return;

        auto container = m_mainLayer->getChildByID(
            "difficulty-container"
        );

        if (!container) {
            log::info("[CD] difficulty-container nao encontrado");
            return;
        }

        auto original = container->getChildByID(
            "difficulty-sprite"
        );

        if (!original) {
            log::info("[CD] difficulty-sprite nao encontrado");
            return;
        }

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
