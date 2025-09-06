#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <unordered_map>

using namespace geode::prelude;

// Forward declarations for sub-popups
class ConditionalRandomizerPopup;
class LiveRandomizerPopup;
class ModesPopup;
class RGBMainPopup;
class RGBExtrasPopup;
class CustomColorsPopup;

class CustomSettingsPopup : public Popup<> {
protected:
    bool setup() override;
    
    // Randomizer callbacks
    void onRandomizerToggle(CCObject* sender);
    void onDelaySlider(CCObject* sender);

    // open sub-popups
    void onOpenConditional(CCObject* sender);
    void onOpenLive(CCObject* sender);
    void onOpenModes(CCObject* sender);
    void onOpenRGBMain(CCObject* sender);
    void onOpenRGBExtras(CCObject* sender);
    void onOpenColors(CCObject* sender);
    void onOpenParticles(CCObject* sender);
    
    // RGB callbacks
    void onRGBToggle(CCObject* sender);
    void onRGBSpeedSlider(CCObject* sender);
    void onRGBSaturationSlider(CCObject* sender);

    // Create sections
    CCNode* createRandomizerSection();
    CCNode* createRGBSection();
    CCNode* createCustomColorsSection();

    // Helper methods
    CCMenuItemToggler* createToggler(std::string settingId, CCPoint position);
    Slider* createSlider(std::string settingId, CCPoint position, float width = 200.0f);
    
    // mapping tags -> setting ids (more reliable than assuming userObject survives)
    std::unordered_map<int, std::string> m_tagToSetting;
    int m_tagCounter = 0;
    
public:
    static CustomSettingsPopup* create();
};

// Custom Colors popup (Colors / Particles)
class CustomColorsPopup : public Popup<> {
protected:
    bool setup() override;
public:
    static CustomColorsPopup* create();
};

// Conditional Randomizer Sub-Popup
class ConditionalRandomizerPopup : public Popup<> {
protected:
    bool setup() override;
    void onToggle(CCObject* sender);
    CCMenuItemToggler* createToggler(std::string settingId, CCPoint position);
    
    std::unordered_map<int, std::string> m_tagToSetting;
    
public:
    static ConditionalRandomizerPopup* create();
};

// Live Randomizer Sub-Popup
class LiveRandomizerPopup : public Popup<> {
protected:
    bool setup() override;
    void onToggle(CCObject* sender);
    void onDelaySlider(CCObject* sender);
    CCMenuItemToggler* createToggler(std::string settingId, CCPoint position);
    Slider* createSlider(std::string settingId, CCPoint position, float width = 200.0f);
    
    std::unordered_map<int, std::string> m_tagToSetting;
    CCLabelBMFont* m_delayValueLabel = nullptr;
    
public:
    static LiveRandomizerPopup* create();
};

// Modes popup (holds moved toggles from main Randomizer)
class ModesPopup : public Popup<> {
protected:
    bool setup() override;
    void onToggle(CCObject* sender);
    CCMenuItemToggler* createToggler(std::string settingId, CCPoint position);

    std::unordered_map<int, std::string> m_tagToSetting;

public:
    static ModesPopup* create();
};

// RGB Main / Extras popups
class RGBMainPopup : public Popup<> {
protected:
    bool setup() override;
    void onToggle(CCObject* sender);
    void onSlider(CCObject* sender);
    CCMenuItemToggler* createToggler(std::string settingId, CCPoint position);
    Slider* createSlider(std::string settingId, CCPoint position, float width = 200.0f);

    std::unordered_map<int, std::string> m_tagToSetting;
    CCLabelBMFont* m_valueLabel = nullptr;

public:
    static RGBMainPopup* create();
};

class RGBExtrasPopup : public Popup<> {
protected:
    bool setup() override;

public:
    static RGBExtrasPopup* create();
};