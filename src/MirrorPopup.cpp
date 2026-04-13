#include "MirrorPopup.hpp"

using namespace geode::prelude;

MirrorPopup* MirrorPopup::create() {
    auto ret = new MirrorPopup();
    // Pasamos un string vacío como argumento del template
    if (ret && ret->initAnchored(220.f, 150.f, "")) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool MirrorPopup::setup(std::string const& value) {
    this->setTitle("Level Settings");

    auto pl = PlayLayer::get();
    bool isMirror = (pl && pl->m_levelSettings) ? pl->m_levelSettings->m_mirrorMode : false;

    auto toggler = CCMenuItemToggler::createWithStandardSprites(
        this, 
        menu_selector(MirrorPopup::onMirrorToggle), 
        0.8f
    );
    toggler->toggle(isMirror);

    auto label = CCLabelBMFont::create("Mirror Mode", "bigFont.fnt");
    label->setScale(0.5f);

    auto menu = CCMenu::create();
    menu->addChild(toggler);
    menu->addChild(label);
    menu->setLayout(RowLayout::create()->setGap(10.f));
    menu->setPosition({110.f, 70.f});
    
    // Si m_mainLayer falla, usamos m_buttonMenu que siempre existe en Popups
    this->m_buttonMenu->getParent()->addChild(menu);
    
    return true;
}

void MirrorPopup::onMirrorToggle(cocos2d::CCObject* sender) {
    auto pl = PlayLayer::get();
    if (pl && pl->m_levelSettings) {
        bool enabled = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
        pl->m_levelSettings->m_mirrorMode = enabled;
    }
}
