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
    bool useCustomColors = false, tintWave = false, tintTrail = false, tintGhostTrail = false, tintDashFire = false, tintUfoDome = false;
    bool tintWaveP1 = false, tintWaveP2 = false, tintTrailP1 = false, tintTrailP2 = false, tintGhostTrailP1 = false, tintGhostTrailP2 = false, tintDashFireP1 = false, tintDashFireP2 = false, tintUfoDomeP1 = false, tintUfoDomeP2 = false;
    bool tintMainParticlesP1 = false, tintMainParticlesP2 = false, tintUfoClickParticlesP1 = false, tintUfoClickParticlesP2 = false, overrideAllVariances = false;
    PlayerColors p1, p2;
    cocos2d::ccColor4B variancesOverride = {10, 10, 10, 10};
};

static ColorSettings g_colorSettings;

// Helper function to load all settings from the mod's config
void loadAllSettings() {
    auto& s = g_colorSettings;
    s.useCustomColors = theMod->getSettingValue<bool>("enable-customcolors");
    s.tintWave = theMod->getSettingValue<bool>("tint-wave");
    s.tintTrail = theMod->getSettingValue<bool>("tint-trail");
    s.tintGhostTrail = theMod->getSettingValue<bool>("tint-ghost-trail");
    s.tintDashFire = theMod->getSettingValue<bool>("tint-dashfire");
    s.tintUfoDome = theMod->getSettingValue<bool>("tint-ufodome");
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
    s.tintMainParticlesP1 = theMod->getSettingValue<bool>("tint-mainparticles-p1");
    s.tintMainParticlesP2 = theMod->getSettingValue<bool>("tint-mainparticles-p2");
    s.tintUfoClickParticlesP1 = theMod->getSettingValue<bool>("tint-ufo-click-particles-p1");
    s.tintUfoClickParticlesP2 = theMod->getSettingValue<bool>("tint-ufo-click-particles-p2");
    s.overrideAllVariances = theMod->getSettingValue<bool>("override-all-variances");
    s.p1.primary = theMod->getSettingValue<cocos2d::ccColor3B>("p1-primary");
    s.p1.secondary = theMod->getSettingValue<cocos2d::ccColor3B>("p1-secondary");
    // ... (load all other colors for p1 and p2) ...
    s.variancesOverride = theMod->getSettingValue<cocos2d::ccColor4B>("variances-override");
}


$on_mod(Loaded) {
    // Perform an initial load of all settings
    loadAllSettings();
    
    // Setting listeners are still useful for live changes if the user doesn't pause
    listenForSettingChanges("enable-customcolors", [](bool value) { g_colorSettings.useCustomColors = value; });
    // ... add listeners for all other settings if desired, following the same pattern ...
}

class $modify(ColorsPlayer, PlayerObject) {

    void applyColorsToPlayer() {
        const auto& settings = g_colorSettings;
        bool isP2 = m_isSecondPlayer;
        const PlayerColors& colors = isP2 ? settings.p2 : settings.p1;
        
        if (settings.useCustomColors) {
            m_iconSprite->setColor(colors.primary);
            m_iconSpriteSecondary->setColor(colors.secondary);
            m_vehicleSprite->setColor(colors.primary);
            m_vehicleSpriteSecondary->setColor(colors.secondary); // This is correct for ship, etc.
            // ... robot/spider logic ...
        }
        
        // --- Special Elements ---

        if (m_waveTrail) {
            bool shouldTint = isP2 ? settings.tintWaveP2 : settings.tintWaveP1;
            if (settings.tintWave || shouldTint) m_waveTrail->setColor(colors.wave);
            else if (settings.useCustomColors) m_waveTrail->setColor(colors.primary);
        }
        
        // CORRECTED: Check for m_isBird and apply color to m_birdVehicle
        if (m_isBird && m_birdVehicle) {
            bool shouldTint = isP2 ? settings.tintUfoDomeP2 : settings.tintUfoDomeP1;
            if (settings.tintUfoDome || shouldTint) {
                m_birdVehicle->setColor(colors.ufoDome);
            } else if (settings.useCustomColors) {
                // Fallback for UFO dome is secondary color when custom colors are on
                m_birdVehicle->setColor(colors.secondary);
            }
        }

        // --- Particles ---
        
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

        bool shouldTintMain = isP2 ? settings.tintMainParticlesP2 : settings.tintMainParticlesP1;
        if (shouldTintMain || settings.useCustomColors) {
            cocos2d::ccColor4F startColor, endColor;
            if (shouldTintMain) {
                startColor = colors.getParticleColor4F(colors.mainParticlesStart);
                endColor = colors.getParticleColor4F(colors.mainParticlesEnd);
            } else {
                startColor = colors.getPrimaryParticleColor();
                // CORRECTED: End alpha is max, as particles fade by size, not alpha.
                endColor = startColor;
            }
            applyToParticle(m_playerGroundParticles, startColor, endColor);
            applyToParticle(m_landParticles0, startColor, endColor);
            applyToParticle(m_landParticles1, startColor, endColor);
            applyToParticle(m_vehicleGroundParticles, startColor, endColor);
        }
    }

    void update(float dt) {
        PlayerObject::update(dt);
        
        // This optimization check is still valid
        bool isP2 = m_isSecondPlayer;
        if (g_colorSettings.useCustomColors || g_colorSettings.tintWave || g_colorSettings.tintTrail || 
            (isP2 ? (g_colorSettings.tintWaveP2 || g_colorSettings.tintTrailP2) : (g_colorSettings.tintWaveP1 || g_colorSettings.tintTrailP1))
            /* ... add other checks here ... */) {
            
            applyColorsToPlayer();
        }
    }
};

// CORRECTED: Re-added PlayLayer hooks to reload settings on unpause
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
