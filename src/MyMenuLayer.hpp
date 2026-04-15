#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class MyMenuLayer : public geode::Popup<> {
protected:
    bool setup() override; 
public:
    static MyMenuLayer* create();
};
