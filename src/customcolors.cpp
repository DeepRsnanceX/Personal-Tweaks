#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

auto theMod = Mod::get();

// misc settings
auto useCustomColors = theMod->getSettingValue<bool>("enable-customcolors");
auto tintTrail = theMod->getSettingValue<bool>("tint-trail");
auto tintGhostTrail = theMod->getSettingValue<bool>("tint-ghost-trail");
auto tintDashFire = theMod->getSettingValue<bool>("tint-dashfire");
// player 1 color settings
auto p1PrimaryColor = theMod->getSettingValue<cocos2d::ccColor3B>("p1-primary");
auto p1SecondaryColor = theMod->getSettingValue<cocos2d::ccColor3B>("p1-secondary");
auto p1WaveColor = theMod->getSettingValue<cocos2d::ccColor3B>("p1-wave");
auto p1TrailColor = theMod->getSettingValue<cocos2d::ccColor3B>("p1-trail");
auto p1GhostTrailColor = theMod->getSettingValue<cocos2d::ccColor3B>("p1-ghost-trail");
auto p1DashFireColor = theMod->getSettingValue<cocos2d::ccColor3B>("p1-dashfire");
// player 2 color settings
auto p2PrimaryColor = theMod->getSettingValue<cocos2d::ccColor3B>("p2-primary");
auto p2SecondaryColor = theMod->getSettingValue<cocos2d::ccColor3B>("p2-secondary");
auto p2WaveColor = theMod->getSettingValue<cocos2d::ccColor3B>("p2-wave");
auto p2TrailColor = theMod->getSettingValue<cocos2d::ccColor3B>("p2-trail");
auto p2GhostTrailColor = theMod->getSettingValue<cocos2d::ccColor3B>("p2-ghost-trail");
auto p2DashFireColor = theMod->getSettingValue<cocos2d::ccColor3B>("p2-dashfire");
// particle settings
auto overrideAllVariances = theMod->getSettingValue<bool>("override-all-variances");
auto variancesOverride = theMod->getSettingValue<cocos2d::ccColor4B>("variances-override");
// particle colors and settings
auto tintMainParticles = theMod->getSettingValue<bool>("tint-mainparticles");
auto p1MainParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("p1-main-particles-start");
auto p1MainParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("p1-main-particles-end");
auto p2MainParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("p2-main-particles-start");
auto p2MainParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("p2-main-particles-end");
auto tintUfoClickParticles = theMod->getSettingValue<bool>("tint-ufo-click-particles");
auto p1UfoClickParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("p1-ufo-click-particles-start");
auto p1UfoClickParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("p1-ufo-click-particles-end");
auto p2UfoClickParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("p2-ufo-click-particles-start");
auto p2UfoClickParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("p2-ufo-click-particles-end");
auto tintRobotJumpParticles = theMod->getSettingValue<bool>("tint-robot-jump-particles");
auto robotJumpParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("robot-jump-particles-start");
auto robotJumpParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("robot-jump-particles-end");
auto tintSwingBurstParticles = theMod->getSettingValue<bool>("tint-swing-burst-particles");
auto swingBurstParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("swing-burst-particles-start");
auto swingBurstParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("swing-burst-particles-end");

