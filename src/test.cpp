#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(FuckThis, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        
        log::info("i just inited so hard");

        return true;
    }

    void setupHasCompleted() {
        log::info("i am about to setupHasCompleted everywhere");

        PlayLayer::setupHasCompleted();

        log::info("oouggh fuckk");
    }
};