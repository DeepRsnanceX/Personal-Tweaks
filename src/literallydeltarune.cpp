#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(DeltaPlayLayer, PlayLayer) {
    struct Fields {
        CCSprite* dashLabel = CCSprite::create("dashLabel.png"_spr);
        CCSprite* downSpr = CCSprite::create("downMsg.png"_spr);
        CCSprite* healSpr = CCSprite::create("revivedText.png"_spr);
        CCSprite* krisTabAlive = CCSprite::create("krisTab_alive.png"_spr);
        CCSprite* krisTabDown = CCSprite::create("krisTab_down.png"_spr);
        CCSprite* barAnim = CCSprite::create("linesTabLoop.gif"_spr);
        bool isTabHidden = false;
        bool hasDied = false;
    };

    void delayStartSound(float dt){
        FMODAudioEngine::sharedEngine()->playEffect("snd_weaponpull_fast.ogg"_spr);
    }

    void dashBtnPressed(CCObject* sender) {
        auto fields = m_fields.self();
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto fmod = FMODAudioEngine::sharedEngine();
        auto UILayer = UILayer::get();

        auto tab = UILayer->getChildByID("deltarune-ui-node"_spr);
        if (!tab) return;

        auto hideTab = CCEaseOut::create(CCMoveTo::create(0.25f, {winSize.width / 2.f, -21.5f}), 3.f);

        tab->stopAllActions();
        tab->runAction(hideTab);

        fmod->playEffect("snd_select.ogg"_spr);

        fields->isTabHidden = true;
    }

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        
        auto fields = m_fields.self();
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto UILayer = UILayer::get();

        auto containerNode = CCNode::create();
        containerNode->setContentSize(fields->krisTabAlive->getContentSize());
        containerNode->setID("deltarune-ui-node"_spr);
        containerNode->setAnchorPoint({0.5f, 0.f});
        containerNode->setScale(1.3f);

        fields->krisTabAlive->setID("kris-tab-alive"_spr);
        fields->krisTabDown->setID("kris-tab-down"_spr);
        fields->dashLabel->setID("tab-dash-action"_spr);
        fields->barAnim->setID("animated-bars"_spr);

        fields->barAnim->setAnchorPoint({0.5f, 0.f});

        fields->krisTabDown->setVisible(false);

        containerNode->addChild(fields->krisTabAlive);
        containerNode->addChild(fields->krisTabDown);
        containerNode->addChild(fields->dashLabel);
        containerNode->addChild(fields->barAnim);

        auto nodeSize = containerNode->getContentSize();

        fields->krisTabAlive->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});
        fields->krisTabDown->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});
        fields->dashLabel->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});
        fields->barAnim->setPosition({nodeSize.width / 2.f, 0.f});

        fields->dashLabel->setZOrder(2);

        UILayer->addChild(containerNode);

        containerNode->setPosition({winSize.width / 2.f, -50.f});
        containerNode->setZOrder(20);

        // when
        fields->downSpr->setColor({ 255, 0, 0 });
        fields->downSpr->setOpacity(0);
        fields->downSpr->setPosition({0.f, 5.f});
        fields->downSpr->setZOrder(1000);
        fields->downSpr->setID("down-sprite"_spr);

        fields->healSpr->setColor({0, 255, 0});
        fields->healSpr->setOpacity(0);
        fields->healSpr->setPosition({0.f, 5.f});
        fields->healSpr->setZOrder(1000);
        fields->healSpr->setID("heal-sprite"_spr);

        auto mainNode = this->getChildByID("main-node");
        auto bLayer = static_cast<CCLayer*>(mainNode->getChildByID("batch-layer"));

        bLayer->addChild(fields->downSpr);

        this->addChild(fields->healSpr);

        // dash button

        auto menu = CCMenu::create();
        auto dashSpr = CCSprite::create("dashBtn.png"_spr);
        auto dashRealBtn = CCMenuItemSpriteExtra::create(
            dashSpr,
            this,
            menu_selector(DeltaPlayLayer::dashBtnPressed)
        );

        menu->addChild(dashRealBtn);

        menu->setLayout(
            RowLayout::create()
                ->setGap(20.f)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setAxisReverse(false)
                ->setCrossAxisOverflow(true)
                ->setAutoGrowAxis(5.f)
        );
        menu->setAnchorPoint({0.5f, 0.f});

        containerNode->addChild(menu);

        menu->setPosition({nodeSize.width / 2.f, 3.f});

        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        auto fields = m_fields.self();
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto fmod = FMODAudioEngine::sharedEngine();

        // no patrick, just bc the code has comments doesn't mean it's ai
        // god forbid i try to actually comment wtf my code does so when i return to it
        // 2 days later having forgotten what EVERYTHING does i can at least read these

        fields->krisTabDown->setVisible(false);
        fields->krisTabAlive->setVisible(true);

        // reset down sprite
        auto forgetDownSpr = CCSequence::create(
            CCFadeOut::create(0.f),
            CCScaleTo::create(0.f, 1.f, 1.f),
            CCMoveTo::create(0.f, {0.f, 0.f}),
            nullptr
        );

        fields->downSpr->stopAllActions();
        fields->downSpr->runAction(forgetDownSpr);

        // heal animation and stuff
        float extraDelay = 0.3f;

        auto downAnim = CCSequence::create(
            CCEaseOut::create(CCMoveBy::create(0.15f, {0.f, 20.f}), 2.f),
            CCEaseBounceOut::create(CCMoveBy::create(0.4f, {0.f, -18.f})),
            CCDelayTime::create(0.25f + extraDelay),
            CCMoveBy::create(0.3f, {0.f, 50.f}),
            nullptr
        );
        auto fadeAwayAnim = CCSequence::create(
            CCDelayTime::create(0.8f + extraDelay),
            CCFadeOut::create(0.3f),
            nullptr
        );
        auto stretchAnim = CCSequence::create(
            CCDelayTime::create(0.8f + extraDelay),
            CCScaleTo::create(0.3f, 1.f, 2.5f),
            nullptr
        );

        auto resetHeal = CCSequence::create(
            CCDelayTime::create(1.1f + extraDelay),
            CCFadeOut::create(0.f),
            CCScaleTo::create(0.f, 1.f, 1.f),
            CCMoveTo::create(0.f, {0.f, 0.f}),
            nullptr
        );

        // reset the tab
        if (fields->isTabHidden) {
            auto enterAction = CCEaseOut::create(CCMoveTo::create(0.3f, {winSize.width / 2.f, 0.f}), 2.f);
            auto UILayer = UILayer::get();
            auto tab = UILayer->getChildByID("deltarune-ui-node"_spr);
            if (!tab) return;

            tab->stopAllActions();
            tab->runAction(enterAction);
        }

        if (fields->hasDied) {
            //fields->healSpr->setPosition({(winSize.width / 2.f) + 86.f, 30.f});
            fields->healSpr->setPosition(convertToNodeSpaceAR(m_player1->getPosition()));
            fields->healSpr->setOpacity(255);
            fields->healSpr->runAction(downAnim);
            fields->healSpr->runAction(fadeAwayAnim);
            fields->healSpr->runAction(stretchAnim);
            fields->healSpr->runAction(resetHeal);

            fmod->playEffect("snd_heal_c.ogg"_spr);
        }
        
    }

    void postUpdate(float p0) {
        PlayLayer::postUpdate(p0);

        auto fields = m_fields.self();

        //fields->healSpr->setPosition(m_player1->getPosition());
    }

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto fmod = FMODAudioEngine::sharedEngine();
        auto fields = m_fields.self();
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto UILayer = UILayer::get();

        auto hasTab = UILayer->getChildByID("deltarune-ui-node"_spr);
        if (!hasTab) return;

        //mod->playEffect("snd_weaponpull_fast.ogg"_spr);

        float delay = 0.25f;

        this->scheduleOnce(schedule_selector(DeltaPlayLayer::delayStartSound), delay);

        auto enterAction = CCSequence::create(
            CCDelayTime::create(delay),
            CCEaseOut::create(CCMoveTo::create(0.3f, {winSize.width / 2.f, 0.f}), 2.f),
            nullptr
        );

        hasTab->runAction(enterAction);
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        PlayLayer::destroyPlayer(player, obj);

        auto bl = GJBaseGameLayer::get();
        auto fields = m_fields.self();

        if (!player->m_isDead) return;
        if (obj == m_anticheatSpike) return;

        fields->downSpr->setPosition(player->getPosition());
        fields->downSpr->setOpacity(255);

        auto downAnim = CCSequence::create(
            CCEaseOut::create(CCMoveBy::create(0.15f, {0.f, 20.f}), 2.f),
            CCEaseBounceOut::create(CCMoveBy::create(0.4f, {0.f, -18.f})),
            CCDelayTime::create(0.25f),
            CCMoveBy::create(0.3f, {0.f, 50.f}),
            nullptr
        );
        auto fadeAwayAnim = CCSequence::create(
            CCDelayTime::create(0.8f),
            CCFadeOut::create(0.3f),
            nullptr
        );
        auto stretchAnim = CCSequence::create(
            CCDelayTime::create(0.8f),
            CCScaleTo::create(0.3f, 1.f, 2.5f),
            nullptr
        );

        auto resetAll = CCSequence::create(
            CCDelayTime::create(1.2f),
            CCScaleTo::create(0.f, 1.f, 1.f),
            CCMoveTo::create(0.f, {0.f, 0.f}),
            nullptr
        );

        fields->downSpr->runAction(downAnim);
        fields->downSpr->runAction(fadeAwayAnim);
        fields->downSpr->runAction(stretchAnim);
        fields->downSpr->runAction(resetAll);

        fields->hasDied = true;

        fields->krisTabAlive->setVisible(!player->m_isDead);
        fields->krisTabDown->setVisible(player->m_isDead);

    }
};