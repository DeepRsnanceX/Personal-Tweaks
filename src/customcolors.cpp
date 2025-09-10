#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Helper function to create a ccColor4F from a ccColor3B
cocos2d::ccColor4F toColor4F(const cocos2d::ccColor3B& color3b, float alpha = 1.0f) {
    return {color3b.r / 255.f, color3b.g / 255.f, color3b.b / 255.f, alpha};
}

// Helper function to create a ccColor4F from a ccColor4B
cocos2d::ccColor4F toColor4F(const cocos2d::ccColor4B& color4b) {
    return {color4b.r / 255.f, color4b.g / 255.f, color4b.b / 255.f, color4b.a / 255.f};
}

auto theMod = Mod::get();

// General settings
bool useCustomColors;
bool overrideAllVariances;
cocos2d::ccColor4B variancesOverride;

// P1 Color Settings
cocos2d::ccColor3B p1PrimaryColor, p1SecondaryColor, p1WaveColor, p1TrailColor, p1GhostTrailColor, p1DashFireColor, p1UfoDomeColor;
bool p1TintWave, p1TintTrail, p1TintGhostTrail, p1TintDashFire, p1TintUfoDome;

// P2 Color Settings
cocos2d::ccColor3B p2PrimaryColor, p2SecondaryColor, p2WaveColor, p2TrailColor, p2GhostTrailColor, p2DashFireColor, p2UfoDomeColor;
bool p2TintWave, p2TintTrail, p2TintGhostTrail, p2TintDashFire, p2TintUfoDome;

// Particle Settings
bool tintMainParticlesP1, tintMainParticlesP2, tintUfoClickParticlesP1, tintUfoClickParticlesP2;
cocos2d::ccColor4B p1MainParticlesStart, p1MainParticlesEnd, p2MainParticlesStart, p2MainParticlesEnd;
cocos2d::ccColor4B p1UfoClickParticlesStart, p1UfoClickParticlesEnd, p2UfoClickParticlesStart, p2UfoClickParticlesEnd;

// Macro to simplify listening for setting changes
#define LISTEN_SETTING(key, var) \
    listenForSettingChanges(key, [](auto value) { var = value; }); \
    var = theMod->getSettingValue<decltype(var)>(key)

$on_mod(Loaded) {
    // Load all settings
    LISTEN_SETTING("enable-customcolors", useCustomColors);
    LISTEN_SETTING("override-all-variances", overrideAllVariances);
    LISTEN_SETTING("variances-override", variancesOverride);

    // Player 1
    LISTEN_SETTING("p1-primary", p1PrimaryColor);
    LISTEN_SETTING("p1-secondary", p1SecondaryColor);
    LISTEN_SETTING("p1-wave", p1WaveColor);
    LISTEN_SETTING("p1-trail", p1TrailColor);
    LISTEN_SETTING("p1-ghost-trail", p1GhostTrailColor);
    LISTEN_SETTING("p1-dashfire", p1DashFireColor);
    LISTEN_SETTING("p1-ufodome", p1UfoDomeColor);
    LISTEN_SETTING("p1-tint-wave", p1TintWave);
    LISTEN_SETTING("p1-tint-trail", p1TintTrail);
    LISTEN_SETTING("p1-tint-ghost-trail", p1TintGhostTrail);
    LISTEN_SETTING("p1-tint-dashfire", p1TintDashFire);
    LISTEN_SETTING("p1-tint-ufodome", p1TintUfoDome);
    
    // Player 2
    LISTEN_SETTING("p2-primary", p2PrimaryColor);
    LISTEN_SETTING("p2-secondary", p2SecondaryColor);
    LISTEN_SETTING("p2-wave", p2WaveColor);
    LISTEN_SETTING("p2-trail", p2TrailColor);
    LISTEN_SETTING("p2-ghost-trail", p2GhostTrailColor);
    LISTEN_SETTING("p2-dashfire", p2DashFireColor);
    LISTEN_SETTING("p2-ufodome", p2UfoDomeColor);
    LISTEN_SETTING("p2-tint-wave", p2TintWave);
    LISTEN_SETTING("p2-tint-trail", p2TintTrail);
    LISTEN_SETTING("p2-tint-ghost-trail", p2TintGhostTrail);
    LISTEN_SETTING("p2-tint-dashfire", p2TintDashFire);
    LISTEN_SETTING("p2-tint-ufodome", p2TintUfoDome);

    // Particles
    LISTEN_SETTING("tint-mainparticles-p1", tintMainParticlesP1);
    LISTEN_SETTING("p1-main-particles-start", p1MainParticlesStart);
    LISTEN_SETTING("p1-main-particles-end", p1MainParticlesEnd);
    LISTEN_SETTING("tint-mainparticles-p2", tintMainParticlesP2);
    LISTEN_SETTING("p2-main-particles-start", p2MainParticlesStart);
    LISTEN_SETTING("p2-main-particles-end", p2MainParticlesEnd);
    LISTEN_SETTING("tint-ufo-click-particles-p1", tintUfoClickParticlesP1);
    LISTEN_SETTING("p1-ufo-click-particles-start", p1UfoClickParticlesStart);
    LISTEN_SETTING("p1-ufo-click-particles-end", p1UfoClickParticlesEnd);
    LISTEN_SETTING("tint-ufo-click-particles-p2", tintUfoClickParticlesP2);
    LISTEN_SETTING("p2-ufo-click-particles-start", p2UfoClickParticlesStart);
    LISTEN_SETTING("p2-ufo-click-particles-end", p2UfoClickParticlesEnd);
}

