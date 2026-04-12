#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class $modify(MySearchLayer, LevelSearchLayer) {
    // Definimos variables que viven dentro del Hook
    struct Fields {
        EventListener<web::WebTask> m_listener;
    };

    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;

        // 1. Crear el sprite del botón (usamos el icono de refresh/update)
        auto buttonSprite = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
        
        // 2. Crear el botón y asignarle la función
        auto historyBtn = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(MySearchLayer::onHistorySearch)
        );

        // 3. Posicionar el botón en el menú lateral derecho
        auto menu = this->getChildByID("right-side-menu");
        if (menu) {
            menu->addChild(historyBtn);
            menu->updateLayout();
        }

        return true;
    }

    void onHistorySearch(CCObject* sender) {
        // Crear un círculo de carga visual
        auto loading = LoadingCircle::create();
        loading->show();

        // URL de la API (asegúrate de que el endpoint sea el correcto de GDHistory)
        std::string apiUrl = "https://geometrydash.eu";

        // Configurar el Listener para manejar la respuesta
        m_fields->m_listener.bind([this, loading](web::WebResponse* res) {
            // Quitar el círculo de carga al recibir respuesta
            loading->fadeAndRemove();

            if (res && res->ok()) {
                // Si la petición fue exitosa (reemplaza a isSuccess)
                auto data = res->json().unwrapOr(matjson::Value());
                
                FLAlertLayer::create(
                    "GDHistory", 
                    "¡Conectado con éxito! Aquí cargarías los niveles.", 
                    "OK"
                )->show();
                
            } else {
                // Si hubo un error en la red o la API
                FLAlertLayer::create(
                    "Error", 
                    "No se pudo obtener datos de GDHistory.", 
                    "Aceptar"
                )->show();
            }
        });

        // Ejecutar la petición Web
        m_fields->m_listener.setFilter(web::WebRequest().get(apiUrl));
    }
};
