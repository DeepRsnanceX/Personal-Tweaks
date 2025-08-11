#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

bool enableBallBug = Mod::get()->getSettingValue<bool>("ballrot-bug");

$on_mod(Loaded) {
    listenForSettingChanges("ballrot-bug", [](bool value) {
        enableBallBug = value;
    });
}

class $modify (PlayerObject)
{
    void switchedToMode(GameObjectType p0) {
        auto ball = m_isBall;

        PlayerObject::switchedToMode(p0);

        if (!enableBallBug) return;

        if (ball && !m_isBall)
        {
            this->runAction(CCEaseOut::create(CCRotateBy::create(0.5f, 360 - this->getRotation()), 2));
        }
    }
};
