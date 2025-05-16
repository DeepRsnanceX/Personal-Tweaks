#include <hiimjustin000.icon_randomizer_api/include/IconRandomizer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

auto doRandomize = Mod::get()->getSettingValue<bool>("ic-randomizer");
auto randomizerType = Mod::get()->getSettingValue<std::string>("randomizer-type");
auto randomizeCol1 = Mod::get()->getSettingValue<bool>("randomize-color1");
auto randomizeCol2 = Mod::get()->getSettingValue<bool>("randomize-color2");
auto randomizeGlowCol = Mod::get()->getSettingValue<bool>("randomize-glowcolor");

$on_mod(Loaded){
    listenForSettingChanges("ic-randomizer", [](bool value) {
        doRandomize = value;
    });
    listenForSettingChanges("randomizer-type", [](std::string value) {
        randomizerType = value;
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
        player->updatePlayerRobotFrame(gameManager->getPlayerRobot());
    } else if (spider) {
        player->updatePlayerSpiderFrame(gameManager->getPlayerSpider());
    } else if (cube) {
        player->updatePlayerFrame(gameManager->getPlayerFrame());
    }
}

void updatePlayerColors(PlayerObject* pobj) {
    IconRandomizer::init();

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

class $modify(PlayerObject){
    void switchedToMode(GameObjectType p0) {
        PlayerObject::switchedToMode(p0);

        auto playLayer = PlayLayer::get();
        if (!doRandomize || randomizerType != "every-gamemode") return;

        if (playLayer){
            IconRandomizer::init();
            IconRandomizer::randomizeAll(ICON_RANDOMIZER_API_ALL_ICONS, true);

            if (randomizeCol1) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_COLOR_1, true); 
            }
            if (randomizeCol2) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_COLOR_2, true);
            }
            if (randomizeGlowCol) {
                IconRandomizer::randomize(ICON_RANDOMIZER_API_GLOW_COLOR, true);
            }     

            updatePlayerColors(this);
            updatePlayerFrames(this);
        }

    }
};

class $modify(PlayLayer) {
    void resetLevel() {
        PlayLayer::resetLevel();
    }
};