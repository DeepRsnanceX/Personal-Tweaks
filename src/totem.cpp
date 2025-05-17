#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include "helper.hpp"

using namespace geode::prelude;

static const std::set<int> sawblades = {88, 89, 98, 183, 184, 185, 186, 187, 188, 397, 398, 399, 678, 679, 680, 740, 741, 742, 1619, 1620, 1701, 1702, 1703, 1705, 1706, 1707, 1708, 1709, 1710, 1734, 1735, 1736};
bool totemIsCooldowned = false;

void SarahsTweaks::cooldownTotem(float dt) {
    totemIsCooldowned = false;
}

class $modify(TotemPlayLayer, PlayLayer) {
    struct Fields {
        int lastFrame = 67;
        int currentFrame = 0;
        float animTimer = 0.f;
        float frameDuration = 0.025f;
        CCSprite* totemAnimationSprite = CCSprite::createWithSpriteFrameName("mctotem_anim_0.png"_spr);
    };

    void startTotemAnim() {
        auto fields = m_fields.self();
        fields->currentFrame = 1;
        fields->animTimer = 0.f;

        // Set the first frame
        std::string frameName = fmt::format("mctotem_anim_{}.png"_spr, fields->currentFrame);
        auto frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
        if (frame) {
            fields->totemAnimationSprite->setDisplayFrame(frame);
        }

        // Schedule the animation function
        this->schedule(schedule_selector(TotemPlayLayer::playTotemAnim), fields->frameDuration);
    }

    void playTotemAnim(float dt) {
        auto fields = m_fields.self();

        // Advance frame
        fields->currentFrame++;
        if (fields->currentFrame > fields->lastFrame) {
            // Stop animation at last frame
            this->unschedule(schedule_selector(TotemPlayLayer::playTotemAnim));
            return;
        }

        // Update sprite frame
        std::string frameName = fmt::format("mctotem_anim_{}.png"_spr, fields->currentFrame);
        auto frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
        if (frame) {
            fields->totemAnimationSprite->setDisplayFrame(frame);
        }
    }

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto fields = m_fields.self();
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        fields->totemAnimationSprite->setPosition({winSize.width / 2, winSize.height / 2});
        fields->totemAnimationSprite->setID("totem-animation"_spr);
        fields->totemAnimationSprite->setScale(2.5f);

        this->addChild(fields->totemAnimationSprite, 1000);

        return true;
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        PlayLayer::destroyPlayer(player, obj);

        if (obj != m_anticheatSpike && !totemIsCooldowned) {
            if (m_player1->m_isDead) return;

            totemIsCooldowned = true;
            this->scheduleOnce(schedule_selector(SarahsTweaks::cooldownTotem), 0.5f);

            auto fmod = FMODAudioEngine::sharedEngine();

            // call anim here
            TotemPlayLayer::startTotemAnim();
            fmod->playEffect("totemUse.ogg"_spr);
        }
    }
};