#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "MirrorPopup.hpp"

using namespace geode::prelude;

bool g_originalMirrorState = false;

class $modify(MyPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontSave) {
        if (!PlayLayer::init(level, useReplay, dontSave)) return false;
        if (m_levelSettings) {
            g_originalMirrorState = m_levelSettings->m_mirrorMode;
        }
        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        if (m_levelSettings && Mod::get()->getSettingValue<bool>("mirror-reset-on-death")) {
            m_levelSettings->m_mirrorMode = g_originalMirrorState;
        }
    }

    void onQuit() {
        if (m_levelSettings) {
            m_levelSettings->m_mirrorMode = g_originalMirrorState;
        }
        PlayLayer::onQuit();
    }
};

class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto gearSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        gearSprite->setScale(0.7f);

        auto gearBtn = CCMenuItemSpriteExtra::create(
            gearSprite, this, menu_selector(MyPauseLayer::onOpenMirrorMenu)
        );

        auto menu = CCMenu::create();
        menu->addChild(gearBtn);
        menu->setPosition({35, 35});
        menu->setID("le10x-settings-menu"_spr);

        this->addChild(menu);
    }

    void onOpenMirrorMenu(CCObject* sender) {
        if (auto popup = MirrorPopup::create()) {
            popup->show();
        }
    }
};
