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
    
    // Convert ccColor4B to ccColor4F for particles
    cocos2d::ccColor4F getParticleColor4F(const cocos2d::ccColor4B& color) const {
        return {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};
    }
    
    // Get primary color as particle color (no alpha fade)
    cocos2d::ccColor4F getPrimaryParticleColor() const {
        return {primary.r / 255.0f, primary.g / 255.0f, primary.b / 255.0f, 1.0f};
    }
};

struct ColorSettings {
    // Main toggles
    bool useCustomColors = false;
    
    // Individual tinting toggles
    bool tintWave = false;
    bool tintTrail = false;
    bool tintGhostTrail = false;
    bool tintDashFire = false;
    bool tintUfoDome = false;
    
    // Per-player tinting toggles
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
    
    // Dirty flags for optimization
    bool colorsDirty = true;
    bool settingsDirty = true;
    
    void markColorsDirty() { colorsDirty = true; }
    void markSettingsDirty() { settingsDirty = true; }
};

static ColorSettings g_colorSettings;

// Helper function to load all settings
void loadAllSettings() {
    auto& settings = g_colorSettings;
    
    // Main toggles
    settings.useCustomColors = theMod->getSettingValue<bool>("enable-customcolors");
    
    // Individual tinting toggles
    settings.tintWave = theMod->getSettingValue<bool>("tint-wave");
    settings.tintTrail = theMod->getSettingValue<bool>("tint-trail");
    settings.tintGhostTrail = theMod->getSettingValue<bool>("tint-ghost-trail");
    settings.tintDashFire = theMod->getSettingValue<bool>("tint-dashfire");
    settings.tintUfoDome = theMod->getSettingValue<bool>("tint-ufodome");
    
    // Per-player tinting (you'll need to add these to mod.json)
    settings.tintWaveP1 = theMod->getSettingValue<bool>("tint-wave-p1");
    settings.tintWaveP2 = theMod->getSettingValue<bool>("tint-wave-p2");
    settings.tintTrailP1 = theMod->getSettingValue<bool>("tint-trail-p1");
    settings.tintTrailP2 = theMod->getSettingValue<bool>("tint-trail-p2");
    settings.tintGhostTrailP1 = theMod->getSettingValue<bool>("tint-ghost-trail-p1");
    settings.tintGhostTrailP2 = theMod->getSettingValue<bool>("tint-ghost-trail-p2");
    settings.tintDashFireP1 = theMod->getSettingValue<bool>("tint-dashfire-p1");
    settings.tintDashFireP2 = theMod->getSettingValue<bool>("tint-dashfire-p2");
    settings.tintUfoDomeP1 = theMod->getSettingValue<bool>("tint-ufodome-p1");
    settings.tintUfoDomeP2 = theMod->getSettingValue<bool>("tint-ufodome-p2");
    
    // Particle settings
    settings.tintMainParticlesP1 = theMod->getSettingValue<bool>("tint-mainparticles-p1");
    settings.tintMainParticlesP2 = theMod->getSettingValue<bool>("tint-mainparticles-p2");
    settings.tintUfoClickParticlesP1 = theMod->getSettingValue<bool>("tint-ufo-click-particles-p1");
    settings.tintUfoClickParticlesP2 = theMod->getSettingValue<bool>("tint-ufo-click-particles-p2");
    settings.overrideAllVariances = theMod->getSettingValue<bool>("override-all-variances");
    
    // Player 1 colors
    settings.p1.primary = theMod->getSettingValue<cocos2d::ccColor3B>("p1-primary");
    settings.p1.secondary = theMod->getSettingValue<cocos2d::ccColor3B>("p1-secondary");
    settings.p1.glow = theMod->getSettingValue<cocos2d::ccColor3B>("p1-glow");
    settings.p1.wave = theMod->getSettingValue<cocos2d::ccColor3B>("p1-wave");
    settings.p1.trail = theMod->getSettingValue<cocos2d::ccColor3B>("p1-trail");
    settings.p1.ghostTrail = theMod->getSettingValue<cocos2d::ccColor3B>("p1-ghost-trail");
    settings.p1.dashFire = theMod->getSettingValue<cocos2d::ccColor3B>("p1-dashfire");
    settings.p1.ufoDome = theMod->getSettingValue<cocos2d::ccColor3B>("p1-ufodome");
    settings.p1.mainParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("p1-main-particles-start");
    settings.p1.mainParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("p1-main-particles-end");
    settings.p1.ufoClickParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("p1-ufo-click-particles-start");
    settings.p1.ufoClickParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("p1-ufo-click-particles-end");
    
    // Player 2 colors
    settings.p2.primary = theMod->getSettingValue<cocos2d::ccColor3B>("p2-primary");
    settings.p2.secondary = theMod->getSettingValue<cocos2d::ccColor3B>("p2-secondary");
    settings.p2.glow = theMod->getSettingValue<cocos2d::ccColor3B>("p2-glow");
    settings.p2.wave = theMod->getSettingValue<cocos2d::ccColor3B>("p2-wave");
    settings.p2.trail = theMod->getSettingValue<cocos2d::ccColor3B>("p2-trail");
    settings.p2.ghostTrail = theMod->getSettingValue<cocos2d::ccColor3B>("p2-ghost-trail");
    settings.p2.dashFire = theMod->getSettingValue<cocos2d::ccColor3B>("p2-dashfire");
    settings.p2.ufoDome = theMod->getSettingValue<cocos2d::ccColor3B>("p2-ufodome");
    settings.p2.mainParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("p2-main-particles-start");
    settings.p2.mainParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("p2-main-particles-end");
    settings.p2.ufoClickParticlesStart = theMod->getSettingValue<cocos2d::ccColor4B>("p2-ufo-click-particles-start");
    settings.p2.ufoClickParticlesEnd = theMod->getSettingValue<cocos2d::ccColor4B>("p2-ufo-click-particles-end");
    
    settings.variancesOverride = theMod->getSettingValue<cocos2d::ccColor4B>("variances-override");
    
    settings.colorsDirty = false;
    settings.settingsDirty = false;
}

