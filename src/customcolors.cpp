#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

auto theMod = Mod::get();

struct PlayerColors {
    // Basic colors
    cocos2d::ccColor3B primary = {255, 255, 255};
    cocos2d::ccColor3B secondary = {255, 255, 255};
    cocos2d::ccColor3B glow = {255, 255, 255};
    
    // Special element colors
    cocos2d::ccColor3B wave = {255, 255, 255};
    cocos2d::ccColor3B trail = {255, 255, 255};
    cocos2d::ccColor3B ghostTrail = {255, 255, 255};
    cocos2d::ccColor3B dashFire = {255, 255, 255};
    cocos2d::ccColor3B ufoDome = {255, 255, 255};
    
    // Particle colors (RGBA)
    cocos2d::ccColor4B mainParticlesStart = {255, 255, 255, 255};
    cocos2d::ccColor4B mainParticlesEnd = {255, 255, 255, 255};
    cocos2d::ccColor4B ufoClickParticlesStart = {255, 255, 255, 255};
    cocos2d::ccColor4B ufoClickParticlesEnd = {255, 255, 255, 255};
    
    cocos2d::ccColor4F getParticleColor4F(const cocos2d::ccColor4B& color) const {
        return {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};
    }
    
    cocos2d::ccColor4F getPrimaryParticleColor() const {
        return {primary.r / 255.0f, primary.g / 255.0f, primary.b / 255.0f, 1.0f};
    }
};

struct ColorSettings {
    // Main toggle
    bool useCustomColors = false;
    
    // Per-player tinting toggles (these are the ones actually used)
    bool tintWaveP1 = false;
    bool tintWaveP2 = false;
    bool tintTrailP1 = false;
    bool tintTrailP2 = false;
    bool tintGhostTrailP1 = false;
    bool tintGhostTrailP2 = false;
    bool tintDashFireP1 = false;
    bool tintDashFireP2 = false;
    bool tintUfoDomeP1 = false;
    bool tintUfoDomeP2 = false;
    
    // Particle settings
    bool tintMainParticlesP1 = false;
    bool tintMainParticlesP2 = false;
    bool tintUfoClickParticlesP1 = false;
    bool tintUfoClickParticlesP2 = false;
    bool overrideAllVariances = false;
    
    // Colors
    PlayerColors p1;
    PlayerColors p2;
    cocos2d::ccColor4B variancesOverride = {10, 10, 10, 10};
};

static ColorSettings g_colorSettings;

