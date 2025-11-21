#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <unordered_map>
#include "BlurLayer.hpp"

using namespace geode::prelude;

enum class SettingsCategory {
    Randomizer,
    RGB,
    Colors,
    Particles,
    Legacy
};

class ModernSettingsPopup : public Popup<>, public geode::ColorPickPopupDelegate {
protected:
    SettingsCategory m_currentCategory = SettingsCategory::Randomizer;
    CCNode* m_contentArea = nullptr;
    CCNode* m_contentContainer = nullptr;
    CCNode* m_sidebar = nullptr;
    CCLabelBMFont* m_titleLabel = nullptr;
    CCSprite* m_separator = nullptr;
    CCSprite* m_areaSeparator = nullptr;
    BlurLayer* m_blurLayer = nullptr;
    CCSprite* m_animSprite = nullptr;
    std::unordered_map<SettingsCategory, CCMenuItemSprite*> m_categoryButtons;
    
    std::unordered_map<int, std::string> m_tagToSetting;
    std::unordered_map<int, CCLabelBMFont*> m_valueLabels;
    std::string m_currentColorSettingId;
    CCSprite* m_currentButtonSprite = nullptr;
    bool m_isColor4B = false;
    
    bool setup() override;
    void setupSidebar();
    void setupContentArea();
    
    // Category switch
    void switchCategory(SettingsCategory category);
    void onCategoryButton(CCObject* sender);
    void updateCategoryButtonStates();
    
    // Contents
    void buildRandomizerContent();
    void buildRGBContent();
    void buildColorsContent();
    void buildParticlesContent();
    void buildLegacyContent();
    
    // Creators
    CCNode* createSectionTitle(const std::string& title);
    CCNode* createSectionTitleWithToggle(const std::string& title, const std::string& settingId, float width);
    CCNode* createSection(const std::string& title, float width);
    CCNode* createLabelToggleRow(const std::string& labelText, const std::string& settingId, float width, float labelScale = 0.4f, float toggleScale = 0.65f);
    CCNode* createParticleToggleRow(const std::string& labelText, const std::string& settingId, float width);
    CCNode* createLabelSliderRow(const std::string& labelText, const std::string& settingId, float width, float labelScale = 0.38f);
    CCNode* createLabelColorRow(const std::string& labelText, const std::string& colorSettingId, float width, const std::string& toggleSettingId = "", float labelScale = 0.38f);
    CCNode* createColorPairRow(const std::string& startColorId, const std::string& endColorId, float width);
    CCNode* createParticleColorPair(const std::string& toggleSettingId, const std::string& startColorId, const std::string& endColorId, float width);
    CCMenuItemToggler* createToggler(std::string settingId, CCPoint position);
    Slider* createSlider(std::string settingId, CCPoint position, float width = 200.0f);
    CCMenu* createColorPickerButton(const std::string& settingId, CCPoint position, bool is4B = false);
    CCMenuItemSprite* createSidebarButton(const std::string& iconName, const std::string& labelText, SettingsCategory category);
    
    // Callbacks
    void onToggle(CCObject* sender);
    void onSlider(CCObject* sender);
    void onColorPicker(CCObject* sender);
    void onModSettings(CCObject* sender);
    void updateColor(cocos2d::ccColor4B const& color) override;
    void onAnimationComplete();
    
public:
    static ModernSettingsPopup* create();
};