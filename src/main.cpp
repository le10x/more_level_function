#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

// Variable para rastrear el estado del noclip
bool g_noclipEnabled = false;

// 1. HOOK DE LA LÓGICA (Evitar la muerte)
class $modify(MyPlayLayer, PlayLayer) {
    // En la 2.2081, destroyPlayer sigue siendo el punto clave para el noclip
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (g_noclipEnabled) {
            // Si el noclip está activo, simplemente salimos de la función sin matar al jugador
            return;
        }
        // Si no está activo, ejecutamos la lógica original del juego
        PlayLayer::destroyPlayer(player, object);
    }
};

// 2. INTERFAZ (Añadir botón al Menú de Pausa)
class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        // Llamar a la función original primero para que se cree el menú
        PauseLayer::customSetup();

        // Buscamos el menú de la derecha (donde suelen estar los ajustes de práctica)
        auto menu = this->getChildByID("right-button-menu");
        
        if (!menu) return;

        // Creamos un sprite para el botón (puedes usar uno de GD o uno personalizado)
        auto spr = ButtonSprite::create("Noclip", "goldFont.fnt", "GJ_button_01.png", 0.8f);
        
        // Creamos el botón vinculado a nuestra función toggle
        auto btn = CCMenuItemSpriteExtra::create(
            spr,
            this,
            menu_selector(MyPauseLayer::onNoclipToggle)
        );
        
        btn->setID("noclip-button-toggle");
        
        // Añadimos el botón al menú y actualizamos el diseño automático de Geode
        menu->addChild(btn);
        menu->updateLayout();
    }

    // Función que se ejecuta al presionar el botón
    void onNoclipToggle(CCObject* sender) {
        g_noclipEnabled = !g_noclipEnabled;

        // Mostrar una notificación rápida en pantalla
        std::string status = g_noclipEnabled ? "Activado" : "Desactivado";
        Notification::create("Noclip " + status, NotificationIcon::Info)->show();
        
        // Opcional: Cambiar el color del botón para indicar estado
        auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto spr = static_cast<ButtonSprite*>(btn->getNormalImage());
        // (Aquí podrías cambiar texturas o colores si lo deseas)
    }
};