// Helper function to load all settings from the mod's config
void loadAllSettings() {
    auto& s = g_colorSettings;
    
    // Main toggle
    s.useCustomColors = theMod->getSettingValue<bool>("enable-customcolors");
    
    // Per-player tinting toggles
    s.tintWaveP1 = theMod->getSettingValue<bool>("tint-wave-p1");
    s.tintWaveP2 = theMod->getSettingValue<bool>("tint-wave-p2");
    s.tintTrailP1 = theMod->getSettingValue<bool>("tint-trail-p1");
    s.tintTrailP2 = theMod->getSettingValue<bool>("tint-trail-p2");
    s.tintGhostTrailP1 = theMod->getSettingValue<bool>("tint-ghost-trail-p1");
    s.tintGhostTrailP2 = theMod->getSettingValue<bool>("tint-ghost-trail-p2");
    s.tintDashFireP1 = theMod->getSettingValue<bool>("tint-dashfire-p1");
    s.tintDashFireP2 = theMod->getSettingValue<bool>("tint-dashfire-p2");
    s.tintUfoDomeP1 = theMod->getSettingValue<bool>("tint-ufodome-p1");
    s.tintUfoDomeP2 = theMod->getSettingValue<bool>("tint-ufodome-p2");
    
    // Particle settings
    s.tintMainParticlesP1 = theMod->getSettingValue<bool>("tint-mainparticles-p1");
    s.tintMainParticlesP2 = theMod->getSettingValue<bool>("tint-mainparticles-p2");
    s.tintUfoClickParticlesP1 = theMod->getSettingValue<bool>("tint-ufo-click-particles-p1");
    s.tintUfoClickParticlesP2 = theMod->getSettingValue<bool>("tint-ufo-click-particles-p2");
    s.overrideAllVariances = theMod->getSettingValue<bool>("override-all-variances");
    
    // Player 1 colors
    s.p1.primary = theMod->getSettingValue<cocos2d::ccColor3B>("p1-primary");
    s.p1.secondary = theMod->getSettingValue<cocos2d::ccColor3B>("p1-secondary");
    s.p1.glow = theMod->getSettingValue<cocos2d::ccColor3B>("p1-glow");
    s.p1.wave = theMod->getSettingValue<cocos2d::ccColor3B>("p1-wave");
    s.p1.trail = theMod->getSettingValue<cocos2d::ccColor3B>("p1-trail");
    s.p1.ghostTrail = theMod->getSettingValue<cocos2d::ccColor3B>("p1-ghost-trail");
    s.p1.dashFire = theMod->getSettingValue<cocos2d::ccColor3B>("p1-dashfire");
    s.p1.ufoDome = theMod->getSettingValue<cocos2d::ccColor3B>("p1-ufodome");
    s.p1.mainParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("p1-main-particles-start");
    s.p1.mainParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("p1-main-particles-end");
    s.p1.ufoClickParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("p1-ufo-click-particles-start");
    s.p1.ufoClickParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("p1-ufo-click-particles-end");
    
    // Player 2 colors
    s.p2.primary = theMod->getSettingValue<cocos2d::ccColor3B>("p2-primary");
    s.p2.secondary = theMod->getSettingValue<cocos2d::ccColor3B>("p2-secondary");
    s.p2.glow = theMod->getSettingValue<cocos2d::ccColor3B>("p2-glow");
    s.p2.wave = theMod->getSettingValue<cocos2d::ccColor3B>("p2-wave");
    s.p2.trail = theMod->getSettingValue<cocos2d::ccColor3B>("p2-trail");
    s.p2.ghostTrail = theMod->getSettingValue<cocos2d::ccColor3B>("p2-ghost-trail");
    s.p2.dashFire = theMod->getSettingValue<cocos2d::ccColor3B>("p2-dashfire");
    s.p2.ufoDome = theMod->getSettingValue<cocos2d::ccColor3B>("p2-ufodome");
    s.p2.mainParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("p2-main-particles-start");
    s.p2.mainParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("p2-main-particles-end");
    s.p2.ufoClickParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("p2-ufo-click-particles-start");
    s.p2.ufoClickParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("p2-ufo-click-particles-end");
    
    s.variancesOverride = theMod->getSettingValue<cocos2d::ccColor4B>("variances-override");
}

