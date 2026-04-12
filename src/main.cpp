#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

// --- CLASE PARA EL MENÚ AZUL PERSONALIZADO ---
class MySettingsPopup : public GeodePopup<CCObject*> {
protected:
    bool setup(CCObject* obj) override {
        this->setTitle("Level Functions");

        auto menu = CCMenu::create();
        menu->setLayout(ColumnLayout::create()->setGap(10.f));
        m_mainLayer->addChild(menu);

        // Toggle: Hold Level
        auto holdToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MySettingsPopup::onToggle), 0.8f
        );
        holdToggle->setTag(1);
        holdToggle->toggle(Mod::get()->getSettingValue<bool>("hold-level"));

        auto holdLabel = CCLabelBMFont::create("Hold Level", "bigFont.fnt");
        holdLabel->setScale(0.5f);

        // Toggle: Ignore Clicks
        auto ignoreToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MySettingsPopup::onToggle), 0.8f
        );
        ignoreToggle->setTag(2);
        ignoreToggle->toggle(Mod::get()->getSettingValue<bool>("ignore-clicks"));

        auto ignoreLabel = CCLabelBMFont::create("Ignore Clicks", "bigFont.fnt");
        ignoreLabel->setScale(0.5f);

        // Añadir al menú con filas sencillas
        auto row1 = CCMenu::create(holdToggle, holdLabel, nullptr);
        row1->setLayout(RowLayout::create()->setGap(10.f));
        auto row2 = CCMenu::create(ignoreToggle, ignoreLabel, nullptr);
        row2->setLayout(RowLayout::create()->setGap(10.f));

        menu->addChild(row1);
        menu->addChild(row2);
        menu->updateLayout();

        return true;
    }

    void onToggle(CCObject* sender) {
        auto toggle = static_cast<CCMenuItemToggler*>(sender);
        bool val = !toggle->isToggled(); // Geode togglers son inversos al hacer click
        
        if (toggle->getTag() == 1) Mod::get()->setSettingValue("hold-level", val);
        else Mod::get()->setSettingValue("ignore-clicks", val);

        // Actualizar el estado físico en el PlayLayer si existe
        if (auto pl = PlayLayer::get()) {
            if (val && toggle->getTag() == 1) {
                pl->m_player1->pushButton(PlayerButton::Jump);
            } else if (!val && toggle->getTag() == 1) {
                pl->m_player1->releaseButton(PlayerButton::Jump);
            }
        }
    }

public:
    static MySettingsPopup* create() {
        auto ret = new MySettingsPopup();
        if (ret && ret->initAnchored(220, 150, nullptr, "GJ_square01.png")) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

// --- LOGICA DEL JUEGO ---
class $modify(MyPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontSave) {
        if (!PlayLayer::init(level, useReplay, dontSave)) return false;
        
        if (Mod::get()->getSettingValue<bool>("hold-level")) {
            this->m_player1->pushButton(PlayerButton::Jump);
        }
        return true;
    }

    void handleButton(bool hold, int button, bool isPlayer2) {
        if (Mod::get()->getSettingValue<bool>("ignore-clicks")) return;

        if (hold && Mod::get()->getSettingValue<bool>("hold-level")) {
            Mod::get()->setSettingValue("hold-level", false);
        }
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

// --- BOTÓN DE PAUSA ---
class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        auto winSize = CCDirector::get()->getWinSize();
        auto side = Mod::get()->getSettingValue<std::string>("button-side");

        auto btnSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        btnSprite->setScale(0.5f); // Tamaño a la mitad

        auto btn = CCMenuItemSpriteExtra::create(
            btnSprite, this, menu_selector(MyPauseLayer::onMySettings)
        );

        auto menu = CCMenu::create();
        float posX = (side == "left") ? 25.f : winSize.width - 25.f;
        menu->setPosition({posX, winSize.height / 2});
        menu->addChild(btn);
        this->addChild(menu);
    }

    void onMySettings(CCObject*) {
        MySettingsPopup::create()->show();
    }
};
