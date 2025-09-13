#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <random>
#include "helper.hpp"

using namespace geode::prelude;

float sensitivity = 14.0f;
bool tpCooldown = false;
bool isPlayer2 = false;
static const std::set<int> sawblades = {88, 89, 98, 183, 184, 185, 186, 187, 188, 397, 398, 399, 678, 679, 680, 740, 741, 742, 1619, 1620, 1701, 1702, 1703, 1705, 1706, 1707, 1708, 1709, 1710, 1734, 1735, 1736};

bool enableTPBar = true;

float getRandomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

void SarahsTweaks::cooldownTP(float dt) {
    tpCooldown = false;
}

CCNode* createTpBar() {
    auto barTop = CCSprite::create("tpBar_top.png"_spr);
    auto barBg = CCSprite::create("tpBar_bg.png"_spr);
    auto barFill = CCSprite::create("tpBar_filler.png"_spr);
    auto tpTextSprite = CCSprite::create("tpBar_tp.png"_spr);
    auto tpFillerLine = CCSprite::create("tpBar_line.png"_spr);

    auto tpPercentLabel = CCLabelBMFont::create("0", "deltarune.fnt"_spr);
    auto percentSimbol = CCLabelBMFont::create("%", "deltarune.fnt"_spr);

    barTop->setID("tp-bar-top"_spr);
    barBg->setID("tp-bar-bg"_spr);
    barFill->setID("tp-bar-fill"_spr);
    tpTextSprite->setID("tp-bar-text"_spr);
    tpFillerLine->setID("tp-bar-filler-line"_spr);
    tpPercentLabel->setID("tp-bar-percent-label"_spr);
    percentSimbol->setID("tp-bar-percent-simbol"_spr);

    barTop->setZOrder(4);
    tpFillerLine->setZOrder(3);
    barFill->setZOrder(2);
    barBg->setZOrder(1);

    auto containerNode = CCNode::create();
    containerNode->setContentSize(barTop->getContentSize());
    containerNode->setAnchorPoint({0.5f, 0.5f});

    containerNode->addChild(barBg);
    containerNode->addChild(barFill);
    containerNode->addChild(barTop);
    containerNode->addChild(tpTextSprite);
    containerNode->addChild(tpFillerLine);
    containerNode->addChild(tpPercentLabel);
    containerNode->addChild(percentSimbol);

    auto nodeSize = containerNode->getContentSize();

    barTop->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});
    barBg->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});

    barFill->setAnchorPoint({0.5f, 0.f});
    barFill->setPosition({nodeSize.width / 2.f, 0.f});
    barFill->setScaleY(0.f);

    tpTextSprite->setPosition({(nodeSize.width / 2.f) - 19.f, nodeSize.height - nodeSize.height / 4.f});

    tpPercentLabel->setPosition({tpTextSprite->getPositionX() - 9.f, tpTextSprite->getPositionY() - 18.f});
    tpPercentLabel->setScale(0.7f);
    tpPercentLabel->setAlignment(kCCTextAlignmentLeft);
    tpPercentLabel->setExtraKerning(-6);
    tpPercentLabel->setAnchorPoint({0.f, 0.5f});
    percentSimbol->setPosition({tpTextSprite->getPositionX(), tpPercentLabel->getPositionY() - 13.f});
    percentSimbol->setScale(0.7f);
    percentSimbol->setAnchorPoint({0.7f, 0.5f});

    auto fillSize = barFill->getContentSize();

    tpFillerLine->setAnchorPoint({0.5f, 1.f});
    tpFillerLine->setPosition({nodeSize.width / 2.f, 1.f});

    containerNode->setID("tp-bar-container"_spr);
    containerNode->setScale(1.5f);

    return containerNode;
}

class $modify(TPBaseLayer, GJBaseGameLayer) {

