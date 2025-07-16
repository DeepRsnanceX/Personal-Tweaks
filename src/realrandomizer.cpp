#include <hiimjustin000.icon_randomizer_api/include/IconRandomizer.hpp>
#include <hiimjustin000.more_icons/include/MoreIcons.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>

using namespace geode::prelude;

auto doCondRandomize  = Mod::get()->getSettingValue<bool>("cond-ic-randomizer");
auto doLiveRandomize  = Mod::get()->getSettingValue<bool>("live-ic-randomizer");
auto maxDelay         = Mod::get()->getSettingValue<double>("random-delay");
auto randomizerType   = Mod::get()->getSettingValue<std::string>("cond-randomizer-type");
auto randomizeCube    = Mod::get()->getSettingValue<bool>("randomize-cube");
auto randomizeShip    = Mod::get()->getSettingValue<bool>("randomize-ship");
auto randomizeBall    = Mod::get()->getSettingValue<bool>("randomize-ball");
auto randomizeUfo     = Mod::get()->getSettingValue<bool>("randomize-ufo");
auto randomizeWave    = Mod::get()->getSettingValue<bool>("randomize-wave");
auto randomizeRobot   = Mod::get()->getSettingValue<bool>("randomize-robot");
auto randomizeSpider  = Mod::get()->getSettingValue<bool>("randomize-spider");
auto randomizeSwing   = Mod::get()->getSettingValue<bool>("randomize-swing");
auto randomizeCol1    = Mod::get()->getSettingValue<bool>("randomize-color1");
auto randomizeCol2    = Mod::get()->getSettingValue<bool>("randomize-color2");
auto randomizeGlowCol = Mod::get()->getSettingValue<bool>("randomize-glowcolor");

$on_mod(Loaded){
    listenForSettingChanges("cond-ic-randomizer", [](bool value) {
        doCondRandomize = value;
    });
    listenForSettingChanges("live-ic-randomizer", [](bool value) {
        doLiveRandomize = value;
    });
    listenForSettingChanges("random-delay", [](double value) {
        maxDelay = value;
    });
    listenForSettingChanges("cond-randomizer-type", [](std::string value) {
        randomizerType = value;
    });
    listenForSettingChanges("randomize-cube", [](bool value) {
        randomizeCube = value;
    });
    listenForSettingChanges("randomize-ship", [](bool value) {
        randomizeShip = value;
    });
    listenForSettingChanges("randomize-ball", [](bool value) {
        randomizeBall = value;
    });
    listenForSettingChanges("randomize-ufo", [](bool value) {
        randomizeUfo = value;
    });
    listenForSettingChanges("randomize-wave", [](bool value) {
        randomizeWave = value;
    });
    listenForSettingChanges("randomize-robot", [](bool value) {
        randomizeRobot = value;
    });
    listenForSettingChanges("randomize-spider", [](bool value) {
        randomizeSpider = value;
    });
    listenForSettingChanges("randomize-swing", [](bool value) {
        randomizeSwing = value;
    });
    listenForSettingChanges("randomize-color1", [](bool value) {
        randomizeCol1 = value;
    });
    listenForSettingChanges("randomize-color2", [](bool value) {
        randomizeCol2 = value;
    });
    listenForSettingChanges("randomize-glowcolor", [](bool value) {
        randomizeGlowCol = value;
    });
}

static constexpr RandomizeType modeTypes[8] = {
    ICON_RANDOMIZER_API_CUBE,
    ICON_RANDOMIZER_API_SHIP,
    ICON_RANDOMIZER_API_BALL,
    ICON_RANDOMIZER_API_UFO,
    ICON_RANDOMIZER_API_WAVE,
    ICON_RANDOMIZER_API_ROBOT,
    ICON_RANDOMIZER_API_SPIDER,
    ICON_RANDOMIZER_API_SWING
};
static constexpr RandomizeType colorTypes[3] = {
    ICON_RANDOMIZER_API_COLOR_1,
    ICON_RANDOMIZER_API_COLOR_2,
    ICON_RANDOMIZER_API_GLOW_COLOR
};

