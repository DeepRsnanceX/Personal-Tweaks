#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

auto mod = Mod::get();

bool doCol1 = mod->getSettingValue<bool>("rgb-col1");
bool doCol2 = mod->getSettingValue<bool>("rgb-col2");

bool BMI = mod->getSettingValue<bool>("better-immersion-mode");

float rgbSpeed = mod->getSettingValue<double>("rgb-speed");

float sat = mod->getSettingValue<double>("rgb-saturation");
float sat2 = mod->getSettingValue<double>("rgb-saturation2");

float bright1 = mod->getSettingValue<double>("rgb-brightness1");
float bright2 = mod->getSettingValue<double>("rgb-brightness2");

bool doWaveColr = mod->getSettingValue<bool>("rgb-wave");
bool doTrailColr = mod->getSettingValue<bool>("rgb-trail");
bool doDashColr = mod->getSettingValue<bool>("rgb-dash");

float p2Distance = mod->getSettingValue<double>("p2-distance");

bool ignoreP2 = mod->getSettingValue<bool>("ignore-p2");

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
    listenForSettingChanges("rgb-brightness1", [](double value) {
        bright1 = value;
    });
    listenForSettingChanges("rgb-brightness2", [](double value) {
        bright2 = value;
    });
    listenForSettingChanges("p2-distance", [](double value) {
        p2Distance = value;
    });
    listenForSettingChanges("better-immersion-mode", [](bool value) {
        BMI = value;
    });
    listenForSettingChanges("rgb-wave", [](bool value) {
        doWaveColr = value;
    });
    listenForSettingChanges("rgb-trail", [](bool value) {
        doTrailColr = value;
    });
    listenForSettingChanges("rgb-dash", [](bool value) {
        doDashColr = value;
    });
}

class $modify(RGBPlayerObject, PlayerObject) {
    struct Fields {
        bool usingCol1 = doCol1;
        bool usingCol2 = doCol2;
        bool usingBMI = BMI;
        bool usingWaveColr = doWaveColr;
        bool usingTrailColr = doTrailColr;
        bool usingDashColr = doDashColr;
        bool isIgnoringP2 = ignoreP2;
        float rgbTimer = 0.f;
    };

    void updateMyRGBSettings() {
        auto fld = m_fields.self();
        fld->usingCol1 = doCol1;
        fld->usingCol2 = doCol2;
        fld->usingBMI = BMI;
        fld->usingWaveColr = doWaveColr;
        fld->usingTrailColr = doTrailColr;
        fld->usingDashColr = doDashColr;
        fld->isIgnoringP2 = ignoreP2;
    }
    
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
        if (fld->isIgnoringP2 && m_isSecondPlayer) return;

        float cycleDuration = 64.0f / rgbSpeed;

        fld->rgbTimer += p0;

        float hue1 = fmod(fld->rgbTimer / cycleDuration, 1.f);
        float hue2 = fmod(hue1 + p2Distance, 1.f);

        auto [r, g, b] = hsvToRgbRaw(hue1, sat, bright1);
        auto [r2, g2, b2] = hsvToRgbRaw(hue2, sat2, bright2);

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

        if (m_waveTrail && fld->usingWaveColr) {
            m_waveTrail->setColor({static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});
        }

        if (m_dashFireSprite && fld->usingDashColr) {
            m_dashFireSprite->setColor({static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});
            m_dashParticles->m_tStartColor = colorForParticle;
            m_dashParticles->m_tEndColor = colorForParticle;
        }

        if (m_regularTrail && fld->usingTrailColr) {
            m_regularTrail->setColor({static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});
        }

        // -----------------------------
        // color 2 hehe
        // -----------------------------
        
        if (fld->usingCol2) {
            m_playerColor2 = ccColor3B({static_cast<GLubyte>(r2), static_cast<GLubyte>(g2), static_cast<GLubyte>(b2)});

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

        // immersion mode
        if (fld->usingBMI) {
            if (m_isShip) {
                m_trailingParticles->m_tStartColor = colorForParticle;
                m_trailingParticles->m_tEndColor = colorForParticle;
                m_shipClickParticles->m_tStartColor = colorForParticle;
                m_shipClickParticles->m_tEndColor = colorForParticle;
            } else if (m_isRobot) {
                m_robotBurstParticles->m_tStartColor = colorForParticle;
                m_robotBurstParticles->m_tEndColor = colorForParticle;
            } else if (m_isSwing) {
                m_swingBurstParticles1->m_tStartColor = colorForParticle;
                m_swingBurstParticles1->m_tEndColor = colorForParticle;
                m_swingBurstParticles2->m_tStartColor = colorForParticle;
                m_swingBurstParticles2->m_tEndColor = colorForParticle;
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
