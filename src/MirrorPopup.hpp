#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class MirrorPopup : public FLAlertLayer {
protected:
    bool init() override;
    void onMirrorToggle(CCObject* sender);
    void onClose(CCObject* sender);

public:
    static MirrorPopup* create();
    void show();
};
