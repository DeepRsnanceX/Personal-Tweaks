#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

auto useCustomColors = Mod::get()->getSettingValue<bool>("enable-customcolors");
auto tintTrail = Mod::get()->getSettingValue<bool>("tint-trail");
auto p1PrimaryColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("p1-primary");
auto p1SecondaryColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("p1-secondary");
auto p1WaveColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("p1-wave");
auto p2PrimaryColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("p2-primary");
auto p2SecondaryColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("p2-secondary");
auto p2WaveColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>("p2-wave");

$on_mod(Loaded) {
    listenForSettingChanges("tint-trail", [](bool value) {
        useCustomColors = value;
    });
    listenForSettingChanges("enable-customcolors", [](bool value) {
        useCustomColors = value;
    });
    listenForSettingChanges("p1-primary", [](cocos2d::ccColor3B value) {
        p1PrimaryColor = value;
    });
    listenForSettingChanges("p1-secondary", [](cocos2d::ccColor3B value) {
        p1SecondaryColor = value;
    });
    listenForSettingChanges("p1-wave", [](cocos2d::ccColor3B value) {
        p1WaveColor = value;
    });
    listenForSettingChanges("p2-primary", [](cocos2d::ccColor3B value) {
        p2PrimaryColor = value;
    });
    listenForSettingChanges("p2-secondary", [](cocos2d::ccColor3B value) {
        p2SecondaryColor = value;
    });
    listenForSettingChanges("p2-wave", [](cocos2d::ccColor3B value) {
        p2WaveColor = value;
    });
}

class $modify(ColorsPlayer, PlayerObject) {
    void update(float p0) {
        PlayerObject::update(p0);

        if (!useCustomColors) return;

        m_iconSprite->setColor(p1PrimaryColor);
        m_iconSpriteSecondary->setColor(p1SecondaryColor);
        m_vehicleSprite->setColor(p1PrimaryColor);
        m_vehicleSpriteSecondary->setColor(p1SecondaryColor);
        if (m_waveTrail) {
            m_waveTrail->setColor(p1WaveColor);
        }
        if (m_isRobot) {
            m_robotSprite->m_color = p1PrimaryColor;
            m_robotSprite->m_secondColor = p1SecondaryColor;
            m_robotSprite->updateColors();
        } else if (m_isSpider) {
            m_spiderSprite->m_color = p1PrimaryColor;
            m_spiderSprite->m_secondColor = p1SecondaryColor;
            m_spiderSprite->updateColors();
        }
        

        if (m_isSecondPlayer) {
            m_iconSprite->setColor(p2PrimaryColor);
            m_iconSpriteSecondary->setColor(p2SecondaryColor);
            m_vehicleSprite->setColor(p2PrimaryColor);
            m_vehicleSpriteSecondary->setColor(p2SecondaryColor);
            if (m_waveTrail) {
                m_waveTrail->setColor(p2WaveColor);
            }
            if (m_isRobot) {
                m_robotSprite->m_color = p2PrimaryColor;
                m_robotSprite->m_secondColor = p2SecondaryColor;
                m_robotSprite->updateColors();
            } else if (m_isSpider) {
                m_spiderSprite->m_color = p2PrimaryColor;
                m_spiderSprite->m_secondColor = p2SecondaryColor;
                m_spiderSprite->updateColors();
            }
        }
    }
};

class $modify(CustomColorsPlayLayer, PlayLayer) {
    void resetLevel() {
        PlayLayer::resetLevel();

        /*
        if (!useCustomColors) return;

        m_player1->setColor(p1PrimaryColor);
        m_player1->setSecondColor(p1SecondaryColor);

        m_player2->setColor(p2PrimaryColor);
        m_player2->setSecondColor(p2SecondaryColor);

        m_player1->m_waveTrail->setColor(p1PrimaryColor);
        m_player2->m_waveTrail->setColor(p2PrimaryColor);

        m_player1->m_regularTrail->setColor(p1SecondaryColor);
        m_player2->m_regularTrail->setColor(p2SecondaryColor);

        m_player1->updatePlayerArt();
        m_player2->updatePlayerArt();

        m_player1->m_colorRelated2 = p1PrimaryColor;
        m_player1->m_flashRelated3 = p1SecondaryColor;
        m_player2->m_colorRelated2 = p2PrimaryColor;
        m_player2->m_flashRelated3 = p2SecondaryColor;
        */

    }
};
