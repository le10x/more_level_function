#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Variables de estado
bool g_noclip = false;
bool g_safeMode = true;
int g_deaths = 0;
CCLabelBMFont* g_deathLabel = nullptr;

// --- Lógica de Vuelo y Safe Mode ---
class $modify(MyPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* p, GameObject* obj) {
        if (g_noclip) {
            g_deaths++;
            if (g_deathLabel) g_deathLabel->setString(fmt::format("Muertes: {}", g_deaths).c_str());
            return;
        }
        PlayLayer::destroyPlayer(p, obj);
    }

    void levelComplete() {
        // Si el Safe Mode está activo y usaste noclip, no guardamos el progreso
        if (g_safeMode && g_noclip) {
            Notification::create("Nivel completado (Safe Mode)", NotificationIcon::Info)->show();
            // Salimos al menú sin guardar enviando al jugador de vuelta
            this->onQuit(nullptr); 
            return;
        }
        PlayLayer::levelComplete();
    }

    bool init(GJGameLevel* level, bool useReplay, bool dontSave) {
        if (!PlayLayer::init(level, useReplay, dontSave)) return false;
        g_deaths = 0;
        g_deathLabel = CCLabelBMFont::create("Muertes: 0", "bigFont.fnt");
        g_deathLabel->setScale(0.4f);
        g_deathLabel->setPosition({40, 20});
        this->addChild(g_deathLabel, 100);
        return true;
    }
};

// --- Interfaz del Menú ---
class MyModMenu : public Geode::Popup<> {
protected:
    bool setup() override {
        this->setTitle("Mi Mod Menu");

        auto menu = CCMenu::create();
        menu->setLayout(ColumnLayout::create()->setGap(10.f));
        this->m_mainLayer->addChild(menu);

        // Toggle Noclip
        auto noclipToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MyModMenu::onNoclip), 0.8f
        );
        noclipToggle->toggle(g_noclip);

        // Toggle Safe Mode
        auto safeToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MyModMenu::onSafeMode), 0.8f
        );
        safeToggle->toggle(g_safeMode);

        // Etiquetas
        auto noclipLabel = CCLabelBMFont::create("Noclip", "bigFont.fnt");
        noclipLabel->setScale(0.5f);
        
        auto safeLabel = CCLabelBMFont::create("Safe Mode", "bigFont.fnt");
        safeLabel->setScale(0.5f);

        // Organizar en filas
        auto noclipRow = CCMenu::create();
        noclipRow->setLayout(RowLayout::create()->setGap(20.f));
        noclipRow->addChild(noclipLabel);
        noclipRow->addChild(noclipToggle);
        noclipRow->updateLayout();

        auto safeRow = CCMenu::create();
        safeRow->setLayout(RowLayout::create()->setGap(20.f));
        safeRow->addChild(safeLabel);
        safeRow->addChild(safeToggle);
        safeRow->updateLayout();

        menu->addChild(noclipRow);
        menu->addChild(safeRow);
        menu->updateLayout();

        return true;
    }

public:
    static MyModMenu* create() {
        auto ret = new MyModMenu();
        if (ret && ret->initAnchored(220, 150)) { // Tamaño de la ventana
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void onNoclip(CCObject*) { g_noclip = !g_noclip; }
    void onSafeMode(CCObject*) { g_safeMode = !g_safeMode; }
};

// --- Botón en el Menú de Pausa (Arriba Izquierda) ---
class $modify(PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto menu = CCMenu::create();
        // Posicionarlo arriba a la izquierda
        auto winSize = CCDirector::get()->getWinSize();
        menu->setPosition({40, winSize.height - 40});
        this->addChild(menu);

        auto spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        spr->setScale(0.7f);
        
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(PauseLayer::onMyMenu)
        );
        menu->addChild(btn);
    }

    void onMyMenu(CCObject*) {
        MyModMenu::create()->show();
    }
};