    void doTheWhateverThing() {
        auto playLayer = PlayLayer::get();
        auto fmod = FMODAudioEngine::sharedEngine();

        if (!playLayer) return;

        tpCooldown = true;
        this->scheduleOnce(schedule_selector(SarahsTweaks::cooldownTP), 0.12f);

        auto plHasBar = playLayer->getChildByID("tp-bar-container"_spr);
        if (!plHasBar) return;

        // increase bar
        auto barFill = plHasBar->getChildByID("tp-bar-fill"_spr);
        if (!barFill) return;

        float currentScaleY = barFill->getScaleY();

        if (currentScaleY < 1.f) {
            fmod->playEffect("snd_graze.ogg"_spr);
        }

        float newScaleY = currentScaleY + getRandomFloat(0.01f, 0.09f);
        if (newScaleY > 1.f) newScaleY = 1.f;
        //barFill->setScaleY(newScaleY);
        auto barScaleAction = CCEaseInOut::create(CCScaleTo::create(0.1f, 1.f, newScaleY), 2.0f);
        barFill->runAction(barScaleAction);

        auto barFillLine = plHasBar->getChildByID("tp-bar-filler-line"_spr);
        if (!barFillLine) return;

        auto fillSize = barFill->getContentSize();
        auto moveLineAnim = CCEaseInOut::create(CCMoveTo::create(0.05f, {barFill->getPositionX(), fillSize.height * newScaleY}), 2.0f);
        //barFillLine->stopAllActions();
        barFillLine->runAction(moveLineAnim);
        //barFillLine->setPosition({barFill->getPositionX(), fillSize.height * newScaleY});

        auto theLabel = static_cast<CCLabelBMFont*>(plHasBar->getChildByID("tp-bar-percent-label"_spr));
        if (!theLabel) return;

        float tpAmountFloat = newScaleY * 100.f;
        int tpAmount = static_cast<int>(tpAmountFloat);
        auto tpString = fmt::format("{}", tpAmount);

        if (tpAmount == 100) {
            tpString = "MAX";
        }

        theLabel->setString(tpString.c_str(), true);

        // all player stuff

        PlayerObject* targetPlayer = isPlayer2 ? m_player2 : m_player1;
        if (!targetPlayer) return;
        
        auto tpSprite = static_cast<CCSprite*>(targetPlayer->getChildByID("tp-effect-sprite"_spr));
        if (!tpSprite) return;

        // Update the sprite to match current player icon
        float tpSpriteExtraScale = 0.3f;
        tpSprite->setDisplayFrame(targetPlayer->m_iconSprite->displayFrame());
        tpSprite->setScale(targetPlayer->m_iconSprite->getScale() + tpSpriteExtraScale);
        
        // Stop any existing animation and run the new one
        tpSprite->stopAllActions();
        
        auto actionThing = CCSequence::create(
            CCFadeIn::create(0.f),
            CCEaseOut::create(CCFadeOut::create(0.3f), 2.f),
            nullptr
        );

        tpSprite->runAction(actionThing);

    }

    void collisionCheckObjects(PlayerObject* player, gd::vector<GameObject*>* objs, int myAss, float v1) {
		GJBaseGameLayer::collisionCheckObjects(player, objs, myAss, v1);
		if (!enableTPBar || !PlayLayer::get()) return;
        if (tpCooldown) return;

		for (int i = 0; i < myAss; i++) {
			GameObject* obj = objs->at(i);
			if (!obj || obj->m_isGroupDisabled || obj == m_anticheatSpike) continue;
			if (obj->m_objectType != GameObjectType::Hazard && obj->m_objectType != GameObjectType::AnimatedHazard && obj->m_objectType != GameObjectType::Solid) continue;
			if (obj->m_objectType == GameObjectType::Solid) continue;
			if (obj->m_isHide || obj->getOpacity() < 1) continue;
			const bool isSawblade = std::binary_search(sawblades.begin(), sawblades.end(), obj->m_objectID);
			const float multiplier = isSawblade ? -2.5f : 2.f;
			CCRect sensitivityRect = CCRect(obj->getObjectRect().origin - CCPoint(sensitivity, sensitivity), obj->getObjectRect().size + CCPoint(sensitivity * multiplier, sensitivity * multiplier));
			if (player->getObjectRect().intersectsRect(sensitivityRect)) doTheWhateverThing();
            isPlayer2 = player->m_isSecondPlayer;
		}
	}

};

