#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "MyMenuLayer.hpp"

using namespace geode::prelude;

class $modify(MenuLayer) {
    void onMyButton(CCObject*) {
        // Creamos y mostramos la ventana
        MyMenuLayer::create()->show();
    }

    bool init() {
        if (!MenuLayer::init()) return false;

        // Añadimos un botón al menú principal para abrir nuestra ventana
        auto menu = this->getChildByID("side-menu");
        auto btn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png"),
            this,
            menu_selector(MenuLayer::onMyButton)
        );
        menu->addChild(btn);
        menu->updateLayout();

        return true;
    }
};
