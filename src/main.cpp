#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontSave) {
        if (!PlayLayer::init(level, useReplay, dontSave)) return false;

        // Auto jump at start si Hold Level está activo
        if (Mod::get()->getSettingValue<bool>("hold-level")) {
            this->handleButton(true, static_cast<int>(PlayerButton::Jump), false);
        }
        return true;
    }

    void handleButton(bool hold, int button, bool isPlayer2) {
        bool ignoreClicks = Mod::get()->getSettingValue<bool>("ignore-clicks");
        bool holdActive = Mod::get()->getSettingValue<bool>("hold-level");

        // Bloqueo total si Ignore Clicks está activo
        if (ignoreClicks) return;

        // Si Hold Level está activo y el usuario presiona manualmente, se desactiva
        if (hold && holdActive) {
            Mod::get()->setSettingValue("hold-level", false);
        }

        PlayLayer::handleButton(hold, button, isPlayer2);
    }

    void onExit() {
        // Lógica de Auto Reset
        if (Mod::get()->getSettingValue<bool>("auto-reset")) {
            Mod::get()->setSettingValue("hold-level", false);
            Mod::get()->setSettingValue("ignore-clicks", false);
        }
        PlayLayer::onExit();
    }
};

class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto winSize = CCDirector::get()->getWinSize();
        auto side = Mod::get()->getSettingValue<std::string>("button-side");

        // Crear el botón de ajustes
        auto btnSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        auto btn = CCMenuItemSpriteExtra::create(
            btnSprite, this, menu_selector(MyPauseLayer::onMySettings)
        );

        auto menu = CCMenu::create();
        
        // Posicionamiento dinámico
        float posX = (side == "left") ? 35.f : winSize.width - 35.f;
        menu->setPosition({posX, winSize.height / 2});

        menu->addChild(btn);
        this->addChild(menu);
    }

    void onMySettings(CCObject* sender) {
        // Método correcto para abrir los ajustes del mod en la 2.2081
        openSettingsPopup(Mod::get());
    }
};
