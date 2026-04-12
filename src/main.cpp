#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Variables globales para persistencia durante la sesión
bool g_holdLevel = false;
bool g_ignoreClicks = false;

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

        // --- TOGGLES (Movidos a la izquierda) ---
        addToggle(menu, "Hold Level", 1, { -60, 20 }, g_holdLevel);
        addToggle(menu, "Ignore Clicks", 2, { -60, -20 }, g_ignoreClicks);

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
        label->setPosition({pos.x + 25, pos.y}); // Texto cerca del check
        menu->addChild(label);
    }

    void onToggle(CCObject* sender) {
        bool state = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
        if (sender->getTag() == 1) {
            g_holdLevel = state;
            if (auto pl = PlayLayer::get()) {
                // Forzamos el estado de salto
                pl->m_player1->pushButton(PlayerButton::Jump);
                if (!state) pl->m_player1->releaseButton(PlayerButton::Jump);
            }
        } else {
            g_ignoreClicks = state;
        }
    }

    void onClose(CCObject*) {
        this->setKeypadEnabled(false);
        this->removeFromParentAndCleanup(true);
    }
};

class $modify(MyPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontSave) {
        if (!PlayLayer::init(level, useReplay, dontSave)) return false;
        if (g_holdLevel) this->m_player1->pushButton(PlayerButton::Jump);
        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        // Mantiene el botón presionado cada frame si Hold está activo
        if (g_holdLevel && !g_ignoreClicks) {
            this->m_player1->pushButton(PlayerButton::Jump);
        }
    }

    void handleButton(bool hold, int button, bool isPlayer2) {
        if (g_ignoreClicks) return;
        // Si el usuario presiona manualmente, desactivamos el auto-hold
        if (hold && g_holdLevel) g_holdLevel = false;
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
        btnSprite->setScale(0.65f); // Un poco más grande (era 0.5)

        auto btn = CCMenuItemSpriteExtra::create(btnSprite, this, menu_selector(MyPauseLayer::onMySettings));
        auto menu = CCMenu::create();
        
        float posX = (side == "left") ? 40.f : winSize.width - 40.f; // Más centrado hacia el medio
        menu->setPosition({posX, winSize.height / 2});
        menu->addChild(btn);
        this->addChild(menu);
    }

    void onMySettings(CCObject*) {
        auto popup = MySettingsLayer::create();
        CCDirector::get()->getRunningScene()->addChild(popup, 100);
    }
};