$on_mod(Loaded) {
    // misc settings
    listenForSettingChanges("tint-trail", [](bool value) {
        tintTrail = value;
    });
    listenForSettingChanges("enable-customcolors", [](bool value) {
        useCustomColors = value;
    });
    listenForSettingChanges("tint-ghost-trail", [](bool value) {
        tintGhostTrail = value;
    });
    listenForSettingChanges("tint-dashfire", [](bool value) {
        tintDashFire = value;
    });
    // p1 colors
    listenForSettingChanges("p1-primary", [](cocos2d::ccColor3B value) {
        p1PrimaryColor = value;
    });
    listenForSettingChanges("p1-secondary", [](cocos2d::ccColor3B value) {
        p1SecondaryColor = value;
    });
    listenForSettingChanges("p1-wave", [](cocos2d::ccColor3B value) {
        p1WaveColor = value;
    });
    listenForSettingChanges("p1-trail", [](cocos2d::ccColor3B value) {
        p1TrailColor = value;
    });
    listenForSettingChanges("p1-ghost-trail", [](cocos2d::ccColor3B value) {
        p1GhostTrailColor = value;
    });
    listenForSettingChanges("p1-dashfire", [](cocos2d::ccColor3B value) {
        p1DashFireColor = value;
    });
    // p2 colors
    listenForSettingChanges("p2-primary", [](cocos2d::ccColor3B value) {
        p2PrimaryColor = value;
    });
    listenForSettingChanges("p2-secondary", [](cocos2d::ccColor3B value) {
        p2SecondaryColor = value;
    });
    listenForSettingChanges("p2-wave", [](cocos2d::ccColor3B value) {
        p2WaveColor = value;
    });
    listenForSettingChanges("p2-trail", [](cocos2d::ccColor3B value) {
        p2TrailColor = value;
    });
    listenForSettingChanges("p2-ghost-trail", [](cocos2d::ccColor3B value) {
        p2GhostTrailColor = value;
    });
    listenForSettingChanges("p2-dashfire", [](cocos2d::ccColor3B value) {
        p2DashFireColor = value;
    });
    // particle settings
    listenForSettingChanges("override-all-variances", [](bool value) {
        overrideAllVariances = value;
    });
    listenForSettingChanges("variances-override", [](cocos2d::ccColor4B value) {
        variancesOverride = value;
    });
    // particle colors and settings
    listenForSettingChanges("tint-mainparticles", [](bool value) {
        tintMainParticles = value;
    });
    listenForSettingChanges("p1-main-particles-start", [](cocos2d::ccColor4B value) {
        p1MainParticlesStart = value;
    });
    listenForSettingChanges("p1-main-particles-end", [](cocos2d::ccColor4B value) {
        p1MainParticlesEnd = value;
    });
    listenForSettingChanges("p2-main-particles-start", [](cocos2d::ccColor4B value) {
        p2MainParticlesStart = value;
    });
    listenForSettingChanges("p2-main-particles-end", [](cocos2d::ccColor4B value) {
        p2MainParticlesEnd = value;
    });
    listenForSettingChanges("tint-ufo-click-particles", [](bool value) {
        tintUfoClickParticles = value;
    });
    listenForSettingChanges("p1-ufo-click-particles-start", [](cocos2d::ccColor4B value) {
        p1UfoClickParticlesStart = value;
    });
    listenForSettingChanges("p1-ufo-click-particles-end", [](cocos2d::ccColor4B value) {
        p1UfoClickParticlesEnd = value;
    });
    listenForSettingChanges("p2-ufo-click-particles-start", [](cocos2d::ccColor4B value) {
        p2UfoClickParticlesStart = value;
    });
    listenForSettingChanges("p2-ufo-click-particles-end", [](cocos2d::ccColor4B value) {
        p2UfoClickParticlesEnd = value;
    });
}

class $modify(ColorsPlayer, PlayerObject) {
    struct Fields{
        // our settings use cocos2d::ccColor3B and 4B, but we need to convert them to ccColor4F for particles
        cocos2d::ccColor4F p1PrimaryParticleStart = cocos2d::ccColor4F(p1PrimaryColor.r / 255.0f, p1PrimaryColor.g / 255.0f, p1PrimaryColor.b / 255.0f, 255.f);
        cocos2d::ccColor4F p1PrimaryParticleEnd = cocos2d::ccColor4F(p1PrimaryColor.r / 255.0f, p1PrimaryColor.g / 255.0f, p1PrimaryColor.b / 255.0f, 0.f);
        cocos2d::ccColor4F p1SecondaryParticleStart = cocos2d::ccColor4F(p1SecondaryColor.r / 255.0f, p1SecondaryColor.g / 255.0f, p1SecondaryColor.b / 255.0f, 255.f);
        cocos2d::ccColor4F p1SecondaryParticleEnd = cocos2d::ccColor4F(p1SecondaryColor.r / 255.0f, p1SecondaryColor.g / 255.0f, p1SecondaryColor.b / 255.0f, 0.f);
        cocos2d::ccColor4F p1DashFireParticleStart = cocos2d::ccColor4F(p1DashFireColor.r / 255.0f, p1DashFireColor.g / 255.0f, p1DashFireColor.b / 255.0f, 255.f);
        cocos2d::ccColor4F p1DashFireParticleEnd = cocos2d::ccColor4F(p1DashFireColor.r / 255.0f, p1DashFireColor.g / 255.0f, p1DashFireColor.b / 255.0f, 0.f);

