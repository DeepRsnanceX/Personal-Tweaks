#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <random>

using namespace geode::prelude;

std::string chosenChar = Mod::get()->getSettingValue<std::string>("tab-character");

bool hpCooldown = false;

int getNumberForChar(std::string chosen) {
    int ret;

    if (chosenChar == "kris") {
        ret = 1;
    } else if (chosenChar == "susie") {
        ret = 2;
    } else if (chosenChar == "ralsei") {
        ret = 3;
    } else if (chosenChar == "noelle") {
        ret = 4;
    } else if (chosenChar == "player") {
        ret = 5;
    }

    return ret;
}

CCSpriteFrame* getGamemodeFrame(PlayerObject* player) {
    CCSpriteFrame* ret;

    if (player->m_isShip || player->m_isBird) {
        ret = player->m_vehicleSprite->displayFrame();
    } else if (player->m_isSpider) {
        ret = player->m_spiderSprite->m_headSprite->displayFrame();
    } else if (player->m_isRobot) {
        ret = player->m_robotSprite->m_headSprite->displayFrame();
    } else {
        ret = player->m_iconSprite->displayFrame();
    }

    return ret;
}

ccColor3B pastelizeColor(const ccColor3B& color, float factor = 0.4f) {
    // Clamp factor between 0 and 1
    factor = std::max(0.0f, std::min(1.0f, factor));
    
    // Mix with white (255, 255, 255)
    GLubyte r = static_cast<GLubyte>(color.r + (255 - color.r) * factor);
    GLubyte g = static_cast<GLubyte>(color.g + (255 - color.g) * factor);
    GLubyte b = static_cast<GLubyte>(color.b + (255 - color.b) * factor);

    ccColor3B pastelized = ccColor3B({r, g, b});
    
    return pastelized;
}

float getRandomHPFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

float getCurrentTPPercentage() {
    auto playLayer = PlayLayer::get();
    if (!playLayer) return 0.f;
    
    auto plHasBar = playLayer->getChildByID("tp-bar-container"_spr);
    if (!plHasBar) return 0.f;
    
    auto barFill = plHasBar->getChildByID("tp-bar-fill"_spr);
    if (!barFill) return 0.f;
    
    return barFill->getScaleY() * 100.f;
}

void cooldownHP(float dt) {
    hpCooldown = false;
}

$on_mod(Loaded){
    listenForSettingChanges("tab-character", [](std::string value) {
        chosenChar = value;
    });
}

class $modify(DeltaPlayLayer, PlayLayer) {
    struct Fields {
        CCSprite* downSpr = CCSprite::create("downMsg.png"_spr);
        CCSprite* healSpr = CCSprite::create("revivedText.png"_spr);

        // TAB ESSENTIALS
        ccColor3B tabColor = ccColor3B({255, 255, 255});
        CCSprite* tabTop = CCSprite::createWithSpriteFrameName("deltaTab_top.png"_spr);
        CCSprite* tabBottom = CCSprite::create("linesTabLoop.gif"_spr);
        // TAB ELEMENTS
        CCSprite* hpOverlay = CCSprite::createWithSpriteFrameName("hpUI.png"_spr);
        CCSprite* hpBarFill = CCSprite::createWithSpriteFrameName("hpBarFiller.png"_spr);
        CCSprite* nameLabel = nullptr;
        CCSprite* charIcon = nullptr;

        // HP SYSTEM
        CCLabelBMFont* hpLabel = nullptr;
        CCLabelBMFont* damageLabel = nullptr;
        float currentHP = 100.f;
        float lastDamage = 0.f;

        // VARIABLES
        bool isTabHidden = false;
        bool hasDied = false;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        
        auto fields = m_fields.self();
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto UILayer = UILayer::get();
        auto gm = GameManager::sharedState();

        switch (int what = getNumberForChar(chosenChar)) {
            case 1:
                fields->tabColor = ccColor3B({0, 255, 255});
                break;
            case 2:
                fields->tabColor = ccColor3B({255, 0, 255});
                break;
            case 3:
                fields->tabColor = ccColor3B({0, 255, 0});
                break;
            case 4:
                fields->tabColor = ccColor3B({255, 255, 0});
                break;
            case 5:
                fields->tabColor = gm->colorForIdx(gm->getPlayerColor());
                break;
        }

