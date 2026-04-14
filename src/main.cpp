#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Variables globales
bool g_noclip = false;
bool g_safeMode = true;
int g_deaths = 0;
CCLabelBMFont* g_deathLabel = nullptr;

// --- Popup del Menú ---
// IMPORTANTE: Heredar de Popup<> permite usar initAnchored, autorelease y show
class MyModMenu : public geode::Popup<> {
protected:
    bool setup() override {
        this->setTitle("Mi Mod Menu");

        auto menu = CCMenu::create();
        menu->setLayout(ColumnLayout::create()->setGap(10.f));
        m_mainLayer->addChild(menu);

        // Noclip Toggle
        auto noclipBtn = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MyModMenu::onNoclip), 0.8f
        );
        noclipBtn->toggle(g_noclip);

        // Safe Mode Toggle
        auto safeBtn = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MyModMenu::onSafeMode), 0.8f
        );
        safeBtn->toggle(g_safeMode);

        menu->addChild(noclipBtn);
        menu->addChild(safeBtn);
        
        menu->updateLayout();
        return true;
    }

public:
    static MyModMenu* create() {
        auto ret = new MyModMenu();
        // initAnchored es parte de geode::Popup
        if (ret && ret->initAnchored(220, 150)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void onNoclip(CCObject*) { g_noclip = !g_noclip; }
    void onSafeMode(CCObject*) { g_safeMode = !g_safeMode; }
};

// --- Hook de Pausa ---
class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto menu = CCMenu::create();
        auto winSize = CCDirector::get()->getWinSize();
        menu->setPosition({40, winSize.height - 40});
        this->addChild(menu);

        auto spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        spr->setScale(0.7f);
        
        // CORRECCIÓN: Usar MyPauseLayer en lugar de PauseLayer en el selector
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(MyPauseLayer::onMyMenu)
        );
        menu->addChild(btn);
    }

    void onMyMenu(CCObject*) {
        MyModMenu::create()->show();
    }
};

// --- Lógica de Juego ---
class $modify(MyPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* p, GameObject* obj) {
        if (g_noclip) {
            g_deaths++;
            if (g_deathLabel) g_deathLabel->setString(fmt::format("Muertes: {}", g_deaths).c_str());
            return;
        }
        PlayLayer::destroyPlayer(p, obj);
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
