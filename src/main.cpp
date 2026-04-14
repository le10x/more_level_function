#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Variables de estado
bool g_noclip = false;
bool g_safeMode = true;
int g_deaths = 0;
CCLabelBMFont* g_deathLabel = nullptr;

// --- Clase del Menú (Usando FLAlertLayer para máxima compatibilidad) ---
class MyModMenu : public FLAlertLayer {
public:
    static MyModMenu* create() {
        auto ret = new MyModMenu();
        if (ret && ret->init(150)) { // 150 es la opacidad del fondo
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init(int opacity) {
        if (!FLAlertLayer::init(opacity)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        // Capa principal (el cuadro marrón de GD)
        auto bg = CCScale9Sprite::create("GJ_square01.png");
        bg->setContentSize({ 220, 150 });
        bg->setPosition(winSize / 2);
        m_mainLayer->addChild(bg);

        auto menu = CCMenu::create();
        m_mainLayer->addChild(menu);

        // Título
        auto title = CCLabelBMFont::create("Mi Mod Menu", "goldFont.fnt");
        title->setPosition({ winSize.width / 2, winSize.height / 2 + 55 });
        title->setScale(0.7f);
        m_mainLayer->addChild(title);

        // Botón cerrar
        auto closeSpr = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
        auto closeBtn = CCMenuItemSpriteExtra::create(
            closeSpr, this, menu_selector(MyModMenu::onClose)
        );
        auto closeMenu = CCMenu::create();
        closeMenu->addChild(closeBtn);
        closeMenu->setPosition({ winSize.width / 2 - 100, winSize.height / 2 + 65 });
        m_mainLayer->addChild(closeMenu);

        // Toggles (Noclip y Safe Mode)
        auto noclipToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MyModMenu::onToggleNoclip), 0.8f
        );
        noclipToggle->toggle(g_noclip);
        noclipToggle->setPosition({ -40, 0 });

        auto safeToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MyModMenu::onToggleSafe), 0.8f
        );
        safeToggle->toggle(g_safeMode);
        safeToggle->setPosition({ 40, 0 });

        menu->addChild(noclipToggle);
        menu->addChild(safeToggle);

        return true;
    }

    void onToggleNoclip(CCObject*) { g_noclip = !g_noclip; }
    void onToggleSafe(CCObject*) { g_safeMode = !g_safeMode; }
    void onClose(CCObject*) { this->removeFromParentAndCleanup(true); }
};

// --- Botón en Pausa ---
class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        auto menu = this->getChildByID("right-button-menu");
        if (!menu) return;

        auto spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        spr->setScale(0.7f);
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(MyPauseLayer::openMyMenu)
        );
        menu->addChild(btn);
        menu->updateLayout();
    }

    void openMyMenu(CCObject*) {
        MyModMenu::create()->show();
    }
};

// --- Noclip Lógica ---
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
