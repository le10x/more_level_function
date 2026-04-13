#pragma once
#include <Geode/Geode.hpp>

class MirrorPopup : public geode::Popup<std::string const&> {
protected:
    bool setup(std::string const& value) override;
    void onMirrorToggle(cocos2d::CCObject* sender);

public:
    // Cambiamos el create para que coincida con el nuevo setup
    static MirrorPopup* create();
};
