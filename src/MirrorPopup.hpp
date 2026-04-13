#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class MirrorPopup : public Geode::Popup<> {
protected:
    bool setup() override;
    void onMirrorToggle(CCObject* sender);
public:
    static MirrorPopup* create();
};
