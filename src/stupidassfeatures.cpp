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

class $modify(StupidPlayerObject, PlayerObject) {
    struct Fields {
        CCSprite* p_soulSpr = nullptr;
        CCSprite* p_grazeSpr = nullptr;
    };

    bool init(int player, int ship, GJBaseGameLayer* gameLayer, CCLayer* layer, bool playLayer) {
        if (!PlayerObject::init(player, ship, gameLayer, layer, playLayer)) return false;
        
        if (!Mod::get()->getSettingValue<bool>("enable-soul")) return true;
        auto fields = m_fields.self();

        fields->p_soulSpr = CCSprite::createWithSpriteFrameName("soulBase.png"_spr);
        fields->p_soulSpr->setScale(1.5f);
        fields->p_soulSpr->setID("soul-sprite"_spr);
        this->addChild(fields->p_soulSpr);

        m_mainLayer->setVisible(false);
        m_robotSprite->setVisible(false);
        m_spiderSprite->setVisible(false);

        if (Mod::get()->getSettingValue<bool>("enable-soul")) {
            fields->p_grazeSpr = CCSprite::createWithSpriteFrameName("soulGraze.png"_spr);
            fields->p_grazeSpr->setID("soul-graze-sprite"_spr);
            fields->p_grazeSpr->setOpacity(0);

            fields->p_soulSpr->addChild(fields->p_grazeSpr);

            fields->p_grazeSpr->setPosition({fields->p_soulSpr->getContentSize().width / 2.f, fields->p_soulSpr->getContentSize().height / 2.f});
        }

        return true;
    }

    ccColor3B getColorToUse() {
        ccColor3B ret;

        if (m_isShip) {
            ret = cocos2d::ccColor3B({ 255, 0, 255 });
        } else if (m_isBall) {
            ret = cocos2d::ccColor3B({255, 0, 0});
        } else if (m_isBird) {
            ret = cocos2d::ccColor3B({ 255, 145, 0 });
        } else if (m_isDart) {
            ret = cocos2d::ccColor3B({ 0, 255, 255 });
        } else if (m_isRobot) {
            ret = cocos2d::ccColor3B({255, 255, 255});
        } else if (m_isSpider) {
            ret = cocos2d::ccColor3B({ 140, 0, 255 });
        } else {
            ret = cocos2d::ccColor3B({0, 255, 0});
        }

        return ret;
    }

    void update(float p0) {
        PlayerObject::update(p0);

        if (!Mod::get()->getSettingValue<bool>("enable-soul")) return;
        auto fields = m_fields.self();
        if (!fields->p_soulSpr) return;

        float targetScale;

        if (m_isDart) {
            targetScale = 1.0f;
        } else if (m_isSpider) {
            targetScale = 1.5f;
        } else {
            targetScale = 1.6f;
        }

        this->setRotation(0);
        fields->p_soulSpr->setColor(getColorToUse());
        fields->p_soulSpr->setScale(targetScale);
        fields->p_soulSpr->setFlipY(m_isUpsideDown);

        if (m_ghostTrail) m_ghostTrail->m_iconSprite = fields->p_soulSpr;
    }
};

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