class $modify(ColorsPlayer, PlayerObject) {
    void update(float dt) {
        PlayerObject::update(dt);

        // Determine which player's settings to use
        bool useP2 = m_isSecondPlayer;

        // --- 1. Determine Base Colors ---
        auto primary = useP2 ? p2PrimaryColor : p1PrimaryColor;
        auto secondary = useP2 ? p2SecondaryColor : p1SecondaryColor;

        if (useCustomColors) {
            m_iconSprite->setColor(primary);
            m_iconSpriteSecondary->setColor(secondary);
            m_vehicleSprite->setColor(primary);
            // The UFO dome is handled separately below, so we don't apply the secondary color to all vehicles
            if (!m_isUfo) {
                m_vehicleSpriteSecondary->setColor(secondary);
            }

            if (m_isRobot) {
                m_robotSprite->m_color = primary;
                m_robotSprite->m_secondColor = secondary;
                m_robotSprite->updateColors();
            } else if (m_isSpider) {
                m_spiderSprite->m_color = primary;
                m_spiderSprite->m_secondColor = secondary;
                m_spiderSprite->updateColors();
            }
        }

        // --- 2. Handle Standalone Tinting ---

        // Wave Trail
        if (useP2 ? p2TintWave : p1TintWave) {
            if (m_waveTrail) m_waveTrail->setColor(useP2 ? p2WaveColor : p1WaveColor);
        }

        // Regular Trail
        if (useP2 ? p2TintTrail : p1TintTrail) {
            if (m_regularTrail) m_regularTrail->setColor(useP2 ? p2TrailColor : p1TrailColor);
        }

        // Ghost Trail (Swing)
        if (useP2 ? p2TintGhostTrail : p1TintGhostTrail) {
            if (m_ghostTrail) m_ghostTrail->m_color = useP2 ? p2GhostTrailColor : p1GhostTrailColor;
        }

        // UFO Dome
        if (m_isUfo && (useP2 ? p2TintUfoDome : p1TintUfoDome)) {
             m_vehicleSpriteSecondary->setColor(useP2 ? p2UfoDomeColor : p1UfoDomeColor);
        }

        // --- 3. Handle Particles & Dash Fire ---

        // Set up particle colors based on settings
        cocos2d::ccColor4F mainStartColor, mainEndColor;
        cocos2d::ccColor4F ufoClickStartColor, ufoClickEndColor;
        cocos2d::ccColor4F dashFireStartColor, dashFireEndColor;
        cocos2d::ccColor3B dashFireSpriteColor;

        if (useP2) {
            mainStartColor = (tintMainParticlesP2) ? toColor4F(p2MainParticlesStart) : toColor4F(primary);
            mainEndColor = (tintMainParticlesP2) ? toColor4F(p2MainParticlesEnd) : toColor4F(primary, 0.f);
            ufoClickStartColor = (tintUfoClickParticlesP2) ? toColor4F(p2UfoClickParticlesStart) : toColor4F(primary);
            ufoClickEndColor = (tintUfoClickParticlesP2) ? toColor4F(p2UfoClickParticlesEnd) : toColor4F(primary, 0.f);
            dashFireSpriteColor = p2TintDashFire ? p2DashFireColor : primary;
        } else {
            mainStartColor = (tintMainParticlesP1) ? toColor4F(p1MainParticlesStart) : toColor4F(primary);
            mainEndColor = (tintMainParticlesP1) ? to_Color4F(p1MainParticlesEnd) : toColor4F(primary, 0.f);
            ufoClickStartColor = (tintUfoClickParticlesP1) ? toColor4F(p1UfoClickParticlesStart) : toColor4F(primary);
            ufoClickEndColor = (tintUfoClickParticlesP1) ? toColor4F(p1UfoClickParticlesEnd) : toColor4F(primary, 0.f);
            dashFireSpriteColor = p1TintDashFire ? p1DashFireColor : primary;
        }
        
        // Dash fire colors are derived from its sprite color
        dashFireStartColor = toColor4F(dashFireSpriteColor);
        dashFireEndColor = toColor4F(dashFireSpriteColor, 0.f);

        // Apply particle colors
        m_playerGroundParticles->m_tStartColor = mainStartColor;
        m_playerGroundParticles->m_tEndColor = mainEndColor;
        m_landParticles0->m_tStartColor = mainStartColor;
        m_landParticles0->m_tEndColor = mainEndColor;
        m_landParticles1->m_tStartColor = mainStartColor;
        m_landParticles1->m_tEndColor = mainEndColor;
        m_vehicleGroundParticles->m_tStartColor = mainStartColor;
        m_vehicleGroundParticles->m_tEndColor = mainEndColor;
        
        m_ufoClickParticles->m_tStartColor = ufoClickStartColor;
        m_ufoClickParticles->m_tEndColor = ufoClickEndColor;

        if (m_dashFireSprite) {
            m_dashFireSprite->setColor(dashFireSpriteColor);
            m_dashParticles->m_tStartColor = dashFireStartColor;
            m_dashParticles->m_tEndColor = dashFireEndColor;
        }

        // Handle variance override
        if (overrideAllVariances) {
            auto varianceColor = toColor4F(variancesOverride);
            m_playerGroundParticles->m_tStartColorVar = varianceColor;
            m_playerGroundParticles->m_tEndColorVar = varianceColor;
            m_landParticles0->m_tStartColorVar = varianceColor;
            m_landParticles0->m_tEndColorVar = varianceColor;
            m_landParticles1->m_tStartColorVar = varianceColor;
            m_landParticles1->m_tEndColorVar = varianceColor;
            m_vehicleGroundParticles->m_tStartColorVar = varianceColor;
            m_vehicleGroundParticles->m_tEndColorVar = varianceColor;
            m_ufoClickParticles->m_tStartColorVar = varianceColor;
            m_ufoClickParticles->m_tEndColorVar = varianceColor;
        }
    }
};
