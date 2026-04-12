#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Variables globales para el estado
bool g_holdLevel = false;
bool g_ignoreClicks = false;

// --- MENÚ PERSONALIZADO ---
class MySettingsLayer : public FLAlertLayer {
public:
    static MySettingsLayer* create() {
        auto ret = new MySettingsLayer();
        if (ret && ret->init(150)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init(int opacity) {
        if (!FLAlertLayer::init(opacity)) return false;

        auto winSize = CCDirector::get()->getWinSize();
        auto bg = CCScale9Sprite::create("GJ_square01.png");
        bg->setContentSize({220, 150});
        bg->setPosition(winSize / 2);
        m_mainLayer->addChild(bg);

        auto menu = CCMenu::create();
        m_mainLayer->addChild(menu);

        auto title = CCLabelBMFont::create("Level Functions", "goldFont.fnt");
        title->setPosition({winSize.width / 2, winSize.height / 2 + 55});
        title->setScale(0.7f);
        m_mainLayer->addChild(title);

        auto closeBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            this, menu_selector(MySettingsLayer::onClose)
        );
        closeBtn->setPosition({-100, 65});
        menu->addChild(closeBtn);

        addToggle(menu, "Hold Level", 1, { -70, 20 }, g_holdLevel);
        addToggle(menu, "Ignore Clicks", 2, { -70, -20 }, g_ignoreClicks);

        return true;
    }

    void addToggle(CCMenu* menu, const char* text, int tag, CCPoint pos, bool enabled) {
        auto toggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(MySettingsLayer::onToggle), 0.7f);
        toggle->setPosition(pos);
        toggle->toggle(enabled);
        toggle->setTag(tag);
        menu->addChild(toggle);

        auto label = CCLabelBMFont::create(text, "bigFont.fnt");
        label->setScale(0.4f);
        label->setAnchorPoint({0, 0.5});
        label->setPosition({pos.x + 25, pos.y});
        menu->addChild(label);
    }

    void onToggle(CCObject* sender) {
        bool state = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
        if (sender->getTag() == 1) g_holdLevel = state;
        else g_ignoreClicks = state;
    }

    void onClose(CCObject*) {
        this->setKeypadEnabled(false);
        this->removeFromParentAndCleanup(true);
    }
};

// --- LÓGICA DE INTERCEPTACIÓN (AQUÍ ESTÁ EL TRUCO) ---
class $modify(MyPlayLayer, PlayLayer) {
    
    // Hook al motor de actualización para inyectar el Hold
    void update(float dt) {
        PlayLayer::update(dt);

        // Si Hold Level está activo, forzamos el salto en el objeto del jugador
        if (g_holdLevel && !g_ignoreClicks && m_player1) {
            // Usamos la función interna que maneja el estado de presión
            if (!m_player1->m_isHolding) {
                this->handleButton(true, static_cast<int>(PlayerButton::Jump), false);
            }
        }
    }

    // Hook a la entrada de botones para el Ignore Clicks
    void handleButton(bool hold, int button, bool isPlayer2) {
        // 1. Si Ignore Clicks está ON, bloqueamos todo
        if (g_ignoreClicks) return;

        // 2. Si el usuario presiona el salto manualmente y el Hold está activo, lo desactivamos
        if (hold && g_holdLevel && button == static_cast<int>(PlayerButton::Jump)) {
            g_holdLevel = false;
            // No retornamos aquí para que el click del usuario SÍ se procese
        }

        PlayLayer::handleButton(hold, button, isPlayer2);
    }

    void onExit() {
        if (Mod::get()->getSettingValue<bool>("auto-reset")) {
            g_holdLevel = false;
            g_ignoreClicks = false;
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
        btnSprite->setScale(0.6f); 
        auto btn = CCMenuItemSpriteExtra::create(btnSprite, this, menu_selector(MyPauseLayer::onMySettings));

        auto menu = CCMenu::create();
        float posX = (side == "left") ? 45.f : winSize.width - 45.f;
        menu->setPosition({posX, winSize.height / 2});
        menu->addChild(btn);
        this->addChild(menu);
    }

    void onMySettings(CCObject*) {
        auto popup = MySettingsLayer::create();
        CCDirector::get()->getRunningScene()->addChild(popup, 100);
    }
};