        if (chosenChar != "player") {
            std::string iconFilename = fmt::format("{}Icon_idle.png"_spr, chosenChar);
            std::string labelFilename = fmt::format("nameLabel_{}.png"_spr, chosenChar);

            fields->charIcon = CCSprite::createWithSpriteFrameName(iconFilename.c_str());
            fields->nameLabel = CCSprite::createWithSpriteFrameName(labelFilename.c_str());
        } else {
            std::string labelFilename = fmt::format("nameLabel_{}.png"_spr, chosenChar);
            fields->nameLabel = CCSprite::createWithSpriteFrameName(labelFilename.c_str());

            fields->charIcon = CCSprite::createWithSpriteFrame(m_player1->m_iconSprite->displayFrame());
            fields->charIcon->setPosition({13.25f, 25.75f});
            fields->charIcon->setScale(0.4f);
            fields->charIcon->setColor(pastelizeColor(fields->tabColor));
        }

        // Create HP label
        fields->hpLabel = CCLabelBMFont::create("100", "hpNumbers.fnt"_spr);
        fields->hpLabel->setID("hp-label"_spr);
        fields->hpLabel->setAlignment(kCCTextAlignmentRight);
        fields->hpLabel->setAnchorPoint({1.f, 0.5f});
        fields->hpLabel->setExtraKerning(16);

        // Create damage label
        fields->damageLabel = CCLabelBMFont::create("0", "damageFont.fnt"_spr);
        fields->damageLabel->setID("damage-label"_spr);
        fields->damageLabel->setOpacity(0);
        fields->damageLabel->setExtraKerning(48);

        auto containerNode = CCNode::create();
        containerNode->setContentSize(fields->tabTop->getContentSize());
        containerNode->setID("deltarune-ui-node"_spr);
        containerNode->setAnchorPoint({0.5f, 0.f});
        containerNode->setScale(1.3f);

        fields->tabTop->setID("tab-top"_spr);
        //fields->dashLabel->setID("tab-dash-action"_spr);
        fields->tabBottom->setID("tab-bottom"_spr);
        fields->charIcon->setID("character-icon"_spr);
        fields->nameLabel->setID("character-name"_spr);
        fields->hpBarFill->setID("hp-bar-filler"_spr);

        fields->tabBottom->setAnchorPoint({0.5f, 0.f});
        fields->hpBarFill->setAnchorPoint({0.f, 0.5f});

        fields->tabTop->setColor(fields->tabColor);
        fields->tabBottom->setColor(fields->tabColor);
        fields->hpBarFill->setColor(fields->tabColor);

        containerNode->addChild(fields->tabTop);
        //containerNode->addChild(fields->dashLabel);
        containerNode->addChild(fields->tabBottom);
        containerNode->addChild(fields->charIcon);
        containerNode->addChild(fields->nameLabel);
        containerNode->addChild(fields->hpOverlay);
        containerNode->addChild(fields->hpBarFill);
        containerNode->addChild(fields->hpLabel);

        auto nodeSize = containerNode->getContentSize();