// Macro to create setting listeners more cleanly
#define LISTEN_SETTING(name, var) \
    listenForSettingChanges(name, [](auto value) { \
        g_colorSettings.var = value; \
        g_colorSettings.markColorsDirty(); \
    })

$on_mod(Loaded) {
    loadAllSettings();
    
    // Settings listeners
    LISTEN_SETTING("enable-customcolors", useCustomColors);
    LISTEN_SETTING("tint-wave", tintWave);
    LISTEN_SETTING("tint-trail", tintTrail);
    LISTEN_SETTING("tint-ghost-trail", tintGhostTrail);
    LISTEN_SETTING("tint-dashfire", tintDashFire);
    LISTEN_SETTING("tint-ufodome", tintUfoDome);
    
    // Per-player listeners (add these to mod.json)
    LISTEN_SETTING("tint-wave-p1", tintWaveP1);
    LISTEN_SETTING("tint-wave-p2", tintWaveP2);
    LISTEN_SETTING("tint-trail-p1", tintTrailP1);
    LISTEN_SETTING("tint-trail-p2", tintTrailP2);
    LISTEN_SETTING("tint-ghost-trail-p1", tintGhostTrailP1);
    LISTEN_SETTING("tint-ghost-trail-p2", tintGhostTrailP2);
    LISTEN_SETTING("tint-dashfire-p1", tintDashFireP1);
    LISTEN_SETTING("tint-dashfire-p2", tintDashFireP2);
    LISTEN_SETTING("tint-ufodome-p1", tintUfoDomeP1);
    LISTEN_SETTING("tint-ufodome-p2", tintUfoDomeP2);
    
    // Particle listeners
    LISTEN_SETTING("tint-mainparticles-p1", tintMainParticlesP1);
    LISTEN_SETTING("tint-mainparticles-p2", tintMainParticlesP2);
    LISTEN_SETTING("tint-ufo-click-particles-p1", tintUfoClickParticlesP1);
    LISTEN_SETTING("tint-ufo-click-particles-p2", tintUfoClickParticlesP2);
    LISTEN_SETTING("override-all-variances", overrideAllVariances);
    
    // Color listeners
    LISTEN_SETTING("p1-primary", p1.primary);
    LISTEN_SETTING("p1-secondary", p1.secondary);
    LISTEN_SETTING("p1-glow", p1.glow);
    LISTEN_SETTING("p1-wave", p1.wave);
    LISTEN_SETTING("p1-trail", p1.trail);
    LISTEN_SETTING("p1-ghost-trail", p1.ghostTrail);
    LISTEN_SETTING("p1-dashfire", p1.dashFire);
    LISTEN_SETTING("p1-ufodome", p1.ufoDome);
    LISTEN_SETTING("p1-main-particles-start", p1.mainParticlesStart);
    LISTEN_SETTING("p1-main-particles-end", p1.mainParticlesEnd);
    LISTEN_SETTING("p1-ufo-click-particles-start", p1.ufoClickParticlesStart);
    LISTEN_SETTING("p1-ufo-click-particles-end", p1.ufoClickParticlesEnd);
    
    LISTEN_SETTING("p2-primary", p2.primary);
    LISTEN_SETTING("p2-secondary", p2.secondary);
    LISTEN_SETTING("p2-glow", p2.glow);
    LISTEN_SETTING("p2-wave", p2.wave);
    LISTEN_SETTING("p2-trail", p2.trail);
    LISTEN_SETTING("p2-ghost-trail", p2.ghostTrail);
    LISTEN_SETTING("p2-dashfire", p2.dashFire);
    LISTEN_SETTING("p2-ufodome", p2.ufoDome);
    LISTEN_SETTING("p2-main-particles-start", p2.mainParticlesStart);
    LISTEN_SETTING("p2-main-particles-end", p2.mainParticlesEnd);
    LISTEN_SETTING("p2-ufo-click-particles-start", p2.ufoClickParticlesStart);
    LISTEN_SETTING("p2-ufo-click-particles-end", p2.ufoClickParticlesEnd);
    
    LISTEN_SETTING("variances-override", variancesOverride);
}

