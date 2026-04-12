#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(MySearchLayer, LevelSearchLayer) {
    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;

        // 1. Crear el sprite del botón (usamos uno de actualización del juego)
        auto buttonSprite = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
        
        // 2. Crear el botón y asignarle una función (onHistorySearch)
        auto historyBtn = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(MySearchLayer::onHistorySearch)
        );

        // 3. Posicionar el botón en el menú de la derecha
        auto menu = this->getChildByID("right-side-menu");
        if (menu) {
            menu->addChild(historyBtn);
            menu->updateLayout();
        }

        return true;
    }

    void onHistorySearch(CCObject* sender) {
        // Mostrar un indicador de carga
        auto loading = LoadingCircle::create();
        loading->show();

        // 4. Petición a la API de GDHistory (Ejemplo de niveles recientes)
        // Nota: Ajustar el endpoint según la documentación actual de GDHistory
        std::string apiUrl = "https://geometrydash.eu";

        web::WebRequest().get(apiUrl).listen(
            [this, loading](web::WebResponse* res) {
                loading->fadeAndRemove();
                
                if (res->isSuccess()) {
                    auto data = res->json().unwrapOr(matjson::Value());
                    // Aquí procesarías los IDs y abrirías LevelBrowserLayer
                    FLAlertLayer::create("Éxito", "Niveles obtenidos de GDHistory", "OK")->show();
                } else {
                    FLAlertLayer::create("Error", "No se pudo conectar con GDHistory", "OK")->show();
                }
            }
        );
    }
};
