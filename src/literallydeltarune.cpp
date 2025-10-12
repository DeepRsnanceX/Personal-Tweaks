#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <geode.custom-keybinds/include/Keybinds.hpp>
#include <capeling.garage-stats-menu/include/StatsDisplayAPI.h>
#include <random>

using namespace geode::prelude;
using namespace keybinds;

std::string chosenChar = Mod::get()->getSettingValue<std::string>("tab-character");
bool enableDeltarune = Mod::get()->getSettingValue<bool>("enable-deltarune");

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
    } else if (chosenChar == "true-player") {
        ret = 6;
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

struct CharacterAttributes {
    float minDamage;
    float maxDamage;
    float maxHealth;
    float bonusHealth = 0.f;
    int magicLv = 1;
    ccColor3B tabColor;
};

CharacterAttributes getCharAttributes(int stars, int isDemon, std::string character) {
    CharacterAttributes attrs;
    auto gm = GameManager::sharedState();
    auto statsManager = GameStatsManager::sharedState();
    int playerStars = statsManager->getStat("6");
    bool demonQualifies = false;

    attrs.magicLv = std::clamp(playerStars / 1000, 1, 25);

    if (stars == 12022004) {
        attrs.minDamage = 20.f;
        attrs.maxDamage = 40.f;
        log::warn("we're using placeholder values!! you're either on a main level or something went pretty wrong.");
    } else if (stars < 10) {
        switch (stars) {
            case 6:
                attrs.minDamage = 10.f;
                attrs.maxDamage = 20.f;
                break;
            case 7:
                attrs.minDamage = 15.f;
                attrs.maxDamage = 30.f;
                break;
            case 8:
                attrs.minDamage = 15.f;
                attrs.maxDamage = 35.f;
                break;
            case 9:
                attrs.minDamage = 20.f;
                attrs.maxDamage = 40.f;
                break;
            default:
                attrs.minDamage = 25.f;
                attrs.maxDamage = 45.f;
                break;
        }
    } else {
        switch (isDemon) {
            case 3: // easy demon
                attrs.minDamage = 30.f;
                attrs.maxDamage = 60.f;
                break;
            case 4: // medium demon
                attrs.minDamage = 35.f;
                attrs.maxDamage = 65.f;
                demonQualifies = true;
                break;
            case 0: // hard demon
                attrs.minDamage = 40.f;
                attrs.maxDamage = 70.f;
                demonQualifies = true;
                break;
            case 5: // insane demon
                attrs.minDamage = 45.f;
                attrs.maxDamage = 75.f;
                demonQualifies = true;
                break;
            case 6: // extreme demon
                attrs.minDamage = 50.f;
                attrs.maxDamage = 80.f;
                demonQualifies = true;
                break;
            default:
                attrs.minDamage = 35.f;
                attrs.maxDamage = 65.f;
                demonQualifies = true;
                break;
        }
    }

    if (demonQualifies) attrs.bonusHealth = 40.f;

    if (character == "kris") {
        attrs.tabColor = ccColor3B({0, 255, 255});
        attrs.maxHealth = 160.f + attrs.bonusHealth;
    } else if (character == "susie") {
        attrs.tabColor = ccColor3B({255, 0, 255});
        attrs.maxHealth = 190.f + attrs.bonusHealth;
    } else if (character == "ralsei") {
        attrs.tabColor = ccColor3B({0, 255, 0});
        attrs.maxHealth = 140.f + attrs.bonusHealth;
    } else if (character == "noelle") {
        attrs.tabColor = ccColor3B({255, 255, 0});
        attrs.maxHealth = 90.f + attrs.bonusHealth;
    } else if (character == "player") {
        attrs.tabColor = gm->colorForIdx(gm->getPlayerColor());
        int playerLv = Mod::get()->getSavedValue<int>("player-lv", 0);
        int bonus = 20 * playerLv;
        attrs.maxHealth = 50.f + bonus;
    } else if (character == "true-player") {
        attrs.tabColor = gm->colorForIdx(gm->getPlayerColor());
        attrs.maxHealth = 1.f;
    }

    return attrs;
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
    
    auto uiLayer = UILayer::get();
    if (!uiLayer) return 0.f;
    
    auto plHasBar = uiLayer->getChildByID("tp-bar-container"_spr);
    if (!plHasBar) return 0.f;
    
    auto barFill = plHasBar->getChildByID("tp-bar-fill"_spr);
    if (!barFill) return 0.f;
    
    return barFill->getScaleY() * 100.f;
}

$on_mod(Loaded){
    BindManager::get()->registerBindable({
		"heal-prayer-key"_spr,
		"Heal Prayer keybind",

		"",

		{ Keybind::create(KEY_C, Modifier::None) },

		Mod::get()->getName()
	});

    BindManager::get()->registerBindable({
		"defend-key"_spr,
		"Defend keybind",

		"",

		{ Keybind::create(KEY_Q, Modifier::None) },

		Mod::get()->getName()
	});

    listenForSettingChanges("tab-character", [](std::string value) {
        chosenChar = value;
    });
    listenForSettingChanges("enable-deltarune", [](bool value) {
        enableDeltarune = value;
    });
}

class $modify(DeltaPlayLayer, PlayLayer) {
    struct Fields {
        CCSprite* downSpr = nullptr;
        CCSprite* healSpr = nullptr;

        // TAB ESSENTIALS
        ccColor3B tabColor = ccColor3B({255, 255, 255});
        CCSprite* tabTop = nullptr;
        CCSprite* tabBottom = nullptr;
        // TAB ELEMENTS
        CCSprite* hpOverlay = nullptr;
        CCSprite* hpBarFill = nullptr;
        CCSprite* nameLabel = nullptr;
        CCSprite* charIcon = nullptr;
        CCSprite* hurtIcon = nullptr;

        // HP SYSTEM
        CCLabelBMFont* hpLabel = nullptr;
        CCLabelBMFont* maxHpLabel = nullptr;
        CCLabelBMFont* damageLabel = nullptr;
        CCLabelBMFont* healingLabel = nullptr;
        float currentHP = 100.f;
        float maxHP = 100.f;
        float lastDamage = 0.f;
        float magicBonus = 0.f;

        // DEFENDING SYSTEM
        CCSprite* defendIcon = nullptr;
        int defendHitsLeft = 0;
        bool isDefending = false;
        bool tabHiddenByDefend = false;

        // VARIABLES
        bool isTabHidden = false;
        bool hasDied = false;

        // FUCK ANDROID
        GJGameLevel* currentLevel = nullptr;
        int levelStars = 12022004;
        int levelDemon = 3;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
	    if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
	
	    if (!enableDeltarune) return true;
	    
	    auto fields = m_fields.self();
	    fields->currentLevel = level;
	    
	    return true;
	}

	void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        if (!enableDeltarune) return;
        
        auto fields = m_fields.self();
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto gm = GameManager::sharedState();
        auto fmod = FMODAudioEngine::sharedEngine();
        auto uiLayer = UILayer::get();
        
        if (!uiLayer) return;

        if (fields->currentLevel) {
            fields->levelStars = fields->currentLevel->m_stars;
            fields->levelDemon = fields->currentLevel->m_demonDifficulty;
            log::info("star and demon values fetched from level successfully!");
        } else {
            log::warn("level not found! ur probably in a main level lol");
            log::info("using placeholder values...");
        }
        
        CharacterAttributes charAttrs = getCharAttributes(fields->levelStars, fields->levelDemon, chosenChar);

        fields->tabColor = charAttrs.tabColor;
        fields->maxHP = charAttrs.maxHealth;
        fields->currentHP = fields->maxHP;

        // NOW create the sprites
        fields->downSpr = CCSprite::create("downMsg.png"_spr);
        fields->healSpr = CCSprite::create("revivedText.png"_spr);
        fields->tabTop = CCSprite::createWithSpriteFrameName("deltaTab_top.png"_spr);
        fields->tabBottom = CCSprite::create("linesTabLoop.gif"_spr);
        fields->hpOverlay = CCSprite::createWithSpriteFrameName("hpUI.png"_spr);
        fields->hpBarFill = CCSprite::createWithSpriteFrameName("hpBarFiller.png"_spr);
        fields->defendIcon = CCSprite::createWithSpriteFrameName("defendIconGlobal.png"_spr);

		fields->defendIcon->setVisible(false);

        fields->hpOverlay->getTexture()->setAliasTexParameters();
        fields->hpBarFill->getTexture()->setAliasTexParameters();
        fields->tabTop->getTexture()->setAliasTexParameters();
        fields->tabBottom->getTexture()->setAliasTexParameters();
        fields->defendIcon->getTexture()->setAliasTexParameters();
        fields->downSpr->getTexture()->setAliasTexParameters();
        fields->healSpr->getTexture()->setAliasTexParameters();

        if (chosenChar == "player" || chosenChar == "true-player") {
            fields->nameLabel = CCSprite::createWithSpriteFrameName("nameLabel_player.png"_spr);
			fields->nameLabel->getTexture()->setAliasTexParameters();

            fields->charIcon = CCSprite::createWithSpriteFrame(m_player1->m_iconSprite->displayFrame());
            fields->charIcon->setPosition({13.25f, 25.75f});
            fields->charIcon->setScale(0.4f);
            fields->charIcon->setColor(pastelizeColor(fields->tabColor));
        } else {
            std::string iconFilename = fmt::format("{}Icon_idle.png"_spr, chosenChar);
            fields->charIcon = CCSprite::createWithSpriteFrameName(iconFilename.c_str());

            std::string labelFilename = fmt::format("nameLabel_{}.png"_spr, chosenChar);
            fields->nameLabel = CCSprite::createWithSpriteFrameName(labelFilename.c_str());
            
            std::string hurtFilename = fmt::format("{}Icon_hurt.png"_spr, chosenChar);
            fields->hurtIcon = CCSprite::createWithSpriteFrameName(hurtFilename.c_str());

			fields->charIcon->getTexture()->setAliasTexParameters();
			fields->nameLabel->getTexture()->setAliasTexParameters();
			fields->hurtIcon->getTexture()->setAliasTexParameters();
        }

        // Create HP label
        fields->hpLabel = CCLabelBMFont::create(fmt::format("{}", static_cast<int>(fields->maxHP)).c_str(), "hpNumbers.fnt"_spr);
        fields->hpLabel->setID("hp-label"_spr);
        fields->hpLabel->setAlignment(kCCTextAlignmentRight);
        fields->hpLabel->setAnchorPoint({1.f, 1.f});

        // Create Max HP label
        fields->maxHpLabel = CCLabelBMFont::create(fmt::format("{}", static_cast<int>(fields->maxHP)).c_str(), "hpNumbers.fnt"_spr);
        fields->maxHpLabel->setID("max-hp-label"_spr);
        fields->maxHpLabel->setAlignment(kCCTextAlignmentRight);
        fields->maxHpLabel->setAnchorPoint({1.f, 1.f});
        fields->maxHpLabel->updateLabel();

        // Create damage label (stays on PlayLayer)
        fields->damageLabel = CCLabelBMFont::create("0", "damageFont.fnt"_spr);
        fields->damageLabel->setID("damage-label"_spr);
        fields->damageLabel->setOpacity(0);

        // Create healing label (stays on PlayLayer)
        fields->healingLabel = CCLabelBMFont::create("0", "damageFont.fnt"_spr);
        fields->healingLabel->setID("healing-label"_spr);
        fields->healingLabel->setOpacity(0);
        fields->healingLabel->setColor({0, 255, 0});

        auto containerNode = CCNode::create();
        containerNode->setContentSize(fields->tabTop->getContentSize());
        containerNode->setID("deltarune-ui-node"_spr);
        containerNode->setAnchorPoint({0.5f, 0.f});
        containerNode->setScale(1.3f);

        fields->tabTop->setID("tab-top"_spr);
        fields->tabBottom->setID("tab-bottom"_spr);
        fields->charIcon->setID("character-icon"_spr);
        fields->nameLabel->setID("character-name"_spr);
        fields->hpBarFill->setID("hp-bar-filler"_spr);
        fields->defendIcon->setID("defend-icon"_spr);

        fields->tabBottom->setAnchorPoint({0.5f, 0.f});
        fields->hpBarFill->setAnchorPoint({0.f, 0.5f});

        fields->tabTop->setColor(fields->tabColor);
        fields->tabBottom->setColor(fields->tabColor);
        fields->hpBarFill->setColor(fields->tabColor);
        fields->defendIcon->setColor(pastelizeColor(fields->tabColor));

        containerNode->addChild(fields->tabTop);
        containerNode->addChild(fields->tabBottom);
        containerNode->addChild(fields->charIcon);
        containerNode->addChild(fields->nameLabel);
        containerNode->addChild(fields->hpOverlay);
        containerNode->addChild(fields->hpBarFill);
        containerNode->addChild(fields->hpLabel);
        containerNode->addChild(fields->maxHpLabel);
        containerNode->addChild(fields->defendIcon);

        auto nodeSize = containerNode->getContentSize();

        fields->tabTop->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});
        fields->tabBottom->setPosition({nodeSize.width / 2.f, 0.f});
        fields->nameLabel->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});
        fields->hpOverlay->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});
        fields->defendIcon->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});
        fields->hpBarFill->setPosition({64.05f, 22.7f});
        fields->hpLabel->setPosition({80.f, 31.5f});
        fields->maxHpLabel->setPosition({102.5f, 31.5f});

        bool invalidForMidPos = chosenChar == "player" || chosenChar == "true-player";
        if (!invalidForMidPos) fields->charIcon->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});

        if (fields->hurtIcon) {
            fields->hurtIcon->setPosition({nodeSize.width / 2.f, nodeSize.height / 2.f});
            containerNode->addChild(fields->hurtIcon);
            fields->hurtIcon->setID("hurt-icon");
            fields->hurtIcon->setVisible(false);
        }

        if (chosenChar == "true-player") {
            fields->hpLabel->setColor({255, 255, 0});
            fields->maxHpLabel->setColor({255, 255, 0});
        }

        fields->hpBarFill->setZOrder(2);

        // Add to UILayer instead of PlayLayer
        uiLayer->addChild(containerNode);

        containerNode->setPosition({winSize.width / 2.f, -50.f});
        containerNode->setZOrder(50);

        // Setup down sprite
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

        this->addChild(fields->downSpr);
        this->addChild(fields->healSpr);
        this->addChild(fields->damageLabel);
        this->addChild(fields->healingLabel);

        auto menu = CCMenu::create();

        // heal button
        auto healSpr = CCSprite::createWithSpriteFrameName("magicBtn.png"_spr);
        auto healRealBtn = CCMenuItemSpriteExtra::create(
            healSpr,
            this,
            menu_selector(DeltaPlayLayer::healPrayer)
        );

        // defend button
        auto defendSpr = CCSprite::createWithSpriteFrameName("defendBtn.png"_spr);
        auto defendRealBtn = CCMenuItemSpriteExtra::create(
            defendSpr,
            this,
            menu_selector(DeltaPlayLayer::playerDefend)
        );

        healSpr->setColor({255, 127, 39});
        defendSpr->setColor({255, 127, 39});

        healSpr->setID("heal-spr"_spr);
        defendSpr->setID("defend-spr"_spr);

        healRealBtn->setID("heal-btn"_spr);
        defendRealBtn->setID("defend-btn"_spr);

        menu->addChild(healRealBtn);
        menu->addChild(defendRealBtn);

        menu->setLayout(
            RowLayout::create()
                ->setGap(2.f)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setAxisReverse(false)
                ->setCrossAxisOverflow(true)
                ->setAutoGrowAxis(5.f)
        );
        menu->setAnchorPoint({0.5f, 0.f});
        menu->setID("buttons-menu"_spr);

        containerNode->addChild(menu);

        menu->setPosition({nodeSize.width / 2.f, 0.f});

        this->addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                DeltaPlayLayer::healPrayerKeybindVer();
            }
            return ListenerResult::Propagate;
        }, "heal-prayer-key"_spr);

        this->addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                DeltaPlayLayer::playerDefendKeybindVer();
            }
            return ListenerResult::Propagate;
        }, "defend-key"_spr);

        // Animate tab entering
        float delay = 0.25f;
        this->scheduleOnce(schedule_selector(DeltaPlayLayer::delayStartSound), delay);

        auto enterAction = CCSequence::create(
            CCDelayTime::create(delay),
            CCEaseOut::create(CCMoveTo::create(0.3f, {winSize.width / 2.f, 0.f}), 2.f),
            nullptr
        );

        containerNode->runAction(enterAction);
    }

    void resetLevel() {
	    PlayLayer::resetLevel();
	
	    if (!enableDeltarune) return;
	    
	    auto fields = m_fields.self();
	    auto winSize = CCDirector::sharedDirector()->getWinSize();
	    auto fmod = FMODAudioEngine::sharedEngine();
	    auto uiLayer = UILayer::get();
	    
	    if (!uiLayer) return;
	
	    // Reset HP
	    fields->currentHP = fields->maxHP;
	    if (fields->hpBarFill) fields->hpBarFill->setScaleX(1.f);
	    if (fields->hpLabel) {
	        fields->hpLabel->setString(fmt::format("{}", static_cast<int>(fields->maxHP)).c_str(), true);
	        fields->hpLabel->setColor({255, 255, 255});
	    }
	    if (fields->maxHpLabel) fields->maxHpLabel->setColor({255, 255, 255});
	    hpCooldown = false;
	
	    if (chosenChar == "true-player") {
	        if (fields->hpLabel) fields->hpLabel->setColor({255, 255, 0});
	        if (fields->maxHpLabel) fields->maxHpLabel->setColor({255, 255, 0});
	    }
	
	    // Reset defending
	    fields->isDefending = false;
	    fields->defendHitsLeft = 0;
	    fields->tabHiddenByDefend = false;
	    if (fields->defendIcon) fields->defendIcon->setVisible(false);
	    if (fields->charIcon) fields->charIcon->setVisible(true);
	
	    // reset down sprite
	    if (fields->downSpr) {
	        auto forgetDownSpr = CCSequence::create(
	            CCFadeOut::create(0.f),
	            CCScaleTo::create(0.f, 1.f, 1.f),
	            CCMoveTo::create(0.f, {0.f, 0.f}),
	            nullptr
	        );
	        fields->downSpr->stopAllActions();
	        fields->downSpr->runAction(forgetDownSpr);
	    }
	
	    // reset damage label
	    if (fields->damageLabel) {
	        auto forgetDamageLabel = CCSequence::create(
	            CCFadeOut::create(0.f),
	            CCScaleTo::create(0.f, 1.f, 1.f),
	            CCMoveTo::create(0.f, {0.f, 0.f}),
	            nullptr
	        );
	        fields->damageLabel->stopAllActions();
	        fields->damageLabel->runAction(forgetDamageLabel);
	    }
	
	    // reset healing label
	    if (fields->healingLabel) {
	        auto forgetHealingLabel = CCSequence::create(
	            CCFadeOut::create(0.f),
	            CCScaleTo::create(0.f, 1.f, 1.f),
	            CCMoveTo::create(0.f, {0.f, 0.f}),
	            nullptr
	        );
	        fields->healingLabel->stopAllActions();
	        fields->healingLabel->runAction(forgetHealingLabel);
	    }
	
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
	    auto tab = uiLayer->getChildByID("deltarune-ui-node"_spr);
	    if (fields->isTabHidden && tab) {
	        auto enterAction = CCEaseOut::create(CCMoveTo::create(0.3f, {winSize.width / 2.f, 0.f}), 2.f);
	        tab->stopAllActions();
	        tab->runAction(enterAction);
	    }
	
	    if (!m_player1) return;
	    
	    auto playerWorldPos = m_player1->getPosition();
	    
	    auto mainNode = this->getChildByID("main-node");
	    if (!mainNode) return;
	    
	    auto bLayer = static_cast<CCLayer*>(mainNode->getChildByID("batch-layer"));
	    if (!bLayer) return;
	    
	    auto worldPos = bLayer->convertToWorldSpace(playerWorldPos);
	    auto screenPos = this->convertToNodeSpace(worldPos);
	    
	    if (fields->healSpr) {
	        fields->healSpr->setPosition({screenPos.x + 15.f, screenPos.y});
	        fields->healSpr->setOpacity(255);
	        fields->healSpr->runAction(downAnim);
	        fields->healSpr->runAction(fadeAwayAnim);
	        fields->healSpr->runAction(stretchAnim);
	        fields->healSpr->runAction(resetHeal);
	    }
	
	    fmod->playEffect("snd_heal_c.ogg"_spr);
	}

    void postUpdate(float p0) {
        PlayLayer::postUpdate(p0);

        if (!enableDeltarune) return;

        auto fields = m_fields.self();

        bool validChars = chosenChar == "player" || chosenChar == "true-player";

        if (!validChars) return;
        if (!fields->charIcon) return;

        auto frame = getGamemodeFrame(m_player1);
        fields->charIcon->setDisplayFrame(frame);

        fields->tabColor = m_player1->m_playerColor1;

        fields->tabTop->setColor(fields->tabColor);
        fields->tabBottom->setColor(fields->tabColor);
        fields->hpBarFill->setColor(fields->tabColor);
        fields->charIcon->setColor(pastelizeColor(fields->tabColor));
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) override {
	    if (!enableDeltarune) return PlayLayer::destroyPlayer(player, obj);
	
	    auto fields = m_fields.self();
	    auto fmod = FMODAudioEngine::sharedEngine();
	    auto uiLayer = UILayer::get();
	    
	    if (!fields->currentLevel || !uiLayer) {
	        return PlayLayer::destroyPlayer(player, obj);
	    }
	    
	    CharacterAttributes charAttrs = getCharAttributes(fields->levelStars, fields->levelDemon, chosenChar);
	
	    if (obj == m_anticheatSpike) {
	        return PlayLayer::destroyPlayer(player, obj);
	    }
	
	    if (!player->m_isDead && !hpCooldown) {
	        if (fields->currentHP > 0) {
	
	            float damageAmount = getRandomHPFloat(charAttrs.minDamage, charAttrs.maxDamage);
	
	            // Apply defense reduction if defending
	            if (fields->isDefending && fields->defendHitsLeft > 0) {
	                damageAmount *= 0.4f;
	                fields->defendHitsLeft--;
	                
	                // End defense if no hits left
	                if (fields->defendHitsLeft <= 0) {
	                    fields->isDefending = false;
	                    if (fields->defendIcon) fields->defendIcon->setVisible(false);
	                    if (fields->charIcon) fields->charIcon->setVisible(true);
	                    
	                    // Show tab again if it was hidden by defend
	                    if (fields->tabHiddenByDefend) {
	                        auto winSize = CCDirector::sharedDirector()->getWinSize();
	                        auto tab = uiLayer->getChildByID("deltarune-ui-node"_spr);
	                        if (tab) {
	                            auto showAction = CCEaseOut::create(CCMoveTo::create(0.3f, {winSize.width / 2.f, 0.f}), 2.f);
	                            tab->runAction(showAction);
	                            fields->isTabHidden = false;
	                            fields->tabHiddenByDefend = false;
	                        }
	                    }
	                }
	            }
	
	            fields->lastDamage = damageAmount;
	            fields->currentHP -= damageAmount;
	            
	            hpCooldown = true;
	            this->scheduleOnce(schedule_selector(DeltaPlayLayer::cooldownHPFunction), 1.1f);
	            fmod->playEffect("snd_hurt.ogg"_spr);
	
	            bool invalidCharsForHurt = chosenChar == "player" || chosenChar == "true-player";
	            
	            if (!invalidCharsForHurt && fields->hurtIcon && fields->charIcon && !fields->isDefending) {
	                fields->charIcon->setVisible(false);
	                fields->hurtIcon->setVisible(true);
	                this->scheduleOnce(schedule_selector(DeltaPlayLayer::resetNormalSprite), 0.5f);
	            }
	
	            // Update HP bar
	            if (fields->hpBarFill) {
	                float newScaleX = std::max(0.f, fields->currentHP / fields->maxHP);
	                fields->hpBarFill->setScaleX(newScaleX);
	            }
	
	            // yellow tint
	            if (fields->currentHP <= fields->maxHP / 4) {
	                if (fields->hpLabel) fields->hpLabel->setColor({255, 255, 0});
	                if (fields->maxHpLabel) fields->maxHpLabel->setColor({255, 255, 0});
	            } else {
	                if (fields->hpLabel) fields->hpLabel->setColor({255, 255, 255});
	                if (fields->maxHpLabel) fields->maxHpLabel->setColor({255, 255, 255});
	            }
	
	            // Update HP label
	            if (fields->currentHP <= 0) {
	                int displayHP = static_cast<int>(std::round(fields->currentHP));
	                if (fields->hpLabel) {
	                    fields->hpLabel->setString(fmt::format("{}", displayHP).c_str(), true);
	                    fields->hpLabel->setColor({255, 0, 0});
	                }
	                if (fields->maxHpLabel) fields->maxHpLabel->setColor({255, 0, 0});
	                
	                // Use down sprite for death
	                auto playerWorldPos = player->getPosition();
	                auto mainNode = this->getChildByID("main-node");
	                if (mainNode && fields->downSpr) {
	                    auto bLayer = static_cast<CCLayer*>(mainNode->getChildByID("batch-layer"));
	                    if (bLayer) {
	                        auto worldPos = bLayer->convertToWorldSpace(playerWorldPos);
	                        auto screenPos = this->convertToNodeSpace(worldPos);
	                        
	                        fields->downSpr->setPosition(screenPos);
	                        fields->downSpr->setOpacity(255);
	
	                        auto downAnim = CCSequence::create(
	                            CCEaseOut::create(CCMoveBy::create(0.15f, {9.f, 20.f}), 2.f),
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
	                    }
	                }
	
	                fields->hasDied = true;
	                
	                // Actually kill the player now
	                return PlayLayer::destroyPlayer(player, obj);
	            } else {
	                // Show damage indicator
	                int displayHP = static_cast<int>(std::round(fields->currentHP));
	                if (fields->hpLabel) fields->hpLabel->setString(fmt::format("{}", displayHP).c_str(), true);
	                fmod->playEffect("snd_hurt.ogg"_spr);
	                
	                // Position and animate damage label
	                auto mainNode = this->getChildByID("main-node");
	                if (mainNode && fields->damageLabel) {
	                    auto bLayer = static_cast<CCLayer*>(mainNode->getChildByID("batch-layer"));
	                    if (bLayer) {
	                        auto playerWorldPos = player->getPosition();
	                        auto worldPos = bLayer->convertToWorldSpace(playerWorldPos);
	                        auto screenPos = this->convertToNodeSpace(worldPos);
	                        
	                        fields->damageLabel->setPosition(screenPos);
	                        fields->damageLabel->setString(fmt::format("{}", static_cast<int>(damageAmount)).c_str(), true);
	                        fields->damageLabel->setOpacity(255);
	
	                        auto downAnim = CCSequence::create(
	                            CCEaseOut::create(CCMoveBy::create(0.15f, {9.f, 20.f}), 2.f),
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
	                }
	            }
	
	            return;
	        }
	    }
	}

    void cooldownHPFunction(float dt) {
        hpCooldown = false;
    }

    void resetNormalSprite(float dt) {
        auto fields = m_fields.self();
        fields->charIcon->setVisible(true);
        fields->hurtIcon->setVisible(false);
    }

    void buttonTintEffect(CCSprite* sprite) {
        auto tintToYellow = CCTintTo::create(0.f, 255, 255, 0);
        auto tintToNormal = CCTintTo::create(0.3f, 255, 127, 39);
        auto tintSequence = CCSequence::create(tintToYellow, tintToNormal, nullptr);
        sprite->runAction(tintSequence);
    }

    void delayStartSound(float dt){
        FMODAudioEngine::sharedEngine()->playEffect("snd_weaponpull_fast.ogg"_spr);
    }

    void playerDefend(CCObject* sender) {
        auto fields = m_fields.self();
        auto fmod = FMODAudioEngine::sharedEngine();
        
        auto menuItem = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto sprite = static_cast<CCSprite*>(menuItem->getNormalImage());

        if (fields->isDefending) {
            fmod->playEffect("snd_cantselect_1.ogg"_spr);
            return;
        }

        buttonTintEffect(sprite);
        
        // Delay the actual action
        fmod->playEffect("snd_select.ogg"_spr);
        this->scheduleOnce(schedule_selector(DeltaPlayLayer::delayedDefendAction), 0.3f);
    }

    void playerDefendKeybindVer() {
        auto fields = m_fields.self();
        auto fmod = FMODAudioEngine::sharedEngine();
		auto uiLayer = UILayer::get();

		if (!uiLayer) return;
        
        auto tab = uiLayer->getChildByID("deltarune-ui-node"_spr);
    	if (!tab) return;
		
        auto menu = static_cast<CCMenu*>(tab->getChildByID("buttons-menu"_spr));
    	if (!menu) return;
		
        auto sprextra = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("defend-btn"_spr));
		if (!sprextra) return;
		
        auto sprite = static_cast<CCSprite*>(sprextra->getChildByID("defend-spr"_spr));
    	if (!sprite) return;

        if (fields->isDefending) {
            fmod->playEffect("snd_cantselect_1.ogg"_spr);
            return;
        }
		
        buttonTintEffect(sprite);
        
        // Delay the actual action
        fmod->playEffect("snd_select.ogg"_spr);
        this->scheduleOnce(schedule_selector(DeltaPlayLayer::delayedDefendAction), 0.3f);
    }

    void delayedDefendAction(float dt) {
	    auto fields = m_fields.self();
	    auto fmod = FMODAudioEngine::sharedEngine();
	    auto winSize = CCDirector::sharedDirector()->getWinSize();
	    auto uiLayer = UILayer::get();
	    
	    if (!uiLayer) return;
	    
	    // Set up defense
	    fields->isDefending = true;
	    fields->defendHitsLeft = 3;
	    
	    // Show defend icon, hide char icon
	    if (fields->defendIcon) {
	        fields->defendIcon->setVisible(true);
	        fields->defendIcon->setColor(pastelizeColor(fields->tabColor));
	    }
	    if (fields->charIcon) fields->charIcon->setVisible(false);
	    
	    // Hide tab
	    auto tab = uiLayer->getChildByID("deltarune-ui-node"_spr);
	    if (tab) {
	        auto hideTab = CCEaseOut::create(CCMoveTo::create(0.25f, {winSize.width / 2.f, -21.5f}), 3.f);
	        tab->stopAllActions();
	        tab->runAction(hideTab);
	        fields->isTabHidden = true;
	        fields->tabHiddenByDefend = true;
	    }
	    
	    // Gain 16% TP
	    auto plHasBar = uiLayer->getChildByID("tp-bar-container"_spr);
	    if (plHasBar) {
	        auto barFill = plHasBar->getChildByID("tp-bar-fill"_spr);
	        if (barFill) {
	            float currentScaleY = barFill->getScaleY();
	            float newScaleY = std::min(1.f, currentScaleY + 0.16f);
	            
	            auto barScaleAction = CCEaseInOut::create(CCScaleTo::create(0.1f, 1.f, newScaleY), 2.0f);
	            barFill->runAction(barScaleAction);
	
	            auto barFillLine = plHasBar->getChildByID("tp-bar-filler-line"_spr);
	            if (barFillLine) {
	                auto fillSize = barFill->getContentSize();
	                barFillLine->setPosition({barFill->getPositionX(), fillSize.height * newScaleY});
	            }
	
	            auto theLabel = static_cast<CCLabelBMFont*>(plHasBar->getChildByID("tp-bar-percent-label"_spr));
	            if (theLabel) {
	                float tpAmountFloat = newScaleY * 100.f;
	                int tpAmount = static_cast<int>(tpAmountFloat);
	                auto tpString = fmt::format("{}", tpAmount);
	                
	                if (tpAmount == 100) {
	                    tpString = "MAX";
	                    theLabel->setScale(0.6f);
	                } else {
	                    theLabel->setScale(0.7f);
	                }
	                
	                theLabel->setString(tpString.c_str(), true);
	            }
	        }
	    }
	}

    void healPrayer(CCObject* sender) {
        auto fields = m_fields.self();
        auto fmod = FMODAudioEngine::sharedEngine();
        
        auto menuItem = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto sprite = static_cast<CCSprite*>(menuItem->getNormalImage());
        
        // Check for enough TP (32% at least)
        float currentTP = getCurrentTPPercentage();
        if (currentTP < 32.f || fields->tabHiddenByDefend) {
            fmod->playEffect("snd_cantselect_1.ogg"_spr);
            return;
        }
        
        // Schedule the actual heal action
        buttonTintEffect(sprite);
        fmod->playEffect("snd_select.ogg"_spr);
        this->scheduleOnce(schedule_selector(DeltaPlayLayer::delayedHealAction), 0.3f);
    }

    void healPrayerKeybindVer() {
	    auto fields = m_fields.self();
	    auto fmod = FMODAudioEngine::sharedEngine();
	    auto uiLayer = UILayer::get();
	    
	    if (!uiLayer) return;
	    
	    auto tab = uiLayer->getChildByID("deltarune-ui-node"_spr);
	    if (!tab) return;
	    
	    auto menu = static_cast<CCMenu*>(tab->getChildByID("buttons-menu"_spr));
	    if (!menu) return;
	    
	    auto sprextra = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("heal-btn"_spr));
	    if (!sprextra) return;
	    
	    auto sprite = static_cast<CCSprite*>(sprextra->getChildByID("heal-spr"_spr));
	    if (!sprite) return;
	    
	    // Check for enough TP (32% at least)
	    float currentTP = getCurrentTPPercentage();
	    if (currentTP < 32.f || fields->tabHiddenByDefend) {
	        fmod->playEffect("snd_cantselect_1.ogg"_spr);
	        return;
	    }
	    
	    // Schedule the actual heal action
	    buttonTintEffect(sprite);
	    fmod->playEffect("snd_select.ogg"_spr);
	    this->scheduleOnce(schedule_selector(DeltaPlayLayer::delayedHealAction), 0.3f);
	}

    void delayedHealAction(float dt) {
        auto fields = m_fields.self();
        auto fmod = FMODAudioEngine::sharedEngine();
        auto uiLayer = UILayer::get();
        CharacterAttributes charAttrs = getCharAttributes(fields->levelStars, fields->levelDemon, chosenChar);

        if (!uiLayer) return;

        // Check for enough TP (32% at least)
        float currentTP = getCurrentTPPercentage();

        if (m_player1->m_isSpider || m_player1->m_isDart) { 
            fields->magicBonus = 6.f;
        } else if (m_player1->m_isBird || m_player1->m_isRobot) {
            fields->magicBonus = 5.f;
        } else if (m_player1->m_isSwing) {
            fields->magicBonus = 7.f;
        } else if (m_player1->m_isShip) {
            fields->magicBonus = 3.f;
        } else if (m_player1->m_isBall) {
            fields->magicBonus = 4.f;
        } else {
            fields->magicBonus = 1.f;
        }

        // Deduct 32% TP
        auto plHasBar = uiLayer->getChildByID("tp-bar-container"_spr);
        if (plHasBar) {
            auto barFill = plHasBar->getChildByID("tp-bar-fill"_spr);
            if (barFill) {
                float currentScaleY = barFill->getScaleY();
                float newScaleY = std::max(0.f, currentScaleY - 0.32f);
                
                auto barScaleAction = CCEaseInOut::create(CCScaleTo::create(0.1f, 1.f, newScaleY), 2.0f);
                barFill->runAction(barScaleAction);

                auto barFillLine = plHasBar->getChildByID("tp-bar-filler-line"_spr);
                if (barFillLine) {
                    auto fillSize = barFill->getContentSize();
                    barFillLine->setPosition({barFill->getPositionX(), fillSize.height * newScaleY});
                }

                auto theLabel = static_cast<CCLabelBMFont*>(plHasBar->getChildByID("tp-bar-percent-label"_spr));
                if (theLabel) {
                    float tpAmountFloat = newScaleY * 100.f;
                    int tpAmount = static_cast<int>(tpAmountFloat);
                    auto tpString = fmt::format("{}", tpAmount);
                    
                    if (tpAmount == 100) {
                        tpString = "MAX";
                        theLabel->setScale(0.6f);
                    } else {
                        theLabel->setScale(0.7f);
                    }
                    
                    theLabel->setString(tpString.c_str(), true);
                }
            }
        }

        // Get random heal amount
        int fullMagicLv = charAttrs.magicLv + static_cast<int>(fields->magicBonus);
        float healAmount = fullMagicLv * 5.f;
        fields->currentHP += healAmount;
        
        bool reachedMax = false;
        if (fields->currentHP >= fields->maxHP) {
            fields->currentHP = fields->maxHP;
            reachedMax = true;
        }

        float newScaleX = std::min(1.f, fields->currentHP / fields->maxHP);
        fields->hpBarFill->setScaleX(newScaleX);

        int displayHP = static_cast<int>(fields->currentHP);
        fields->hpLabel->setString(fmt::format("{}", displayHP).c_str(), true);
        if (fields->currentHP > fields->maxHP / 4) {
            fields->hpLabel->setColor({255, 255, 255});
            fields->maxHpLabel->setColor({255, 255, 255});
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

            fields->healSpr->stopAllActions();
            fields->healSpr->runAction(downAnim);
            fields->healSpr->runAction(fadeAwayAnim);
            fields->healSpr->runAction(stretchAnim);
            fields->healSpr->runAction(resetHeal);
        } else {
            // Position and animate healing label
            fields->healingLabel->setPosition(screenPos);
            fields->healingLabel->setString(fmt::format("{}", static_cast<int>(healAmount)).c_str(), true);
            fields->healingLabel->setOpacity(255);

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

            auto resetHealingLabel = CCSequence::create(
                CCDelayTime::create(1.1f),
                CCFadeOut::create(0.f),
                CCScaleTo::create(0.f, 1.f, 1.f),
                CCMoveTo::create(0.f, {0.f, 0.f}),
                nullptr
            );

            fields->healingLabel->stopAllActions();
            fields->healingLabel->runAction(downAnim);
            fields->healingLabel->runAction(fadeAwayAnim);
            fields->healingLabel->runAction(stretchAnim);
            fields->healingLabel->runAction(resetHealingLabel);
        }

        // Play heal sound
        fmod->playEffect("snd_heal_c.ogg"_spr);
    }

};

