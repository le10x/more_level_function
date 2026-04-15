#include "MyMenuLayer.hpp"

MyMenuLayer* MyMenuLayer::create() {
    auto ret = new MyMenuLayer();
    // 240.f es el ancho, 160.f es el alto
    if (ret && ret->init(240.f, 160.f)) { 
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool MyMenuLayer::setup() {
    // El botón de cerrar (X) se crea SOLO gracias a geode::Popup
    this->setTitle("Mi Mod Menu");
    
    // El color de fondo suele ser el marrón oscuro de GD por defecto.
    // Si quieres añadir un texto simple:
    auto label = CCLabelBMFont::create("Hola!", "bigFont.fnt");
    label->setPosition(m_mainLayer->getContentSize() / 2);
    m_mainLayer->addChild(label);

    return true;
}
