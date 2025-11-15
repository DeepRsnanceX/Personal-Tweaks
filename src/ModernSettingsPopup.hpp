#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <unordered_map>

using namespace geode::prelude;

enum class SettingsCategory {
    Randomizer,
    RGB,
    Colors,
    Particles
};

class ModernSettingsPopup : public Popup<>, public geode::ColorPickPopupDelegate {
protected:
    SettingsCategory m_currentCategory = SettingsCategory::Randomizer;
    CCNode* m_contentArea = nullptr;
    CCNode* m_sidebar = nullptr;
    CCSprite* m_blurBG = nullptr;
    std::unordered_map<SettingsCategory, CCMenuItemSprite*> m_categoryButtons;
    
    // Reusable components
    std::unordered_map<int, std::string> m_tagToSetting;
    std::unordered_map<int, CCLabelBMFont*> m_valueLabels;
    std::string m_currentColorSettingId;
    CCSprite* m_currentButtonSprite = nullptr;
    bool m_isColor4B = false;

    int m_currentPage = 0;
    int m_maxPages = 0;
    
    bool setup() override;
    void setupBlurBackground();
    void setupSidebar();
    void setupContentArea();
    
    // Category switching
    void switchCategory(SettingsCategory category);
    void onCategoryButton(CCObject* sender);
    void updateCategoryButtonStates();
    
    // Content builders for each category
    void buildRandomizerContent();
    void buildRGBContent();
    void buildColorsContent();
    void buildParticlesContent();
    
    // Helper methods
    CCMenuItemToggler* createToggler(std::string settingId, CCPoint position);
    CCMenuItemSprite* createRadioButton(std::string settingId, CCPoint position);
    Slider* createSlider(std::string settingId, CCPoint position, float width = 200.0f);
    CCMenuItemSpriteExtra* createColorPickerButton(const std::string& settingId, CCPoint position, bool is4B = false);
    CCNode* createSectionTitle(const std::string& title);
    CCMenuItemSprite* createSidebarButton(const std::string& iconName, const std::string& labelText, SettingsCategory category);
    CCMenuItemSpriteExtra* createNavButton(const std::string& labelText);
    
    // Callbacks
    void onToggle(CCObject* sender);
    void onRadioButton(CCObject* sender);
    void onSlider(CCObject* sender);
    void onColorPicker(CCObject* sender);
    void updateColor(cocos2d::ccColor4B const& color) override;
    void onNavButton(CCObject* sender);
    
public:
    static ModernSettingsPopup* create();
};