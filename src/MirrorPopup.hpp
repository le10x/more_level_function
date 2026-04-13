#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class MirrorPopup : public geode::Popup<> {
protected:
    bool setup() override;
    void onMirrorToggle(cocos2d::CCObject* sender);
public:
    static MirrorPopup* create();
};