class $modify(TPPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        auto barNode = createTpBar();
        this->addChild(barNode);

        if (!barNode) return true;
        barNode->setPosition({45.f, winSize.height / 2.f});

        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        auto plHasBar = this->getChildByID("tp-bar-container"_spr);
        if (!plHasBar) return;

        auto barFill = plHasBar->getChildByID("tp-bar-fill"_spr);
        if (!barFill) return;

        barFill->setScaleY(0.f);

        auto barFillLine = plHasBar->getChildByID("tp-bar-filler-line"_spr);
        if (!barFillLine) return;
        auto fillSize = barFill->getContentSize();
        barFillLine->setPosition({barFill->getPositionX(), 1.f});

        auto percentLabel = static_cast<CCLabelBMFont*>(plHasBar->getChildByID("tp-bar-percent-label"_spr));
        if (!percentLabel) return;
        percentLabel->setString("0", true);
    }
};

class $modify(TPPlayerObject, PlayerObject) {
    bool init(int player, int ship, GJBaseGameLayer* gameLayer, CCLayer* layer, bool playLayer) {
        if (!PlayerObject::init(player, ship, gameLayer, layer, playLayer)) return false;
        
        // Create the TP effect sprite
        auto tpSprite = CCSprite::create();
        tpSprite->setID("tp-effect-sprite"_spr);
        tpSprite->setOpacity(0); // Start invisible
        //tpSprite->setBlendFunc({GL_ONE_MINUS_SRC_COLOR, GL_ONE});
        
        // We'll set the display frame and scale later when we need it
        this->addChild(tpSprite);
        
        return true;
    }

    void switchedToMode(GameObjectType p0) {
        PlayerObject::switchedToMode(p0);
        
        // Get the PlayLayer and TP bar
        auto playLayer = PlayLayer::get();
        if (!playLayer) return;
        
        auto plHasBar = playLayer->getChildByID("tp-bar-container"_spr);
        if (!plHasBar) return;
        
        auto barFill = plHasBar->getChildByID("tp-bar-fill"_spr);
        if (!barFill) return;
        
        float currentScaleY = barFill->getScaleY();
        
        if (currentScaleY <= 0.f) return;
        
        // Calculate drain amount: 10% to 100% of current TP
        float minDrain = currentScaleY * 0.1f;
        float maxDrain = currentScaleY;
        float drainAmount = getRandomFloat(minDrain, maxDrain);
        
        float newScaleY = currentScaleY - drainAmount;
        if (newScaleY < 0.f) newScaleY = 0.f;
        
        // Update the bar fill
        auto barScaleAction = CCEaseInOut::create(CCScaleTo::create(0.25f, 1.f, newScaleY), 2.0f);
        barFill->runAction(barScaleAction);
        
        // Update the line position
        auto barFillLine = plHasBar->getChildByID("tp-bar-filler-line"_spr);
        if (barFillLine) {
            auto fillSize = barFill->getContentSize();
            auto moveLineAnim = CCEaseInOut::create(CCMoveTo::create(0.1f, {barFill->getPositionX(), fillSize.height * newScaleY}), 2.0f);
            barFillLine->runAction(moveLineAnim);
            //barFillLine->setPosition({barFill->getPositionX(), fillSize.height * newScaleY});
        }
        
        // Update the percentage label
        auto theLabel = static_cast<CCLabelBMFont*>(plHasBar->getChildByID("tp-bar-percent-label"_spr));
        if (theLabel) {
            float tpAmountFloat = newScaleY * 100.f;
            int tpAmount = static_cast<int>(tpAmountFloat);
            auto tpString = fmt::format("{}", tpAmount);
            theLabel->setString(tpString.c_str(), true);
        }

    }
};