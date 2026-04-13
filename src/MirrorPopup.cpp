#include "MirrorPopup.hpp"

MirrorPopup* MirrorPopup::create() {
    auto ret = new MirrorPopup();
    if (ret && ret->initAnchored(220.f, 150.f)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool MirrorPopup::setup() {
    this->setTitle("Level Settings");

    auto pl = PlayLayer::get();
    // Verificamos que PlayLayer y levelSettings existan para evitar crasheos
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
    
    m_mainLayer->addChild(menu);
    
    return true;
}

void MirrorPopup::onMirrorToggle(CCObject* sender) {
    auto pl = PlayLayer::get();
    if (pl && pl->m_levelSettings) {
        bool enabled = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
        pl->m_levelSettings->m_mirrorMode = enabled;
        // En la 2.2, el cambio visual se aplica al resetear o por triggers.
    }
}
