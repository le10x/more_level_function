#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Heredamos de Popup para tener el botón de cerrar automático
class MyMenuLayer : public geode::Popup<> {
protected:
    bool setup() override; // Aquí se configura el contenido
public:
    static MyMenuLayer* create(); // Método para instanciarla
};
