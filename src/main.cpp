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
class MyModMenu : public geode::Popup<> {
protected:
    bool setup() override {
        this->setTitle("Mi Mod Menu");

        auto menu = CCMenu::create();
        menu->setLayout(ColumnLayout::create()->setGap(10.f));
        m_mainLayer->addChild(menu);

        // Noclip Toggle
        // Usamos static_cast<cocos2d::CCObject*> para evitar el error de rvalue
        auto noclipBtn = CCMenuItemToggler::createWithStandardSprites(
            static_cast<cocos2d::CCObject*>(this), 
            menu_selector(MyModMenu::onNoclip), 
            0.8f
        );
        noclipBtn->toggle(g_noclip);

        // Safe Mode Toggle
        auto safeBtn = CCMenuItemToggler::createWithStandardSprites(
            static_cast<cocos2d::CCObject*>(this), 
            menu_selector(MyModMenu::onSafeMode), 
            0.8f
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
        // Cambiamos initAnchored por el init estándar de Geode si el template falla
        if (ret && ret->init(220, 150)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void onNoclip(CCObject* sender) { g_noclip = !g_noclip; }
    void onSafeMode(CCObject* sender) { g_safeMode = !g_safeMode; }
};

// --- Hook de Pausa ---
class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto menu = CCMenu::create();
        auto winSize = CCDirector::get()->getWinSize();
        menu->setPosition({40, winSize.height - 40});
        menu->setID("my-quick-menu");
        this->addChild(menu);

        auto spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        spr->setScale(0.7f);
        
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(MyPauseLayer::onMyMenu)
        );
        menu->addChild(btn);
    }

    void onMyMenu(CCObject* sender) {
        auto popup = MyModMenu::create();
        if (popup) {
            popup->show();
        }
    }
};

// --- Lógica de Juego (PlayLayer) ---
class $modify(MyPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* p, GameObject* obj) {
        if (g_noclip) {
            g_deaths++;
            if (g_deathLabel) {
                g_deathLabel->setString(fmt::format("Muertes: {}", g_deaths).c_str());
            }
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
        g_deathLabel->setZOrder(100);
        this->addChild(g_deathLabel);
        
        return true;
    }
};