class $modify(DeltaEndLevelLayer, EndLevelLayer) {
    struct Fields {
        CCLabelBMFont* strongerLabel = nullptr;
        bool gotStronger = false;
    };

    void customSetup() {
        EndLevelLayer::customSetup();

        if (!enableDeltarune) return;

        auto accManager = GJAccountManager::get();
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto x = winSize.width / 2.f;
        auto y = winSize.height / 2.f;
        auto fields = m_fields.self();
        auto mainLayer = this->getChildByID("main-layer");

        int starsProgress = Mod::get()->getSavedValue<int>("stars-progress", 0);

        Mod::get()->setSavedValue<int>("stars-progress", starsProgress + m_stars);

        int newStars = Mod::get()->getSavedValue<int>("stars-progress", 0);
        int starsLeft = 100 - newStars;

        if (starsLeft <= 0) fields->gotStronger = true;

        std::string gotStronger = fmt::format("* {} got stronger.", accManager->m_username);
        std::string notYetStronger = fmt::format("* {} LEFT.", starsLeft);
        std::string strongerLabelText = (fields->gotStronger) ? gotStronger : notYetStronger;

        fields->strongerLabel = CCLabelBMFont::create(strongerLabelText.c_str(), "deltarune.fnt"_spr);
        fields->strongerLabel->setOpacity(0);
        fields->strongerLabel->setScale(0.6f);
        fields->strongerLabel->setPosition({x, y + 55.f});

        mainLayer->addChild(fields->strongerLabel);
    }

