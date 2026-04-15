#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class MyMenuLayer : public geode::Popup<float, float> {
protected:
    bool setup(float p0, float p1) override; 
public:
    static MyMenuLayer* create(float width, float height);
};
