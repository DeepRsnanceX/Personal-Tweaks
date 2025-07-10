#include <Geode/modify/GJGarageLayer.hpp>

using namespace geode::prelude;

auto doAnimation = Mod::get()->getSettingValue<bool>("garage-select-anim");

$on_mod(Loaded){
    listenForSettingChanges("garage-select-anim", [](bool value) {
        doAnimation = value;
    });
}

class $modify(StupidGarageLayer, GJGarageLayer){
    static void onModify(auto& self) {
        (void) self.setHookPriorityBeforePre("GJGarageLayer::onSelect", "hiimjustin000.more_icons");
    }

    void onSelect(CCObject* sender) {
        GJGarageLayer::onSelect(sender);

        if (!doAnimation) return;

        float defScale = 1.6f;
        float jetScale = 1.5f;

        auto fmod = FMODAudioEngine::sharedEngine();
        auto fakeAssGlow = this->getChildByID("saritahhh.icon_preview/fake-glow-display");

        auto bounceDef1 = CCSequence::create(CCScaleTo::create(0.f, defScale + 0.100f), CCEaseExponentialOut::create(CCScaleTo::create(0.2f, defScale)), nullptr);
        auto bounceDef2 = CCSequence::create(CCScaleTo::create(0.f, defScale + 0.100f), CCEaseExponentialOut::create(CCScaleTo::create(0.2f, defScale)), nullptr);
        auto bounceJet1 = CCSequence::create(CCScaleTo::create(0.f, jetScale + 0.100f), CCEaseExponentialOut::create(CCScaleTo::create(0.2f, jetScale)), nullptr);
        auto bounceJet2 = CCSequence::create(CCScaleTo::create(0.f, jetScale + 0.100f), CCEaseExponentialOut::create(CCScaleTo::create(0.2f, jetScale)), nullptr);

        m_playerObject->stopAllActions();
        fakeAssGlow->stopAllActions();
        
        if (m_selectedIconType == IconType::Jetpack) {
            m_playerObject->runAction(bounceJet1);
            fakeAssGlow->runAction(bounceJet2);
        } else {
            m_playerObject->runAction(bounceDef1);
            fakeAssGlow->runAction(bounceDef2);
        }

        fmod->playEffect("click_stereo.ogg"_spr);

    }
};