$on_mod(Loaded) {
    // Perform an initial load of all settings
    loadAllSettings();
    
    // Setting listeners for live updates
    listenForSettingChanges("enable-customcolors", [](bool value) { 
        g_colorSettings.useCustomColors = value; 
    });
    
    // Per-player tinting listeners
    listenForSettingChanges("tint-wave-p1", [](bool value) { 
        g_colorSettings.tintWaveP1 = value; 
    });
    listenForSettingChanges("tint-wave-p2", [](bool value) { 
        g_colorSettings.tintWaveP2 = value; 
    });
    listenForSettingChanges("tint-trail-p1", [](bool value) { 
        g_colorSettings.tintTrailP1 = value; 
    });
    listenForSettingChanges("tint-trail-p2", [](bool value) { 
        g_colorSettings.tintTrailP2 = value; 
    });
    listenForSettingChanges("tint-ghost-trail-p1", [](bool value) { 
        g_colorSettings.tintGhostTrailP1 = value; 
    });
    listenForSettingChanges("tint-ghost-trail-p2", [](bool value) { 
        g_colorSettings.tintGhostTrailP2 = value; 
    });
    listenForSettingChanges("tint-dashfire-p1", [](bool value) { 
        g_colorSettings.tintDashFireP1 = value; 
    });
    listenForSettingChanges("tint-dashfire-p2", [](bool value) { 
        g_colorSettings.tintDashFireP2 = value; 
    });
    listenForSettingChanges("tint-ufodome-p1", [](bool value) { 
        g_colorSettings.tintUfoDomeP1 = value; 
    });
    listenForSettingChanges("tint-ufodome-p2", [](bool value) { 
        g_colorSettings.tintUfoDomeP2 = value; 
    });
    
    // Particle listeners
    listenForSettingChanges("tint-mainparticles-p1", [](bool value) { 
        g_colorSettings.tintMainParticlesP1 = value; 
    });
    listenForSettingChanges("tint-mainparticles-p2", [](bool value) { 
        g_colorSettings.tintMainParticlesP2 = value; 
    });
    listenForSettingChanges("tint-ufo-click-particles-p1", [](bool value) { 
        g_colorSettings.tintUfoClickParticlesP1 = value; 
    });
    listenForSettingChanges("tint-ufo-click-particles-p2", [](bool value) { 
        g_colorSettings.tintUfoClickParticlesP2 = value; 
    });
    listenForSettingChanges("override-all-variances", [](bool value) { 
        g_colorSettings.overrideAllVariances = value; 
    });
    
    // Player 1 color listeners
    listenForSettingChanges("p1-primary", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p1.primary = value; 
    });
    listenForSettingChanges("p1-secondary", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p1.secondary = value; 
    });
    listenForSettingChanges("p1-glow", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p1.glow = value; 
    });
    listenForSettingChanges("p1-wave", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p1.wave = value; 
    });
    listenForSettingChanges("p1-trail", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p1.trail = value; 
    });
    listenForSettingChanges("p1-ghost-trail", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p1.ghostTrail = value; 
    });
    listenForSettingChanges("p1-dashfire", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p1.dashFire = value; 
    });
    listenForSettingChanges("p1-ufodome", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p1.ufoDome = value; 
    });
    listenForSettingChanges("p1-main-particles-start", [](cocos2d::ccColor4B value) { 
        g_colorSettings.p1.mainParticlesStart = value; 
    });
    listenForSettingChanges("p1-main-particles-end", [](cocos2d::ccColor4B value) { 
        g_colorSettings.p1.mainParticlesEnd = value; 
    });
    listenForSettingChanges("p1-ufo-click-particles-start", [](cocos2d::ccColor4B value) { 
        g_colorSettings.p1.ufoClickParticlesStart = value; 
    });
    listenForSettingChanges("p1-ufo-click-particles-end", [](cocos2d::ccColor4B value) { 
        g_colorSettings.p1.ufoClickParticlesEnd = value; 
    });
    
    // Player 2 color listeners
    listenForSettingChanges("p2-primary", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p2.primary = value; 
    });
    listenForSettingChanges("p2-secondary", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p2.secondary = value; 
    });
    listenForSettingChanges("p2-glow", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p2.glow = value; 
    });
    listenForSettingChanges("p2-wave", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p2.wave = value; 
    });
    listenForSettingChanges("p2-trail", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p2.trail = value; 
    });
    listenForSettingChanges("p2-ghost-trail", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p2.ghostTrail = value; 
    });
    listenForSettingChanges("p2-dashfire", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p2.dashFire = value; 
    });
    listenForSettingChanges("p2-ufodome", [](cocos2d::ccColor3B value) { 
        g_colorSettings.p2.ufoDome = value; 
    });
    listenForSettingChanges("p2-main-particles-start", [](cocos2d::ccColor4B value) { 
        g_colorSettings.p2.mainParticlesStart = value; 
    });
    listenForSettingChanges("p2-main-particles-end", [](cocos2d::ccColor4B value) { 
        g_colorSettings.p2.mainParticlesEnd = value; 
    });
    listenForSettingChanges("p2-ufo-click-particles-start", [](cocos2d::ccColor4B value) { 
        g_colorSettings.p2.ufoClickParticlesStart = value; 
    });
    listenForSettingChanges("p2-ufo-click-particles-end", [](cocos2d::ccColor4B value) { 
        g_colorSettings.p2.ufoClickParticlesEnd = value; 
    });
    
    listenForSettingChanges("variances-override", [](cocos2d::ccColor4B value) { 
        g_colorSettings.variancesOverride = value; 
    });
}

class $modify(ColorsPlayer, PlayerObject) {

