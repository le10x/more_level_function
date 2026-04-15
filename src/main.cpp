#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "MyMenuLayer.hpp"

using namespace geode::prelude;

class $modify(MyMenuLayerHook, MenuLayer) {
    void onMyButton(CCObject* sender) {
        auto popup = MyMenuLayer::create();
        if (popup) {
            popup->show();
        }
    }

    bool init() {
        if (!MenuLayer::init()) return false;
        
        auto menu = this->getChildByID("side-menu");
        if (menu) {
            auto btn = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png"),
                this,
                menu_selector(MyMenuLayerHook::onMyButton) // Usar el nombre de la clase del hook
            );
            menu->addChild(btn);
            menu->updateLayout();
        }
        return true;
    }
};
