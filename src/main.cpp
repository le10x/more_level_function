#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// --- LÓGICA DE RESET ---
class $modify(MyPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontSave) {
        if (!PlayLayer::init(level, useReplay, dontSave)) return false;

        // Si Hold Level está activo, saltamos al inicio
        if (Mod::get()->getSettingValue<bool>("hold-level")) {
            this->pushButton(0, true);
        }
        return true;
    }

    void pushButton(int p0, bool p1) {
        bool noClicks = Mod::get()->getSettingValue<bool>("deactivate-clicks");
        bool holdActive = Mod::get()->getSettingValue<bool>("hold-level");

        // Si los clicks están desactivados, ignoramos la entrada...
        // EXCEPTO si Hold Level está activo y queremos que el click NO lo detenga.
        if (noClicks) return;

        // Si hacemos click y Hold Level está activo, se detiene el auto-hold (si no hay No Clicks)
        if (holdActive) {
            Mod::get()->setSettingValue("hold-level", false);
        }

        PlayLayer::pushButton(p0, p1);
    }

    void onQuit() {
        // Al salir del nivel, si Auto Reset está ON, apagamos todo
        if (Mod::get()->getSettingValue<bool>("auto-reset")) {
            Mod::get()->setSettingValue("hold-level", false);
            Mod::get()->setSettingValue("deactivate-clicks", false);
        }
        PlayLayer::onQuit();
    }
};

// --- INTERFAZ DE PAUSA ---
class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto winSize = CCDirector::get()->getWinSize();
        auto side = Mod::get()->getSettingValue<std::string>("button-side");

        // Crear botón de configuración (usando textura de Geode)
        auto btnSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        auto btn = CCMenuItemSpriteExtra::create(
            btnSprite, this, menu_selector(MyPauseLayer::onMySettings)
        );

        auto menu = CCMenu::create();
        if (side == "left") {
            menu->setPosition({35, winSize.height / 2});
        } else {
            menu->setPosition({winSize.width - 35, winSize.height / 2});
        }

        menu->addChild(btn);
        this->addChild(menu);
    }

    void onMySettings(CCObject*) {
        // Aquí abrimos el menú de configuración del mod directamente
        // Es la forma más limpia de mostrar los checks que pediste
        geode::openSettingsPopup(Mod::get());
    }
};
