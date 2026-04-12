#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Estado global estático
class ModState {
public:
    static inline bool holdEnabled = false;
    static inline bool ignoreEnabled = false;
};

// --- MENÚ DE FUNCIONES (FLAlertLayer) ---
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

        // Toggles con la posición corregida (texto cerca del check)
        addToggle(menu, "Hold Level", 1, { -70, 20 }, ModState::holdEnabled);
        addToggle(menu, "Ignore Clicks", 2, { -70, -20 }, ModState::ignoreEnabled);

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
        if (sender->getTag() == 1) ModState::holdEnabled = state;
        else ModState::ignoreEnabled = state;
    }

    void onClose(CCObject*) {
        this->setKeypadEnabled(false);
        this->removeFromParentAndCleanup(true);
    }
};

// --- LÓGICA DE JUEGO (Interceptación directa) ---
class $modify(MyPlayLayer, PlayLayer) {
    
    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        if (ModState::holdEnabled && !ModState::ignoreEnabled && m_player1) {
            // Forzamos el botón de salto directamente en el PlayerObject
            // Esto suele saltarse la capa de entrada del CBF
            m_player1->pushButton(PlayerButton::Jump);
        }
    }

    void handleButton(bool hold, int button, bool isPlayer2) {
        // Si Ignore Clicks está activo, no dejamos pasar la señal a RobTop
        if (ModState::ignoreEnabled) return;

        // Si el jugador presiona manualmente, desactivamos el auto-hold
        if (hold && ModState::holdEnabled && button == static_cast<int>(PlayerButton::Jump)) {
            ModState::holdEnabled = false;
            if (m_player1) m_player1->releaseButton(PlayerButton::Jump);
        }

        PlayLayer::handleButton(hold, button, isPlayer2);
    }

    void onExit() {
        if (Mod::get()->getSettingValue<bool>("auto-reset")) {
            ModState::holdEnabled = false;
            ModState::ignoreEnabled = false;
        }
        PlayLayer::onExit();
    }
};

// --- BOTÓN DE PAUSA ---
class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        auto winSize = CCDirector::get()->getWinSize();
        auto side = Mod::get()->getSettingValue<std::string>("button-side");

        auto btnSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        btnSprite->setScale(0.65f); 

        auto btn = CCMenuItemSpriteExtra::create(btnSprite, this, menu_selector(MyPauseLayer::onMySettings));
        auto menu = CCMenu::create();
        
        float posX = (side == "left") ? 40.f : winSize.width - 40.f;
        menu->setPosition({posX, winSize.height / 2});
        menu->addChild(btn);
        this->addChild(menu);
    }

    void onMySettings(CCObject*) {
        auto popup = MySettingsLayer::create();
        CCDirector::get()->getRunningScene()->addChild(popup, 100);
    }
};