    void applyColorsToPlayer() {
        const auto& settings = g_colorSettings;
        bool isP2 = m_isSecondPlayer;
        const PlayerColors& colors = isP2 ? settings.p2 : settings.p1;
        
        // Basic icon colors (only if custom colors enabled)
        if (settings.useCustomColors) {
            m_iconSprite->setColor(colors.primary);
            m_iconSpriteSecondary->setColor(colors.secondary);
            m_vehicleSprite->setColor(colors.primary);
            m_vehicleSpriteSecondary->setColor(colors.secondary);
            
            if (m_isRobot && m_robotSprite) {
                m_robotSprite->m_color = colors.primary;
                m_robotSprite->m_secondColor = colors.secondary;
                m_robotSprite->updateColors();
            } else if (m_isSpider && m_spiderSprite) {
                m_spiderSprite->m_color = colors.primary;
                m_spiderSprite->m_secondColor = colors.secondary;
                m_spiderSprite->updateColors();
            }
        }
        
        // Wave Trail
        if (m_waveTrail) {
            bool shouldTint = isP2 ? settings.tintWaveP2 : settings.tintWaveP1;
            if (shouldTint) {
                // Individual toggle enabled - use specific wave color
                m_waveTrail->setColor(colors.wave);
            } else if (settings.useCustomColors) {
                // Custom colors enabled but no individual toggle - fallback to primary
                m_waveTrail->setColor(colors.primary);
            }
            // If neither condition is true, don't tint (original game colors)
        }
        
        // Regular Trail
        if (m_regularTrail) {
            bool shouldTint = isP2 ? settings.tintTrailP2 : settings.tintTrailP1;
            if (shouldTint) {
                // Individual toggle enabled - use specific trail color
                m_regularTrail->setColor(colors.trail);
            } else if (settings.useCustomColors) {
                // Custom colors enabled but no individual toggle - fallback to secondary
                m_regularTrail->setColor(colors.secondary);
            }
            // If neither condition is true, don't tint (original game colors)
        }
        
        // Ghost Trail
        if (m_ghostTrail) {
            bool shouldTint = isP2 ? settings.tintGhostTrailP2 : settings.tintGhostTrailP1;
            if (shouldTint) {
                // Individual toggle enabled - use specific ghost trail color
                m_ghostTrail->m_color = colors.ghostTrail;
            } else if (settings.useCustomColors) {
                // Custom colors enabled but no individual toggle - fallback to primary
                m_ghostTrail->m_color = colors.primary;
            }
            // If neither condition is true, don't tint (original game colors)
        }
        
        // Dash Fire
        if (m_dashFireSprite) {
            bool shouldTint = isP2 ? settings.tintDashFireP2 : settings.tintDashFireP1;
            if (shouldTint) {
                // Individual toggle enabled - use specific dash fire color
                m_dashFireSprite->setColor(colors.dashFire);
                if (m_dashParticles) {
                    auto dashStart = colors.getParticleColor4F({colors.dashFire.r, colors.dashFire.g, colors.dashFire.b, 255});
                    auto dashEnd = colors.getParticleColor4F({colors.dashFire.r, colors.dashFire.g, colors.dashFire.b, 0});
                    m_dashParticles->m_tStartColor = dashStart;
                    m_dashParticles->m_tEndColor = dashEnd;
                }
            } else if (settings.useCustomColors) {
                // Custom colors enabled but no individual toggle - fallback to secondary
                m_dashFireSprite->setColor(colors.secondary);
                if (m_dashParticles) {
                    auto secondaryParticle = cocos2d::ccColor4F{colors.secondary.r / 255.0f, colors.secondary.g / 255.0f, colors.secondary.b / 255.0f, 1.0f};
                    m_dashParticles->m_tStartColor = secondaryParticle;
                    m_dashParticles->m_tEndColor = {secondaryParticle.r, secondaryParticle.g, secondaryParticle.b, 0.0f};
                }
            }
            // If neither condition is true, don't tint (original game colors)
        }
        
        // UFO Dome
        if (m_isBird && m_birdVehicle) {
            bool shouldTint = isP2 ? settings.tintUfoDomeP2 : settings.tintUfoDomeP1;
            if (shouldTint) {
                // Individual toggle enabled - use specific UFO dome color
                m_birdVehicle->setColor(colors.ufoDome);
            }
            // If custom colors enabled but no individual toggle - DON'T TINT UFO dome (as requested)
            // If neither condition is true, don't tint (original game colors)
        }
        
        // Particles
        auto applyToParticle = [&](cocos2d::CCParticleSystem* particle, const cocos2d::ccColor4F& start, const cocos2d::ccColor4F& end) {
            if (particle) {
                particle->m_tStartColor = start;
                particle->m_tEndColor = end;
                if (settings.overrideAllVariances) {
                    auto variance = colors.getParticleColor4F(settings.variancesOverride);
                    particle->m_tStartColorVar = variance;
                    particle->m_tEndColorVar = variance;
                }
            }
        };

        if (m_playerGroundParticles || m_landParticles0 || m_landParticles1 || m_vehicleGroundParticles) {
            bool shouldTintMain = isP2 ? settings.tintMainParticlesP2 : settings.tintMainParticlesP1;
            cocos2d::ccColor4F startColor, endColor;
            
            if (shouldTintMain) {
                startColor = colors.getParticleColor4F(colors.mainParticlesStart);
                endColor = colors.getParticleColor4F(colors.mainParticlesEnd);
            } else if (settings.useCustomColors) {
                startColor = colors.getPrimaryParticleColor();
                endColor = startColor; // No alpha fade for primary particles
            } else {
                return; // Skip particle updates if no relevant settings enabled
            }
            
            applyToParticle(m_playerGroundParticles, startColor, endColor);
            applyToParticle(m_landParticles0, startColor, endColor);
            applyToParticle(m_landParticles1, startColor, endColor);
            applyToParticle(m_vehicleGroundParticles, startColor, endColor);
        }
        
        if (m_ufoClickParticles) {
            bool shouldTintUfo = isP2 ? settings.tintUfoClickParticlesP2 : settings.tintUfoClickParticlesP1;
            if (shouldTintUfo) {
                m_ufoClickParticles->m_tStartColor = colors.getParticleColor4F(colors.ufoClickParticlesStart);
                m_ufoClickParticles->m_tEndColor = colors.getParticleColor4F(colors.ufoClickParticlesEnd);
            } else if (settings.useCustomColors) {
                auto primaryParticle = colors.getPrimaryParticleColor();
                m_ufoClickParticles->m_tStartColor = primaryParticle;
                m_ufoClickParticles->m_tEndColor = primaryParticle;
            }
            
            if (settings.overrideAllVariances) {
                auto variance = colors.getParticleColor4F(settings.variancesOverride);
                m_ufoClickParticles->m_tStartColorVar = variance;
                m_ufoClickParticles->m_tEndColorVar = variance;
            }
        }
    }