class $modify(ColorsPlayer, PlayerObject) {
    struct Fields {
        bool lastColorsApplied = false;
        float lastUpdateTime = 0.0f;
        static constexpr float UPDATE_INTERVAL = 0.016f; // ~60 FPS
    };

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
        
        // Special elements (can work independently of custom colors)
        if (m_waveTrail) {
            bool shouldTint = isP2 ? settings.tintWaveP2 : settings.tintWaveP1;
            if (settings.tintWave || shouldTint) {
                m_waveTrail->setColor(colors.wave);
            } else if (settings.useCustomColors) {
                m_waveTrail->setColor(colors.primary);
            }
        }
        
        if (m_regularTrail) {
            bool shouldTint = isP2 ? settings.tintTrailP2 : settings.tintTrailP1;
            if (settings.tintTrail || shouldTint) {
                m_regularTrail->setColor(colors.trail);
            } else if (settings.useCustomColors) {
                // Don't tint trail if custom colors but no specific trail tint
            }
        }
        
        if (m_ghostTrail) {
            bool shouldTint = isP2 ? settings.tintGhostTrailP2 : settings.tintGhostTrailP1;
            if (settings.tintGhostTrail || shouldTint) {
                m_ghostTrail->m_color = colors.ghostTrail;
            } else if (settings.useCustomColors) {
                m_ghostTrail->m_color = colors.primary;
            }
        }
        
        if (m_dashFireSprite) {
            bool shouldTint = isP2 ? settings.tintDashFireP2 : settings.tintDashFireP1;
            if (settings.tintDashFire || shouldTint) {
                m_dashFireSprite->setColor(colors.dashFire);
                if (m_dashParticles) {
                    auto dashStart = colors.getParticleColor4F({colors.dashFire.r, colors.dashFire.g, colors.dashFire.b, 255});
                    auto dashEnd = colors.getParticleColor4F({colors.dashFire.r, colors.dashFire.g, colors.dashFire.b, 0});
                    m_dashParticles->m_tStartColor = dashStart;
                    m_dashParticles->m_tEndColor = dashEnd;
                }
            } else if (settings.useCustomColors) {
                m_dashFireSprite->setColor(colors.primary);
                if (m_dashParticles) {
                    auto primaryParticle = colors.getPrimaryParticleColor();
                    m_dashParticles->m_tStartColor = primaryParticle;
                    m_dashParticles->m_tEndColor = {primaryParticle.r, primaryParticle.g, primaryParticle.b, 0.0f};
                }
            }
        }
        
        if (m_birdVehicle) {
            bool shouldTint = isP2 ? settings.tintUfoDomeP2 : settings.tintUfoDomeP1;
            if (settings.tintUfoDome || shouldTint) {
                m_birdVehicle->setColor(colors.ufoDome);
            } else if (settings.useCustomColors) {
                m_birdVehicle->setColor(colors.primary);
            }
        }
        
        // Particles
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
            
            auto applyToParticle = [&](cocos2d::CCParticleSystem* particle) {
                if (particle) {
                    particle->m_tStartColor = startColor;
                    particle->m_tEndColor = endColor;
                    
                    if (settings.overrideAllVariances) {
                        auto variance = colors.getParticleColor4F(settings.variancesOverride);
                        particle->m_tStartColorVar = variance;
                        particle->m_tEndColorVar = variance;
                    }
                }
            };
            
            applyToParticle(m_playerGroundParticles);
            applyToParticle(m_landParticles0);
            applyToParticle(m_landParticles1);
            applyToParticle(m_vehicleGroundParticles);
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
        
        auto fields = m_fields.self();
        
        // Check if we need to apply colors at all
        if (g_colorSettings.useCustomColors || 
            g_colorSettings.tintWave || g_colorSettings.tintTrail || 
            g_colorSettings.tintGhostTrail || g_colorSettings.tintDashFire || 
            g_colorSettings.tintUfoDome ||
            (m_isSecondPlayer ? (g_colorSettings.tintWaveP2 || g_colorSettings.tintTrailP2 || 
                               g_colorSettings.tintGhostTrailP2 || g_colorSettings.tintDashFireP2 || 
                               g_colorSettings.tintUfoDomeP2) :
                              (g_colorSettings.tintWaveP1 || g_colorSettings.tintTrailP1 || 
                               g_colorSettings.tintGhostTrailP1 || g_colorSettings.tintDashFireP1 || 
                               g_colorSettings.tintUfoDomeP1))) {
            
            applyColorsToPlayer();
            fields->lastColorsApplied = true;
        }
    }
};

class $modify(ColorsPlayLayer, PlayLayer) {
    void resume() {
        PlayLayer::resume();
        g_colorSettings.markColorsDirty();
    }

    void resumeAndRestart(bool p0) {
        PlayLayer::resumeAndRestart(p0);
        g_colorSettings.markColorsDirty();
    }
};
