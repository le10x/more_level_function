#include "MyMenuLayer.hpp"

MyMenuLayer* MyMenuLayer::create(float width, float height) {
    auto ret = new MyMenuLayer();
    // Pasamos el ancho y alto al init
    if (ret && ret->init(width, height)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool MyMenuLayer::setup(float p0, float p1) {
    // Ahora setTitle y m_mainLayer funcionarán porque heredamos correctamente
    this->setTitle("Mi Mod Menu");
    
    auto label = CCLabelBMFont::create("Hola!", "bigFont.fnt");
    // m_mainLayer es la capa principal del Popup
    label->setPosition(m_mainLayer->getContentSize() / 2);
    m_mainLayer->addChild(label);

    return true;
}
