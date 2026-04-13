#include "MirrorPopup.hpp"

MirrorPopup* MirrorPopup::create() {
    auto ret = new MirrorPopup();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool MirrorPopup::init() {
    if (!FLAlertLayer::init(150)) return false; // Opacidad del fondo

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    // Crear el fondo café (sprite original de GD)
    auto bg = CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize({ 220, 150 });
    bg->setPosition(winSize / 2);
    m_mainLayer->addChild(bg);

    // Título
    auto title = CCLabelBMFont::create("Level Settings", "goldFont.fnt");
    title->setScale(0.7f);
    title->setPosition(winSize.width / 2, winSize.height / 2 + 55);
    m_mainLayer->addChild(title);

    // Lógica del Mirror
    auto pl = PlayLayer::get();
    bool isMirror = (pl && pl->m_levelSettings) ? pl->m_levelSettings->m_mirrorMode : false;

    auto toggler = CCMenuItemToggler::createWithStandardSprites(
        this, menu_selector(MirrorPopup::onMirrorToggle), 0.8f
    );
    toggler->toggle(isMirror);

    auto label = CCLabelBMFont::create("Mirror Mode", "bigFont.fnt");
    label->setScale(0.5f);

    auto menu = CCMenu::create();
    menu->addChild(toggler);
    menu->addChild(label);
    menu->setLayout(RowLayout::create()->setGap(10.f));
    menu->setPosition({ winSize.width / 2, winSize.height / 2 });
    m_mainLayer->addChild(menu);

    // Botón de cerrar (OK)
    auto okBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("OK"),
        this,
        menu_selector(MirrorPopup::onClose)
    );
    auto okMenu = CCMenu::create();
    okMenu->addChild(okBtn);
    okMenu->setPosition({ winSize.width / 2, winSize.height / 2 - 50 });
    m_mainLayer->addChild(okMenu);

    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);

    return true;
}

void MirrorPopup::show() {
    FLAlertLayer::show();
}

void MirrorPopup::onMirrorToggle(CCObject* sender) {
    if (auto pl = PlayLayer::get()) {
        bool enabled = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
        pl->m_levelSettings->m_mirrorMode = enabled;
    }
}

void MirrorPopup::onClose(CCObject* sender) {
    this->removeFromParentAndCleanup(true);
}