void updatePlayerFrames(PlayerObject* player) {

    auto gameManager = GameManager::sharedState();
    // for the sake of organization...
    auto ship = player->m_isShip;
    auto ball = player->m_isBall;
    auto bird = player->m_isBird;
    auto dart = player->m_isDart;
    auto swing = player->m_isSwing;
    auto robot = player->m_isRobot;
    auto spider = player->m_isSpider;
    auto cube = !player->m_isShip && !player->m_isBall && !player->m_isBird && !player->m_isDart && !player->m_isSwing && !player->m_isRobot && !player->m_isSpider;
    auto platformer = player->m_isPlatformer;

    if (ship) {
        if (platformer) {
            player->updatePlayerJetpackFrame(gameManager->getPlayerJetpack());
        } else {
            player->updatePlayerShipFrame(gameManager->getPlayerShip());
        }
        player->updatePlayerFrame(gameManager->getPlayerFrame());
    } else if (ball) {
        player->updatePlayerRollFrame(gameManager->getPlayerBall());
    } else if (bird) {
        player->updatePlayerBirdFrame(gameManager->getPlayerBird());
        player->updatePlayerFrame(gameManager->getPlayerFrame());
    } else if (dart) {
        player->updatePlayerDartFrame(gameManager->getPlayerDart());
    } else if (swing) {
        player->updatePlayerSwingFrame(gameManager->getPlayerSwing());
    } else if (robot) {
        //player->updatePlayerRobotFrame(gameManager->getPlayerRobot());
        MoreIcons::updatePlayerObject(player);
        player->m_ghostTrail->m_playerObject = player;
    } else if (spider) {
        //player->updatePlayerSpiderFrame(gameManager->getPlayerSpider());
        MoreIcons::updatePlayerObject(player);
    } else if (cube) {
        player->updatePlayerFrame(gameManager->getPlayerFrame());
    } else {
        player->updatePlayerFrame(gameManager->getPlayerFrame()); // just makin sure
    }

}

void updatePlayerColors(PlayerObject* pobj) {

    auto gameManager = GameManager::sharedState();

    pobj->setColor(gameManager->colorForIdx(gameManager->getPlayerColor()));
    pobj->setSecondColor(gameManager->colorForIdx(gameManager->getPlayerColor2()));

    if (gameManager->getPlayerGlow()){
        pobj->enableCustomGlowColor(gameManager->colorForIdx(gameManager->getPlayerGlowColor()));
    } else {
        pobj->disableCustomGlowColor();
    }

    pobj->updateGlowColor();
    pobj->updatePlayerArt();

}

class $modify(RandomizerPlayer, PlayerObject){
    struct Fields {
        float timePassed = 0.0f;
        bool modeEnabled[8] = {false}; // cache for enabled randomizers
        bool colorEnabled[3] = {false}; // cache for enabled colors
    };

    void updateRandomizerCache() {
        auto fields = m_fields.self();

        // gamemodes
        fields->modeEnabled[0] = randomizeCube;
        fields->modeEnabled[1] = randomizeShip;
        fields->modeEnabled[2] = randomizeBall;
        fields->modeEnabled[3] = randomizeUfo;
        fields->modeEnabled[4] = randomizeWave;
        fields->modeEnabled[5] = randomizeRobot;
        fields->modeEnabled[6] = randomizeSpider;
        fields->modeEnabled[7] = randomizeSwing;

        // colors
        fields->colorEnabled[0] = randomizeCol1;
        fields->colorEnabled[1] = randomizeCol2;
        fields->colorEnabled[2] = randomizeGlowCol;
    }

    bool init(int player, int ship, GJBaseGameLayer* gameLayer, CCLayer* layer, bool playLayer) {
        if (!PlayerObject::init(player, ship, gameLayer, layer, playLayer)) return false;
        
        this->updateRandomizerCache();

        return true;
    }

