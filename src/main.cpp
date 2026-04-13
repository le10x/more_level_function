#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "MirrorPopup.hpp"

using namespace geode::prelude;

// Variable estática para recordar el estado original del nivel
static bool g_originalMirrorState = false;

class $modify(MyPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontSave) {
        if (!PlayLayer::init(level, useReplay, dontSave)) return false;
        
        // Guardamos el estado original solo si los ajustes del nivel existen
        if (m_levelSettings) {
            g_originalMirrorState = m_levelSettings->m_mirrorMode;
        }
        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        
        // Si el usuario tiene activo "Reset on Death", devolvemos el Mirror a su estado original
        if (m_levelSettings && Mod::get()->getSettingValue<bool>("mirror-reset-on-death")) {
            m_levelSettings->m_mirrorMode = g_originalMirrorState;
        }
    }

    void onQuit() {
        // Siempre restauramos al salir para no modificar el nivel permanentemente
        if (m_levelSettings) {
            m_levelSettings->m_mirrorMode = g_originalMirrorState;
        }
        PlayLayer::onQuit();
    }
};

class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        // Creamos el sprite del engranaje original de GD
        auto gearSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        gearSprite->setScale(0.7f);

        auto gearBtn = CCMenuItemSpriteExtra::create(
            gearSprite, 
            this, 
            menu_selector(MyPauseLayer::onOpenMirrorMenu)
        );

        // Creamos un menú para contener el botón
        auto menu = CCMenu::create();
        menu->addChild(gearBtn);
        
        // Posición a la izquierda (ajusta estos valores si se encima con otros botones)
        menu->setPosition({35, 35});
        menu->setID("le10x-settings-menu"_spr);

        this->addChild(menu);
    }

    void onOpenMirrorMenu(CCObject* sender) {
        // Llamamos al método create() que ahora maneja el template correctamente
        if (auto popup = MirrorPopup::create()) {
            popup->show();
        }
    }
};
