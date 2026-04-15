#include "MyMenuLayer.hpp"

MyMenuLayer* MyMenuLayer::create() {
    // Definimos el tamaño de la ventana (Ancho, Alto)
    auto ret = new MyMenuLayer();
    if (ret && ret->init(250.f, 150.f)) {
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