    void switchedToMode(GameObjectType p0) {
        PlayerObject::switchedToMode(p0);

        auto playLayer = PlayLayer::get();
        if (!playLayer) return;

        auto cube = !m_isShip && !m_isBall && !m_isBird && !m_isDart && !m_isSwing && !m_isRobot && !m_isSpider;
        bool validRandomModes = randomizerType == "every-gamemode" || randomizerType == "both";

        if (playLayer && doCondRandomize && validRandomModes) {
            IconRandomizer::init();
            
            if (cube && randomizeCube) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_CUBE);
            }
            if (m_isShip && randomizeShip) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_SHIP);
            }
            if (m_isBall && randomizeBall) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_BALL);
            }
            if (m_isBird && randomizeUfo) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_UFO);
            }
            if (m_isDart && randomizeWave) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_WAVE);
            }
            if (m_isRobot && randomizeRobot) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_ROBOT);
            }
            if (m_isSpider && randomizeSpider) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_SPIDER);
            }
            if (m_isSwing && randomizeSwing) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_SWING);
            }

            if (randomizeCol1) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_COLOR_1); 
            }
            if (randomizeCol2) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_COLOR_2);
            }
            if (randomizeGlowCol) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_GLOW_COLOR);
            }     

            updatePlayerColors(this);
            updatePlayerFrames(this);
        }
    }

    void toggleBirdMode(bool p0, bool p1) {
        PlayerObject::toggleBirdMode(p0, p1);

        log::debug("bir mod");
    }

    void update(float p0) {
        PlayerObject::update(p0);

        auto playLayer = PlayLayer::get();
        if (!playLayer || !doLiveRandomize) return;

        auto fields = m_fields.self();
        fields->timePassed += p0;
        if (fields->timePassed < maxDelay) return;
        fields->timePassed -= maxDelay;

        static bool initialized = false;
        if (!initialized) {
            IconRandomizer::init();
            initialized = true;
        }

        for (int i = 0; i < 8; ++i) {
            if (fields->modeEnabled[i]) {
                IconRandomizer::randomize(modeTypes[i]);
            }
        }
        for (int i = 0; i < 3; ++i) {
            if (fields->colorEnabled[i]) {
                IconRandomizer::randomize(colorTypes[i]);
            }
        }

        updatePlayerColors(this);
        updatePlayerFrames(this);
    }
};

class $modify(RandomizerPL, PlayLayer) {
    void resetLevel() {

        auto cube = !m_player1->m_isShip && !m_player1->m_isBall && !m_player1->m_isBird && !m_player1->m_isDart && !m_player1->m_isSwing && !m_player1->m_isRobot && !m_player1->m_isSpider;
        bool validRandModes = randomizerType == "on-death" || randomizerType == "both";

        if (doCondRandomize && validRandModes) {
            IconRandomizer::init();
            
            if (cube && randomizeCube) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_CUBE);
            }
            if (m_player1->m_isShip && randomizeShip) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_SHIP);
            }
            if (m_player1->m_isBall && randomizeBall) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_BALL);
            }
            if (m_player1->m_isBird && randomizeUfo) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_UFO);
            }
            if (m_player1->m_isDart && randomizeWave) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_WAVE);
            }
            if (m_player1->m_isRobot && randomizeRobot) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_ROBOT);
            }
            if (m_player1->m_isSpider && randomizeSpider) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_SPIDER);
            }
            if (m_player1->m_isSwing && randomizeSwing) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_SWING);
            }

            if (randomizeCol1) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_COLOR_1, true); 
            }
            if (randomizeCol2) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_COLOR_2, true);
            }
            if (randomizeGlowCol) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_GLOW_COLOR, true);
            }     

            updatePlayerColors(m_player1);
            updatePlayerFrames(m_player1);
        }

        PlayLayer::resetLevel();
    }

    void resume() {
        PlayLayer::resume();
        if (m_player1) static_cast<RandomizerPlayer*>(m_player1)->updateRandomizerCache();
    }

    void resumeAndRestart(bool p0) {
        PlayLayer::resumeAndRestart(p0);
        if (m_player1) static_cast<RandomizerPlayer*>(m_player1)->updateRandomizerCache();
    }
};