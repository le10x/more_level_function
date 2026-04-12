#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// --- MENÚ AZUL (FLAlertLayer) ---
class MySettingsLayer : public FLAlertLayer {
public:
    static MySettingsLayer* create() {
        auto ret = new MySettingsLayer();
        if (ret && ret->init(150)) { // 150 es la opacidad del fondo
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init(int opacity) {
        if (!FLAlertLayer::init(opacity)) return false;

        auto winSize = CCDirector::get()->getWinSize();
        
        // Fondo azul
        auto bg = CCScale9Sprite::create("GJ_square01.png");
        bg->setContentSize({220, 150});
        bg->setPosition(winSize / 2);
        m_mainLayer->addChild(bg);

        auto menu = CCMenu::create();
        m_mainLayer->addChild(menu);

        // Título
        auto title = CCLabelBMFont::create("Level Functions", "goldFont.fnt");
        title->setPosition({winSize.width / 2, winSize.height / 2 + 55});
        title->setScale(0.7f);
        m_mainLayer->addChild(title);

        // Botón de cerrar
        auto closeSprite = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
        auto closeBtn = CCMenuItemSpriteExtra::create(closeSprite, this, menu_selector(MySettingsLayer::onClose));
        closeBtn->setPosition({-100, 65});
        menu->addChild(closeBtn);

        // --- TOGGLES ---
        addToggle(menu, "Hold Level", "hold-level", {0, 20});
        addToggle(menu, "Ignore Clicks", "ignore-clicks", {0, -20});

        return true;
    }

    void addToggle(CCMenu* menu, const char* text, const char* settingKey, CCPoint pos) {
        auto label = CCLabelBMFont::create(text, "bigFont.fnt");
        label->setScale(0.4f);
        label->setPosition({pos.x + 20, pos.y});
        label->setAnchorPoint({0, 0.5});
        menu->addChild(label);

        auto toggle = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(MySettingsLayer::onToggle), 0.7f);
        toggle->setPosition({pos.x - 40, pos.y});
        toggle->setTag(settingKey[0]); // Usamos la primera letra como ID rápido
        toggle->toggle(Mod::get()->getSettingValue<bool>(settingKey));
        menu->addChild(toggle);
    }

    void onToggle(CCObject* sender) {
        auto toggle = static_cast<CCMenuItemToggler*>(sender);
        bool newState = !toggle->isToggled();
        const char* key = (toggle->getTag() == 'h') ? "hold-level" : "ignore-clicks";
        
        Mod::get()->setSettingValue(key, newState);

        if (auto pl = PlayLayer::get()) {
            if (toggle->getTag() == 'h') {
                if (newState) pl->m_player1->pushButton(PlayerButton::Jump);
                else pl->m_player1->releaseButton(PlayerButton::Jump);
            }
        }
    }

    void onClose(CCObject*) {
        this->setKeypadEnabled(false);
        this->removeFromParentAndCleanup(true);
    }
};

// --- RESTO DEL CÓDIGO (PlayLayer y PauseLayer) ---
class $modify(MyPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontSave) {
        if (!PlayLayer::init(level, useReplay, dontSave)) return false;
        if (Mod::get()->getSettingValue<bool>("hold-level")) m_player1->pushButton(PlayerButton::Jump);
        return true;
    }

    void handleButton(bool hold, int button, bool isPlayer2) {
        if (Mod::get()->getSettingValue<bool>("ignore-clicks")) return;
        if (hold && Mod::get()->getSettingValue<bool>("hold-level")) Mod::get()->setSettingValue("hold-level", false);
        PlayLayer::handleButton(hold, button, isPlayer2);
    }

    void onExit() {
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
        btnSprite->setScale(0.5f);
        auto btn = CCMenuItemSpriteExtra::create(btnSprite, this, menu_selector(MyPauseLayer::onMySettings));

        auto menu = CCMenu::create();
        float posX = (side == "left") ? 25.f : winSize.width - 25.f;
        menu->setPosition({posX, winSize.height / 2});
        menu->addChild(btn);
        this->addChild(menu);
    }

    void onMySettings(CCObject*) {
        auto popup = MySettingsLayer::create();
        CCDirector::get()->getRunningScene()->addChild(popup, 100);
    }
};