    void lvlUpStuff(float dt) {
        auto fields = m_fields.self();
        int currentLevel = Mod::get()->getSavedValue<int>("player-lv", 0);

        auto showLabel = CCEaseIn::create(CCFadeIn::create(0.4f), 2.f);

        if (fields->gotStronger) {
            Mod::get()->setSavedValue<int>("stars-progress", 0);
            Mod::get()->setSavedValue<int>("player-lv", currentLevel + 1);

            FMODAudioEngine::sharedEngine()->playEffect("gettingStronger.ogg"_spr);
        }

        fields->strongerLabel->runAction(showLabel);
        
    }

    void showLayer(bool p0) {
        EndLevelLayer::showLayer(p0);
        if (!enableDeltarune) return;

        this->scheduleOnce(schedule_selector(DeltaEndLevelLayer::lvlUpStuff), 0.3f);
    }
};

class $modify(DeltaruneStatsGarage, GJGarageLayer) {
    void resetPlayerLevel(CCObject* sender) {
        Mod::get()->setSavedValue<int>("player-lv", 0);
    }

    bool init() {
        if (!GJGarageLayer::init()) return false;
        
        if (!enableDeltarune) return true;
        auto statMenu = this->getChildByID("capeling.garage-stats-menu/stats-menu");
        if (!statMenu) return true;

        auto playerLv = StatsDisplayAPI::getNewItem("player-level"_spr, CCSprite::createWithSpriteFrameName("LVLabel.png"_spr), Mod::get()->getSavedValue<int>("player-lv", 0));

        statMenu->addChild(playerLv);
        statMenu->updateLayout();

        auto resetBtnSpr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
        auto resetBtn = CCMenuItemSpriteExtra::create(
            resetBtnSpr,
            this,
            menu_selector(DeltaruneStatsGarage::resetPlayerLevel)
        );
        auto godIFuckingHateThatTheseNeedToBeOnAMenuFuckingHellFuckingShit = CCMenu::create();
        godIFuckingHateThatTheseNeedToBeOnAMenuFuckingHellFuckingShit->setContentSize(resetBtnSpr->getContentSize());
        godIFuckingHateThatTheseNeedToBeOnAMenuFuckingHellFuckingShit->addChild(resetBtn);
        godIFuckingHateThatTheseNeedToBeOnAMenuFuckingHellFuckingShit->setID("reset-lv-btn"_spr);
        godIFuckingHateThatTheseNeedToBeOnAMenuFuckingHellFuckingShit->setPosition({-50.f, -200.f});

        resetBtn->setPosition({godIFuckingHateThatTheseNeedToBeOnAMenuFuckingHellFuckingShit->getContentSize().width / 2.f, godIFuckingHateThatTheseNeedToBeOnAMenuFuckingHellFuckingShit->getContentSize().height / 2.f});

        this->addChild(godIFuckingHateThatTheseNeedToBeOnAMenuFuckingHellFuckingShit);

        return true;
    }
};