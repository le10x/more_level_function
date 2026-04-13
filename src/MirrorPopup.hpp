#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Cambiado Geode::Popup por geode::Popup
class MirrorPopup : public geode::Popup<> {
protected:
    bool setup() override;
    void onMirrorToggle(CCObject* sender);
public:
    static MirrorPopup* create();
};