        cocos2d::ccColor4F p2PrimaryParticleStart = cocos2d::ccColor4F(p2PrimaryColor.r / 255.0f, p2PrimaryColor.g / 255.0f, p2PrimaryColor.b / 255.0f, 255.f);
        cocos2d::ccColor4F p2PrimaryParticleEnd = cocos2d::ccColor4F(p2PrimaryColor.r / 255.0f, p2PrimaryColor.g / 255.0f, p2PrimaryColor.b / 255.0f, 0.f);
        cocos2d::ccColor4F p2SecondaryParticleStart = cocos2d::ccColor4F(p2SecondaryColor.r / 255.0f, p2SecondaryColor.g / 255.0f, p2SecondaryColor.b / 255.0f, 255.f);
        cocos2d::ccColor4F p2SecondaryParticleEnd = cocos2d::ccColor4F(p2SecondaryColor.r / 255.0f, p2SecondaryColor.g / 255.0f, p2SecondaryColor.b / 255.0f, 0.f);
        cocos2d::ccColor4F p2DashFireParticleStart = cocos2d::ccColor4F(p2DashFireColor.r / 255.0f, p2DashFireColor.g / 255.0f, p2DashFireColor.b / 255.0f, 255.f);
        cocos2d::ccColor4F p2DashFireParticleEnd = cocos2d::ccColor4F(p2DashFireColor.r / 255.0f, p2DashFireColor.g / 255.0f, p2DashFireColor.b / 255.0f, 0.f);

        // particle colors
        cocos2d::ccColor4F p1MainParticleStart = cocos2d::ccColor4F(p1MainParticlesStart.r / 255.0f, p1MainParticlesStart.g / 255.0f, p1MainParticlesStart.b / 255.0f, p1MainParticlesStart.a / 255.0f);
        cocos2d::ccColor4F p1MainParticleEnd = cocos2d::ccColor4F(p1MainParticlesEnd.r / 255.0f, p1MainParticlesEnd.g / 255.0f, p1MainParticlesEnd.b / 255.0f, p1MainParticlesEnd.a / 255.0f);
        cocos2d::ccColor4F p2MainParticleStart = cocos2d::ccColor4F(p2MainParticlesStart.r / 255.0f, p2MainParticlesStart.g / 255.0f, p2MainParticlesStart.b / 255.0f, p2MainParticlesStart.a / 255.0f);
        cocos2d::ccColor4F p2MainParticleEnd = cocos2d::ccColor4F(p2MainParticlesEnd.r / 255.0f, p2MainParticlesEnd.g / 255.0f, p2MainParticlesEnd.b / 255.0f, p2MainParticlesEnd.a / 255.0f);
        cocos2d::ccColor4F p1UfoClickParticleStart = cocos2d::ccColor4F(p1UfoClickParticlesStart.r / 255.0f, p1UfoClickParticlesStart.g / 255.0f, p1UfoClickParticlesStart.b / 255.0f, p1UfoClickParticlesStart.a / 255.0f);
        cocos2d::ccColor4F p1UfoClickParticleEnd = cocos2d::ccColor4F(p1UfoClickParticlesEnd.r / 255.0f, p1UfoClickParticlesEnd.g / 255.0f, p1UfoClickParticlesEnd.b / 255.0f, p1UfoClickParticlesEnd.a / 255.0f);
        cocos2d::ccColor4F p2UfoClickParticleStart = cocos2d::ccColor4F(p2UfoClickParticlesStart.r / 255.0f, p2UfoClickParticlesStart.g / 255.0f, p2UfoClickParticlesStart.b / 255.0f, p2UfoClickParticlesStart.a / 255.0f);
        cocos2d::ccColor4F p2UfoClickParticleEnd = cocos2d::ccColor4F(p2UfoClickParticlesEnd.r / 255.0f, p2UfoClickParticlesEnd.g / 255.0f, p2UfoClickParticlesEnd.b / 255.0f, p2UfoClickParticlesEnd.a / 255.0f);
        // uh
        cocos2d::ccColor4F varianceOverrider = cocos2d::ccColor4F(variancesOverride.r / 255.0f, variancesOverride.g / 255.0f, variancesOverride.b / 255.0f, variancesOverride.a / 255.0f);
    };