    void update(float dt) {
        PlayerObject::update(dt);
        
        // Check if we need to apply colors at all
        bool isP2 = m_isSecondPlayer;
        if (g_colorSettings.useCustomColors || 
            (isP2 ? (g_colorSettings.tintWaveP2 || g_colorSettings.tintTrailP2 || 
                   g_colorSettings.tintGhostTrailP2 || g_colorSettings.tintDashFireP2 || 
                   g_colorSettings.tintUfoDomeP2 || g_colorSettings.tintMainParticlesP2 || 
                   g_colorSettings.tintUfoClickParticlesP2) :
                  (g_colorSettings.tintWaveP1 || g_colorSettings.tintTrailP1 || 
                   g_colorSettings.tintGhostTrailP1 || g_colorSettings.tintDashFireP1 || 
                   g_colorSettings.tintUfoDomeP1 || g_colorSettings.tintMainParticlesP1 || 
                   g_colorSettings.tintUfoClickParticlesP1))) {
            
            applyColorsToPlayer();
        }
    }
};

class $modify(ColorsPlayLayer, PlayLayer) {
    void resume() {
        PlayLayer::resume();
        loadAllSettings();
    }

    void resumeAndRestart(bool p0) {
        PlayLayer::resumeAndRestart(p0);
        loadAllSettings();
    }
};