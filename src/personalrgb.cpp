#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

auto mod = Mod::get();

bool doCol1 = mod->getSettingValue<bool>("rgb-col1");
bool doCol2 = mod->getSettingValue<bool>("rgb-col2");
bool ignoreP2 = mod->getSettingValue<bool>("ignore-p2");
float sat = mod->getSettingValue<double>("rgb-saturation");
float sat2 = mod->getSettingValue<double>("rgb-saturation2");
float rgbSpeed = mod->getSettingValue<double>("rgb-speed");

$on_mod(Loaded){
    listenForSettingChanges("rgb-col1", [](bool value) {
        doCol1 = value;
    });
    listenForSettingChanges("rgb-col2", [](bool value) {
        doCol2 = value;
    });
    listenForSettingChanges("ignore-p2", [](bool value) {
        ignoreP2 = value;
    });
    listenForSettingChanges("rgb-saturation", [](double value) {
        sat = value;
    });
    listenForSettingChanges("rgb-saturation2", [](double value) {
        sat2 = value;
    });
    listenForSettingChanges("rgb-speed", [](double value) {
        rgbSpeed = value;
    });
}

class $modify(RGBPlayerObject, PlayerObject) {
    struct Fields {
        bool usingCol1 = doCol1;
        bool usingCol2 = doCol2;
        float rgbTimer = 0.f;
    };
    
    std::tuple<int, int, int> hsvToRgbRaw(float h, float s, float v) {
        float r, g, b;
        int i = int(h * 6);
        float f = h * 6 - i;
        float p = v * (1 - s);
        float q = v * (1 - f * s);
        float t = v * (1 - (1 - f) * s);

        switch (i % 6) {
            case 0: r = v, g = t, b = p; break;
            case 1: r = q, g = v, b = p; break;
            case 2: r = p, g = v, b = t; break;
            case 3: r = p, g = q, b = v; break;
            case 4: r = t, g = p, b = v; break;
            case 5: r = v, g = p, b = q; break;
        }
        return {
            static_cast<int>(r * 255),
            static_cast<int>(g * 255),
            static_cast<int>(b * 255)
        };
    }

    void update(float p0) {
        PlayerObject::update(p0);

        auto fld = m_fields.self();
        auto playLayer = PlayLayer::get();
        if (!playLayer) return;
        if (ignoreP2 && m_isSecondPlayer) return;

        float cycleDuration = 64.0f / rgbSpeed;

        fld->rgbTimer += p0;

        float hue1 = fmod(fld->rgbTimer / cycleDuration, 1.f);
        float hue2 = fmod(hue1 + 0.5f, 1.f);

        auto [r, g, b] = hsvToRgbRaw(hue1, sat, 1.f);
        auto [r2, g2, b2] = hsvToRgbRaw(hue2, sat2, 1.f);

        cocos2d::ccColor4F colorForParticle({
            r / 255.0f,
            g / 255.0f,
            b / 255.0f,
            1.0f
        });

        cocos2d::ccColor4F colorForParticle2({
            r2 / 255.0f,
            g2 / 255.0f,
            b2 / 255.0f,
            1.0f
        });

        if (fld->usingCol1) {
            m_playerColor1 = ccColor3B({static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});

            m_iconSprite->setColor({static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});
            m_vehicleSprite->setColor({static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});
            if (m_isRobot) {
                m_robotSprite->m_color = ccColor3B({static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});
            } else if (m_isSpider) {
                m_spiderSprite->m_color = ccColor3B({static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});
            }
            
            if (m_ghostTrail) {
                m_ghostTrail->m_color = ccColor3B({static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});
            }
            if (m_waveTrail) {
                m_waveTrail->setColor({static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});
            }

            if (m_dashFireSprite) {
                m_dashFireSprite->setColor({static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});
                m_dashParticles->m_tStartColor = colorForParticle;
                m_dashParticles->m_tEndColor = colorForParticle;
            }

            //particles bc other mods don't do them lol
            m_playerGroundParticles->m_tStartColor = colorForParticle;
            m_playerGroundParticles->m_tEndColor = colorForParticle;
            m_vehicleGroundParticles->m_tStartColor = colorForParticle;
            m_vehicleGroundParticles->m_tEndColor = colorForParticle;
            m_landParticles0->m_tStartColor = colorForParticle;
            m_landParticles0->m_tEndColor = colorForParticle;
            m_landParticles1->m_tStartColor = colorForParticle;
            m_landParticles1->m_tEndColor = colorForParticle;
            m_ufoClickParticles->m_tStartColor = colorForParticle;
            m_ufoClickParticles->m_tEndColor = colorForParticle;
        }
        
        if (fld->usingCol2) {
            m_playerColor2 = ccColor3B({static_cast<GLubyte>(r2), static_cast<GLubyte>(g2), static_cast<GLubyte>(b2));

            m_iconSpriteSecondary->setColor({static_cast<GLubyte>(r2), static_cast<GLubyte>(g2), static_cast<GLubyte>(b2)});
            m_vehicleSpriteSecondary->setColor({static_cast<GLubyte>(r2), static_cast<GLubyte>(g2), static_cast<GLubyte>(b2)});
            if (m_isRobot) {
                m_robotSprite->m_secondColor = ccColor3B({static_cast<GLubyte>(r2), static_cast<GLubyte>(g2), static_cast<GLubyte>(b2)});
            } else if (m_isSpider) {
                m_spiderSprite->m_secondColor = ccColor3B({static_cast<GLubyte>(r2), static_cast<GLubyte>(g2), static_cast<GLubyte>(b2)});
            } else if (m_isBird) {
                m_trailingParticles->m_tStartColor = colorForParticle2;
                m_trailingParticles->m_tEndColor = colorForParticle2;
            }
        }
        
        if (m_isRobot) {
            if (fld->usingCol1 || fld->usingCol2) {
                m_robotSprite->updateColors();
            }
        } else if (m_isSpider) {
            if (fld->usingCol1 || fld->usingCol2) {
                m_spiderSprite->updateColors();
            }
        }
    }
};

class $modify(RGBPlayLayer, PlayLayer) {
    void resume() {
        PlayLayer::resume();
    }

    void resumeAndRestart(bool p0) {
        PlayLayer::resumeAndRestart(p0);
    }
};