    void updateMyParticleColors() {
        auto fields = m_fields.self();

        fields->p1PrimaryParticleStart = cocos2d::ccColor4F(p1PrimaryColor.r / 255.0f, p1PrimaryColor.g / 255.0f, p1PrimaryColor.b / 255.0f, 255.f);
        fields->p1PrimaryParticleEnd = cocos2d::ccColor4F(p1PrimaryColor.r / 255.0f, p1PrimaryColor.g / 255.0f, p1PrimaryColor.b / 255.0f, 0.f);
        fields->p1SecondaryParticleStart = cocos2d::ccColor4F(p1SecondaryColor.r / 255.0f, p1SecondaryColor.g / 255.0f, p1SecondaryColor.b / 255.0f, 255.f);
        fields->p1SecondaryParticleEnd = cocos2d::ccColor4F(p1SecondaryColor.r / 255.0f, p1SecondaryColor.g / 255.0f, p1SecondaryColor.b / 255.0f, 0.f);
        fields->p1DashFireParticleStart = cocos2d::ccColor4F(p1DashFireColor.r / 255.0f, p1DashFireColor.g / 255.0f, p1DashFireColor.b / 255.0f, 255.f);
        fields->p1DashFireParticleEnd = cocos2d::ccColor4F(p1DashFireColor.r / 255.0f, p1DashFireColor.g / 255.0f, p1DashFireColor.b / 255.0f, 0.f);

        fields->p2PrimaryParticleStart = cocos2d::ccColor4F(p2PrimaryColor.r / 255.0f, p2PrimaryColor.g / 255.0f, p2PrimaryColor.b / 255.0f, 255.f);
        fields->p2PrimaryParticleEnd = cocos2d::ccColor4F(p2PrimaryColor.r / 255.0f, p2PrimaryColor.g / 255.0f, p2PrimaryColor.b / 255.0f, 0.f);
        fields->p2SecondaryParticleStart = cocos2d::ccColor4F(p2SecondaryColor.r / 255.0f, p2SecondaryColor.g / 255.0f, p2SecondaryColor.b / 255.0f, 255.f);
        fields->p2SecondaryParticleEnd = cocos2d::ccColor4F(p2SecondaryColor.r / 255.0f, p2SecondaryColor.g / 255.0f, p2SecondaryColor.b / 255.0f, 0.f);
        fields->p2DashFireParticleStart = cocos2d::ccColor4F(p2DashFireColor.r / 255.0f, p2DashFireColor.g / 255.0f, p2DashFireColor.b / 255.0f, 255.f);
        fields->p2DashFireParticleEnd = cocos2d::ccColor4F(p2DashFireColor.r / 255.0f, p2DashFireColor.g / 255.0f, p2DashFireColor.b / 255.0f, 0.f);

        // particle colors
        fields->p1MainParticleStart = cocos2d::ccColor4F(p1MainParticlesStart.r / 255.0f, p1MainParticlesStart.g / 255.0f, p1MainParticlesStart.b / 255.0f, p1MainParticlesStart.a / 255.0f);
        fields->p1MainParticleEnd = cocos2d::ccColor4F(p1MainParticlesEnd.r / 255.0f, p1MainParticlesEnd.g / 255.0f, p1MainParticlesEnd.b / 255.0f, p1MainParticlesEnd.a / 255.0f);
        fields->p2MainParticleStart = cocos2d::ccColor4F(p2MainParticlesStart.r / 255.0f, p2MainParticlesStart.g / 255.0f, p2MainParticlesStart.b / 255.0f, p2MainParticlesStart.a / 255.0f);
        fields->p2MainParticleEnd = cocos2d::ccColor4F(p2MainParticlesEnd.r / 255.0f, p2MainParticlesEnd.g / 255.0f, p2MainParticlesEnd.b / 255.0f, p2MainParticlesEnd.a / 255.0f);
        fields->p1UfoClickParticleStart = cocos2d::ccColor4F(p1UfoClickParticlesStart.r / 255.0f, p1UfoClickParticlesStart.g / 255.0f, p1UfoClickParticlesStart.b / 255.0f, p1UfoClickParticlesStart.a / 255.0f);
        fields->p1UfoClickParticleEnd = cocos2d::ccColor4F(p1UfoClickParticlesEnd.r / 255.0f, p1UfoClickParticlesEnd.g / 255.0f, p1UfoClickParticlesEnd.b / 255.0f, p1UfoClickParticlesEnd.a / 255.0f);
        fields->p2UfoClickParticleStart = cocos2d::ccColor4F(p2UfoClickParticlesStart.r / 255.0f, p2UfoClickParticlesStart.g / 255.0f, p2UfoClickParticlesStart.b / 255.0f, p2UfoClickParticlesStart.a / 255.0f);
        fields->p2UfoClickParticleEnd = cocos2d::ccColor4F(p2UfoClickParticlesEnd.r / 255.0f, p2UfoClickParticlesEnd.g / 255.0f, p2UfoClickParticlesEnd.b / 255.0f, p2UfoClickParticlesEnd.a / 255.0f);
        // uh
        fields->varianceOverrider = cocos2d::ccColor4F(variancesOverride.r / 255.0f, variancesOverride.g / 255.0f, variancesOverride.b / 255.0f, variancesOverride.a / 255.0f);
    }