        fields->tabTop->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});
        //fields->dashLabel->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});
        fields->tabBottom->setPosition({nodeSize.width / 2.f, 0.f});
        fields->nameLabel->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});
        fields->hpOverlay->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});
        fields->hpBarFill->setPosition({64.05f, 22.7f});
        fields->hpLabel->setPosition({80.f, 19.f});
        if (chosenChar != "player") fields->charIcon->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});

        //fields->dashLabel->setZOrder(2);
        fields->hpBarFill->setZOrder(2);

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
        this->addChild(fields->damageLabel);

        // dash button

        auto menu = CCMenu::create();
        auto dashSpr = CCSprite::createWithSpriteFrameName("dashBtn.png"_spr);
        auto dashRealBtn = CCMenuItemSpriteExtra::create(
            dashSpr,
            this,
            menu_selector(DeltaPlayLayer::dashBtnPressed)
        );

        auto healSpr = CCSprite::createWithSpriteFrameName("magicBtn.png"_spr);
        auto healRealBtn = CCMenuItemSpriteExtra::create(
            healSpr,
            this,
            menu_selector(DeltaPlayLayer::healPrayer)
        );

        dashSpr->setColor({255, 127, 39});
        healSpr->setColor({255, 127, 39});

        menu->addChild(dashRealBtn);
        menu->addChild(healRealBtn);

        menu->setLayout(
            RowLayout::create()
                ->setGap(2.f)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setAxisReverse(false)
                ->setCrossAxisOverflow(true)
                ->setAutoGrowAxis(5.f)
        );
        menu->setAnchorPoint({0.5f, 0.f});

        containerNode->addChild(menu);

        menu->setPosition({nodeSize.width / 2.f, 0.f});

        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        auto fields = m_fields.self();
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto fmod = FMODAudioEngine::sharedEngine();

        // Reset HP
        fields->currentHP = 100.f;
        fields->hpBarFill->setScaleX(1.f);
        fields->hpLabel->setString("100", true);
        fields->hpLabel->setColor({255, 255, 255});
        hpCooldown = false;

        // no patrick, just bc the code has comments doesn't mean it's ai
        // god forbid i try to actually comment wtf my code does so when i return to it
        // 2 days later having forgotten what EVERYTHING does i can at least read these

        // reset down sprite
        auto forgetDownSpr = CCSequence::create(
            CCFadeOut::create(0.f),
            CCScaleTo::create(0.f, 1.f, 1.f),
            CCMoveTo::create(0.f, {0.f, 0.f}),
            nullptr
        );

        fields->downSpr->stopAllActions();
        fields->downSpr->runAction(forgetDownSpr);

        // reset damage label
        auto forgetDamageLabel = CCSequence::create(
            CCFadeOut::create(0.f),
            CCScaleTo::create(0.f, 1.f, 1.f),
            CCMoveTo::create(0.f, {0.f, 0.f}),
            nullptr
        );

        fields->damageLabel->stopAllActions();
        fields->damageLabel->runAction(forgetDamageLabel);

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
            auto playerWorldPos = m_player1->getPosition();
            
            auto mainNode = this->getChildByID("main-node");
            auto bLayer = static_cast<CCLayer*>(mainNode->getChildByID("batch-layer"));
            
            auto worldPos = bLayer->convertToWorldSpace(playerWorldPos);
            
            auto screenPos = this->convertToNodeSpace(worldPos);
            
            fields->healSpr->setPosition({screenPos.x + 15.f, screenPos.y});
            fields->healSpr->setOpacity(255);
            fields->healSpr->runAction(downAnim);
            fields->healSpr->runAction(fadeAwayAnim);
            fields->healSpr->runAction(stretchAnim);
            fields->healSpr->runAction(resetHeal);

            fmod->playEffect("snd_heal_c.ogg"_spr);
        }

        if (chosenChar != "player") {
            std::string iconFilename = fmt::format("{}Icon_idle.png"_spr, chosenChar);

            fields->charIcon = CCSprite::createWithSpriteFrameName(iconFilename.c_str());
        }
        
    }

    void postUpdate(float p0) {
        PlayLayer::postUpdate(p0);

        auto fields = m_fields.self();

        if (!fields->charIcon) return;
        if (chosenChar != "player") return;

        auto frame = getGamemodeFrame(m_player1);
        fields->charIcon->setDisplayFrame(frame);

        fields->tabColor = m_player1->m_playerColor1;

        fields->tabTop->setColor(fields->tabColor);
        fields->tabBottom->setColor(fields->tabColor);
        fields->hpBarFill->setColor(fields->tabColor);
        fields->charIcon->setColor(pastelizeColor(fields->tabColor));
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
        auto fields = m_fields.self();
        auto fmod = FMODAudioEngine::sharedEngine();

        if (!player->m_isDead && obj != m_anticheatSpike && !hpCooldown) {
            if (fields->currentHP > 0) {

                float damageAmount = getRandomHPFloat(10.f, 40.f);
                fields->lastDamage = damageAmount;
                fields->currentHP -= damageAmount;
                
                hpCooldown = true;
                this->scheduleOnce(schedule_selector(DeltaPlayLayer::cooldownHPFunction), 1.1f);
                fmod->playEffect("snd_hurt.ogg"_spr);

                // Update HP bar
                float newScaleX = std::max(0.f, fields->currentHP / 100.f);
                fields->hpBarFill->setScaleX(newScaleX);

                // Update HP label
                if (fields->currentHP <= 0) {
                    int displayHP = static_cast<int>(std::round(fields->currentHP));
                    fields->hpLabel->setString(fmt::format("{}", displayHP).c_str(), true);
                    fields->hpLabel->setColor({255, 0, 0});
                    
                    // Use down sprite for death
                    auto playerWorldPos = player->getPosition();
                    auto mainNode = this->getChildByID("main-node");
                    auto bLayer = static_cast<CCLayer*>(mainNode->getChildByID("batch-layer"));
                    auto worldPos = bLayer->convertToWorldSpace(playerWorldPos);
                    auto screenPos = bLayer->convertToNodeSpace(worldPos);
                    
                    fields->downSpr->setPosition(screenPos);
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
                    
                    // Actually kill the player now
                    PlayLayer::destroyPlayer(player, obj);
                } else {
                    // Show damage indicator
                    int displayHP = static_cast<int>(std::round(fields->currentHP));
                    fields->hpLabel->setString(fmt::format("{}", displayHP).c_str(), true);
                    fmod->playEffect("snd_hurt.ogg"_spr);
                    
                    // Position and animate damage label
                    auto playerWorldPos = player->getPosition();
                    auto mainNode = this->getChildByID("main-node");
                    auto bLayer = static_cast<CCLayer*>(mainNode->getChildByID("batch-layer"));
                    auto worldPos = bLayer->convertToWorldSpace(playerWorldPos);
                    auto screenPos = this->convertToNodeSpace(worldPos);
                    
                    fields->damageLabel->setPosition(screenPos);
                    fields->damageLabel->setString(fmt::format("{}", static_cast<int>(damageAmount)).c_str(), true);
                    fields->damageLabel->setOpacity(255);

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

                    auto resetDamageLabel = CCSequence::create(
                        CCDelayTime::create(1.1f),
                        CCFadeOut::create(0.f),
                        CCScaleTo::create(0.f, 1.f, 1.f),
                        CCMoveTo::create(0.f, {0.f, 0.f}),
                        nullptr
                    );

                    fields->damageLabel->runAction(downAnim);
                    fields->damageLabel->runAction(fadeAwayAnim);
                    fields->damageLabel->runAction(stretchAnim);
                    fields->damageLabel->runAction(resetDamageLabel);
                }

                // TODO: Update charIcon for non-player characters when taking damage
                if (chosenChar != "player") {
                    // Add logic here to update charIcon to hurt frame
                }

                return; // Don't call original if we still have HP
            }
        }

    }

    void cooldownHPFunction(float dt) {
        hpCooldown = false;
    }

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

    void healPrayer(CCObject* sender) {
        auto fields = m_fields.self();
        auto fmod = FMODAudioEngine::sharedEngine();

        // Check for enough TP (32% at least)
        float currentTP = getCurrentTPPercentage();
        if (currentTP < 32.f) {
            //fmod->playEffect("snd_cancel.gg"_spr);
            return;
        }

        // Get random heal amount
        float healAmount = getRandomHPFloat(15.f, 40.f);
        fields->currentHP += healAmount;
        
        bool reachedMax = false;
        if (fields->currentHP >= 100.f) {
            fields->currentHP = 100.f;
            reachedMax = true;
        }

        float newScaleX = std::min(1.f, fields->currentHP / 100.f);
        fields->hpBarFill->setScaleX(newScaleX);

        int displayHP = static_cast<int>(fields->currentHP);
        fields->hpLabel->setString(fmt::format("{}", displayHP).c_str(), true);
        if (fields->currentHP > 0) {
            fields->hpLabel->setColor({255, 255, 255});
        }

        PlayerObject* targetPlayer = m_player1; // handle p2 later
        auto playerWorldPos = targetPlayer->getPosition();
        auto mainNode = this->getChildByID("main-node");
        auto bLayer = static_cast<CCLayer*>(mainNode->getChildByID("batch-layer"));
        auto worldPos = bLayer->convertToWorldSpace(playerWorldPos);
        auto screenPos = this->convertToNodeSpace(worldPos);

        if (reachedMax) {
            fields->healSpr->setPosition({screenPos.x + 15.f, screenPos.y});
            fields->healSpr->setOpacity(255);

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

            auto resetHeal = CCSequence::create(
                CCDelayTime::create(1.1f),
                CCFadeOut::create(0.f),
                CCScaleTo::create(0.f, 1.f, 1.f),
                CCMoveTo::create(0.f, {0.f, 0.f}),
                nullptr
            );

            fields->healSpr->runAction(downAnim);
            fields->healSpr->runAction(fadeAwayAnim);
            fields->healSpr->runAction(stretchAnim);
            fields->healSpr->runAction(resetHeal);
        } else {
            // Create and animate healing indicator label
            auto healingLabel = CCLabelBMFont::create(fmt::format("+{}", static_cast<int>(healAmount)).c_str(), "damageFont.fnt"_spr);
            healingLabel->setColor({0, 255, 0}); // Green color
            healingLabel->setPosition({screenPos.x + 15.f, screenPos.y});
            healingLabel->setZOrder(1000);
            
            this->addChild(healingLabel);

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

            auto cleanup = CCSequence::create(
                CCDelayTime::create(1.1f),
                CCCallFunc::create(healingLabel, callfunc_selector(CCNode::removeFromParent)),
                nullptr
            );

            healingLabel->runAction(downAnim);
            healingLabel->runAction(fadeAwayAnim);
            healingLabel->runAction(stretchAnim);
            healingLabel->runAction(cleanup);
        }

        // Play heal sound
        fmod->playEffect("snd_heal_c.ogg"_spr);
    }
};