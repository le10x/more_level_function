#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(MySearchLayer, LevelSearchLayer) {
    // Definimos el listener usando el tipo Task genérico
    struct Fields {
        EventListener<web::WebTask> m_listener; 
    };

    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;

        auto buttonSprite = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
        
        auto historyBtn = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(MySearchLayer::onHistorySearch)
        );

        auto menu = this->getChildByID("right-side-menu");
        if (menu) {
            menu->addChild(historyBtn);
            menu->updateLayout();
        }

        return true;
    }

    void onHistorySearch(CCObject* sender) {
        auto loading = LoadingCircle::create();
        loading->show();

        std::string apiUrl = "https://geometrydash.eu";

        // Usamos m_fields->m_listener para capturar la tarea
        m_fields->m_listener.bind([this, loading](web::WebResponse* res) {
            loading->fadeAndRemove();

            if (res && res->ok()) {
                FLAlertLayer::create("GDHistory", "Conexión exitosa", "OK")->show();
            } else {
                FLAlertLayer::create("Error", "No se pudo obtener datos", "OK")->show();
            }
        });

        // Iniciamos la petición
        auto req = web::WebRequest();
        m_fields->m_listener.setFilter(req.get(apiUrl));
    }
};
