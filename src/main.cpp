#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontSave) {
        if (!PlayLayer::init(level, useReplay, dontSave)) return false;

        // Auto jump at start
        if (Mod::get()->getSettingValue<bool>("hold-level")) {
            this->handleButton(true, static_cast<int>(PlayerButton::Jump), false);
        }
        return true;
    }

    void handleButton(bool hold, int button, bool isPlayer2) {
        bool ignoreClicks = Mod::get()->getSettingValue<bool>("ignore-clicks");
        bool holdActive = Mod::get()->getSettingValue<bool>("hold-level");

        // If Ignore Clicks is ON, we block all input
        if (ignoreClicks) return;

        // If Hold Level is ON, the first manual click takes control and disables it
        if (hold && holdActive) {
            Mod::get()->setSettingValue("hold-level", false);
        }

        PlayLayer::handleButton(hold, button, isPlayer2);
    }

    void onExit() {
        // Reset settings if Auto Reset is enabled
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

        auto btnSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        auto btn = CCMenuItemSpriteExtra::create(
            btnSprite, this, menu_selector(MyPauseLayer::onMySettings)
        );

        auto menu = CCMenu::create();
        float posX = (side == "left") ? 35.f : winSize.width - 35.f;
        menu->setPosition({posX, winSize.height / 2});

        menu->addChild(btn);
        this->addChild(menu);
    }

    void onMySettings(CCObject* sender) {
        geode::openSettingsPopup(Mod::get());
    }
};
            