    void update(float p0) {
        PlayerObject::update(p0);

        auto f = m_fields.self();

        if (!useCustomColors) return;

        // ----------- PLAYER 1 -----------------

        if (overrideAllVariances) {
            // override all variances
            m_playerGroundParticles->m_tStartColorVar = f->varianceOverrider;
            m_playerGroundParticles->m_tEndColorVar = f->varianceOverrider;
            m_landParticles0->m_tStartColorVar = f->varianceOverrider;
            m_landParticles0->m_tEndColorVar = f->varianceOverrider;
            m_landParticles1->m_tStartColorVar = f->varianceOverrider;
            m_landParticles1->m_tEndColorVar = f->varianceOverrider;
            m_vehicleGroundParticles->m_tStartColorVar = f->varianceOverrider;
            m_vehicleGroundParticles->m_tEndColorVar = f->varianceOverrider;
            m_ufoClickParticles->m_tStartColorVar = f->varianceOverrider;
            m_ufoClickParticles->m_tEndColorVar = f->varianceOverrider;
        }

        // particles
        if (tintMainParticles) {
            m_playerGroundParticles->m_tStartColor = f->p1MainParticleStart;
            m_playerGroundParticles->m_tEndColor = f->p1MainParticleEnd;
            m_landParticles0->m_tStartColor = f->p1MainParticleStart;
            m_landParticles0->m_tEndColor = f->p1MainParticleEnd;
            m_landParticles1->m_tStartColor = f->p1MainParticleStart;
            m_landParticles1->m_tEndColor = f->p1MainParticleEnd;
            m_vehicleGroundParticles->m_tStartColor = f->p1MainParticleStart;
            m_vehicleGroundParticles->m_tEndColor = f->p1MainParticleEnd;
        } else {
            m_playerGroundParticles->m_tStartColor = f->p1PrimaryParticleStart;
            m_playerGroundParticles->m_tEndColor = f->p1PrimaryParticleEnd;
            m_landParticles0->m_tStartColor = f->p1PrimaryParticleStart;
            m_landParticles0->m_tEndColor = f->p1PrimaryParticleEnd;
            m_landParticles1->m_tStartColor = f->p1PrimaryParticleStart;
            m_landParticles1->m_tEndColor = f->p1PrimaryParticleEnd;
            m_vehicleGroundParticles->m_tStartColor = f->p1PrimaryParticleStart;
            m_vehicleGroundParticles->m_tEndColor = f->p1PrimaryParticleEnd;
        }

        if (tintUfoClickParticles) {
            m_ufoClickParticles->m_tStartColor = f->p1UfoClickParticleStart;
            m_ufoClickParticles->m_tEndColor = f->p1UfoClickParticleEnd;
        } else {
            m_ufoClickParticles->m_tStartColor = f->p1PrimaryParticleStart;
            m_ufoClickParticles->m_tEndColor = f->p1PrimaryParticleEnd;
        }

        // icon effects
        m_iconSprite->setColor(p1PrimaryColor);
        m_iconSpriteSecondary->setColor(p1SecondaryColor);
        m_vehicleSprite->setColor(p1PrimaryColor);
        m_vehicleSpriteSecondary->setColor(p1SecondaryColor);
        if (m_waveTrail) {
            m_waveTrail->setColor(p1WaveColor);
        }
        if (m_regularTrail) {
            if (tintTrail) {
                m_regularTrail->setColor(p1TrailColor);
            }
        }

        if (m_dashFireSprite) {
            if (tintDashFire) {
                m_dashFireSprite->setColor(p1DashFireColor);
                m_dashParticles->m_tStartColor = f->p1DashFireParticleStart;
                m_dashParticles->m_tEndColor = f->p1DashFireParticleEnd;
            } else {
                m_dashFireSprite->setColor(p1PrimaryColor);
                m_dashParticles->m_tStartColor = f->p1PrimaryParticleStart;
                m_dashParticles->m_tEndColor = f->p1PrimaryParticleEnd;
            }
        }
        
        if (m_ghostTrail) {
            if (tintGhostTrail) {
                m_ghostTrail->m_color = p1GhostTrailColor;
            } else {
                m_ghostTrail->m_color = p1PrimaryColor;
            }
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
        
        // ----------- PLAYER 2 -----------------

        // ...existing code...
        // ----------- PLAYER 2 -----------------

        if (m_isSecondPlayer) {

            // particles
            if (tintMainParticles) {
                m_playerGroundParticles->m_tStartColor = f->p2MainParticleStart;
                m_playerGroundParticles->m_tEndColor = f->p2MainParticleEnd;
                m_landParticles0->m_tStartColor = f->p2MainParticleStart;
                m_landParticles0->m_tEndColor = f->p2MainParticleEnd;
                m_landParticles1->m_tStartColor = f->p2MainParticleStart;
                m_landParticles1->m_tEndColor = f->p2MainParticleEnd;
                m_vehicleGroundParticles->m_tStartColor = f->p2MainParticleStart;
                m_vehicleGroundParticles->m_tEndColor = f->p2MainParticleEnd;
            } else {
                m_playerGroundParticles->m_tStartColor = f->p2PrimaryParticleStart;
                m_playerGroundParticles->m_tEndColor = f->p2PrimaryParticleEnd;
                m_landParticles0->m_tStartColor = f->p2PrimaryParticleStart;
                m_landParticles0->m_tEndColor = f->p2PrimaryParticleEnd;
                m_landParticles1->m_tStartColor = f->p2PrimaryParticleStart;
                m_landParticles1->m_tEndColor = f->p2PrimaryParticleEnd;
                m_vehicleGroundParticles->m_tStartColor = f->p2PrimaryParticleStart;
                m_vehicleGroundParticles->m_tEndColor = f->p2PrimaryParticleEnd;
            }

            if (tintUfoClickParticles) {
                m_ufoClickParticles->m_tStartColor = f->p2UfoClickParticleStart;
                m_ufoClickParticles->m_tEndColor = f->p2UfoClickParticleEnd;
            } else {
                m_ufoClickParticles->m_tStartColor = f->p2PrimaryParticleStart;
                m_ufoClickParticles->m_tEndColor = f->p2PrimaryParticleEnd;
            }

            // icon effects
            m_iconSprite->setColor(p2PrimaryColor);
            m_iconSpriteSecondary->setColor(p2SecondaryColor);
            m_vehicleSprite->setColor(p2PrimaryColor);
            m_vehicleSpriteSecondary->setColor(p2SecondaryColor);
            if (m_waveTrail) {
                m_waveTrail->setColor(p2WaveColor);
            }
            if (m_regularTrail) {
                if (tintTrail) {
                    m_regularTrail->setColor(p2TrailColor);
                }
            }

            if (m_dashFireSprite) {
                if (tintDashFire) {
                    m_dashFireSprite->setColor(p2DashFireColor);
                    m_dashParticles->m_tStartColor = f->p2DashFireParticleStart;
                    m_dashParticles->m_tEndColor = f->p2DashFireParticleEnd;
                } else {
                    m_dashFireSprite->setColor(p2PrimaryColor);
                    m_dashParticles->m_tStartColor = f->p2PrimaryParticleStart;
                    m_dashParticles->m_tEndColor = f->p2PrimaryParticleEnd;
                }
            }

            if (m_ghostTrail) {
                if (tintGhostTrail) {
                    m_ghostTrail->m_color = p2GhostTrailColor;
                } else {
                    m_ghostTrail->m_color = p2PrimaryColor;
                }
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

class $modify(ColorsPlayLayer, PlayLayer) {
    void resume() {
        PlayLayer::resume();
        static_cast<ColorsPlayer*>(m_player1)->updateMyParticleColors();
    }

    void resumeAndRestart(bool p0) {
        PlayLayer::resumeAndRestart(p0);
        static_cast<ColorsPlayer*>(m_player1)->updateMyParticleColors();
    }
};
