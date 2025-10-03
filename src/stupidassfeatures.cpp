#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

auto doAnimation = Mod::get()->getSettingValue<bool>("garage-select-anim");
auto noMirror = Mod::get()->getSettingValue<bool>("nomirror");

$on_mod(Loaded){
    listenForSettingChanges("garage-select-anim", [](bool value) {
        doAnimation = value;
    });
    listenForSettingChanges("nomirror", [](bool value) {
        noMirror = value;
    });
}

class $modify(FuckYouMirrorPortal, GJBaseGameLayer) {
    void toggleFlipped(bool p0, bool p1) {
        if (!noMirror) GJBaseGameLayer::toggleFlipped(p0, p1);
    }
};

class $modify(StupidGarageLayer, GJGarageLayer){
    static void onModify(auto& self) {
        (void) self.setHookPriorityBeforePre("GJGarageLayer::onSelect", "hiimjustin000.more_icons");
    }

    void onSelect(CCObject* sender) {
        GJGarageLayer::onSelect(sender);

        if (!doAnimation) return;

        if (m_selectedIconType == IconType::Special) return;

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

        //fmod->playEffect("click_stereo.ogg"_spr);

    }
};

#define GET_SLIDER(sender) geode::cast::typeinfo_cast<SliderThumb*>(sender); if (!slider) return

class $modify(AllowLowVolumeOLHook, OptionsLayer) {
    void musicSliderChanged(cocos2d::CCObject* sender) {
        auto slider = GET_SLIDER(sender);
        auto value = slider->getValue();
        auto* audioEngine = FMODAudioEngine::sharedEngine();
        float originalVolume = audioEngine->getBackgroundMusicVolume();
        audioEngine->setBackgroundMusicVolume(value);

        if (originalVolume <= 0.f && value > 0.f) GameManager::get()->playMenuMusic();
    }

    void sfxSliderChanged(cocos2d::CCObject* sender) {
        auto slider = GET_SLIDER(sender);
        auto value = slider->getValue();
        FMODAudioEngine::sharedEngine()->setEffectsVolume(value);
    }
};

class $modify(AllowLowVolumePLHook, PauseLayer) {
    void musicSliderChanged(cocos2d::CCObject* sender) {
        auto slider = GET_SLIDER(sender);
        auto value = slider->getValue();
        FMODAudioEngine::sharedEngine()->setBackgroundMusicVolume(value);
    }

    // Function is merged with the one in OptionsLayer on Windows
    #if !(defined(GEODE_IS_WINDOWS) && GEODE_COMP_GD_VERSION == 22060)
    void sfxSliderChanged(cocos2d::CCObject* sender) {
        auto slider = GET_SLIDER(sender);
        auto value = slider->getValue();
        FMODAudioEngine::sharedEngine()->setEffectsVolume(value);
    }
    #endif
};
