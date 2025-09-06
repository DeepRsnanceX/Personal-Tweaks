#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <unordered_map>

using namespace geode::prelude;

// Forward declarations for sub-popups
class ConditionalRandomizerPopup;
class LiveRandomizerPopup;

class CustomSettingsPopup : public Popup<> {
protected:
    bool setup() override;
    
    // Randomizer callbacks
    void onRandomizerToggle(CCObject* sender);
    void onDelaySlider(CCObject* sender);

    // open sub-popups
    void onOpenConditional(CCObject* sender);
    void onOpenLive(CCObject* sender);
    
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