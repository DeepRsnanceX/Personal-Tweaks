#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

enum class SettingsTab {
    RANDOMIZER,
    RGB,
    COLORS,
    PARTICLES,
    LEGACY,
    RENDERER,
    MISC,
    TOTEM
};

class CustomSettingsPopup : public Popup<> {
protected:
    SettingsTab m_currentTab = SettingsTab::RANDOMIZER;
    CCMenu* m_tabMenu;
    CCMenu* m_contentMenu;
    ScrollLayer* m_scrollLayer;
    CCNode* m_contentNode;
    
    std::map<SettingsTab, CCMenuItemSpriteExtra*> m_tabButtons;

    bool setup() override {
        this->setTitle("Personal Tweaks Settings");
        
        auto winSize = this->m_mainLayer->getContentSize();
        
        // Create tab menu
        m_tabMenu = CCMenu::create();
        m_tabMenu->setLayout(RowLayout::create()->setGap(8.f)->setAxisAlignment(AxisAlignment::Center));
        m_tabMenu->setContentSize({winSize.width - 40, 35});
        m_tabMenu->setPosition({winSize.width / 2, winSize.height - 50});
        this->m_mainLayer->addChild(m_tabMenu);
        
        // Create tabs
        createTabs();
        
        // Create scroll layer for content
        auto scrollSize = CCSize{winSize.width - 60, winSize.height - 120};
        m_scrollLayer = ScrollLayer::create(scrollSize);
        m_scrollLayer->setPosition({30, 20});
        this->m_mainLayer->addChild(m_scrollLayer);
        
        // Create content node
        m_contentNode = CCNode::create();
        m_scrollLayer->m_contentLayer->addChild(m_contentNode);
        
        // Show initial tab
        showTab(SettingsTab::RANDOMIZER);
        
        return true;
    }
    
    void createTabs() {
        struct TabInfo {
            SettingsTab tab;
            const char* name;
        };
        
        std::vector<TabInfo> tabs = {
            {SettingsTab::RANDOMIZER, "Randomizer"},
            {SettingsTab::RGB, "RGB"},
            {SettingsTab::COLORS, "Colors"},
            {SettingsTab::PARTICLES, "Particles"},
            {SettingsTab::LEGACY, "Legacy"},
            {SettingsTab::RENDERER, "Renderer"},
            {SettingsTab::MISC, "Misc"},
            {SettingsTab::TOTEM, "Totem"}
        };
        
        for (auto& info : tabs) {
            auto buttonSprite = ButtonSprite::create(info.name, "goldFont.fnt", "GJ_button_01.png", 0.7f);
            
            auto button = CCMenuItemSpriteExtra::create(
                buttonSprite,
                this,
                menu_selector(CustomSettingsPopup::onTabClicked)
            );
            button->setTag(static_cast<int>(info.tab));
            
            m_tabButtons[info.tab] = button;
            m_tabMenu->addChild(button);
        }
        
        m_tabMenu->updateLayout();
    }
    
    void onTabClicked(CCObject* sender) {
        auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto tab = static_cast<SettingsTab>(button->getTag());
        showTab(tab);
    }
    
    void showTab(SettingsTab tab) {
        m_currentTab = tab;
        
        // Update tab button states
        for (auto& [tabType, button] : m_tabButtons) {
            button->setEnabled(tabType != tab);
        }
        
        // Clear current content
        m_contentNode->removeAllChildren();
        
        // Create content based on tab
        switch (tab) {
            case SettingsTab::RANDOMIZER:
                createRandomizerContent();
                break;
            case SettingsTab::RGB:
                createRGBContent();
                break;
            case SettingsTab::COLORS:
                createColorsContent();
                break;
            case SettingsTab::PARTICLES:
                createParticlesContent();
                break;
            case SettingsTab::LEGACY:
                createLegacyContent();
                break;
            case SettingsTab::RENDERER:
                createRendererContent();
                break;
            case SettingsTab::MISC:
                createMiscContent();
                break;
            case SettingsTab::TOTEM:
                createTotemContent();
                break;
        }
        
        m_scrollLayer->moveToTop();
    }
    
    void createRandomizerContent() {
        auto content = CCMenu::create();
        content->setLayout(ColumnLayout::create()->setGap(5.f)->setAxisReverse(true)->setAxisAlignment(AxisAlignment::Start));
        
        // Main toggles
        content->addChild(createToggle("Enable Conditional Icon Randomizer", "cond-ic-randomizer"));
        content->addChild(createToggle("Enable Live Icon Randomizer", "live-ic-randomizer"));
        
        // Randomizer type selector
        content->addChild(createLabel("Randomizer Type:"));
        content->addChild(createStringSelector("cond-randomizer-type", {
            {"every-gamemode", "Every Gamemode"},
            {"on-death", "On Death"},
            {"both", "Both"},
            {"on-click", "On Click"}
        }));
        
        // Delay slider
        content->addChild(createLabel("Randomize Delay:"));
        content->addChild(createSlider("random-delay", 0.1f, 70.0f, "%.1fs"));
        
        // Gamemode toggles
        content->addChild(createLabel("Randomize Gamemodes:"));
        content->addChild(createToggle("Randomize Cube", "randomize-cube"));
        content->addChild(createToggle("Randomize Ship", "randomize-ship"));
        content->addChild(createToggle("Randomize Ball", "randomize-ball"));
        content->addChild(createToggle("Randomize UFO", "randomize-ufo"));
        content->addChild(createToggle("Randomize Wave", "randomize-wave"));
        content->addChild(createToggle("Randomize Robot", "randomize-robot"));
        content->addChild(createToggle("Randomize Spider", "randomize-spider"));
        content->addChild(createToggle("Randomize Swing", "randomize-swing"));
        
        // Color toggles
        content->addChild(createLabel("Randomize Colors:"));
        content->addChild(createToggle("Randomize Color 1", "randomize-color1"));
        content->addChild(createToggle("Randomize Color 2", "randomize-color2"));
        content->addChild(createToggle("Randomize Glow Color", "randomize-glowcolor"));
        
        float contentHeight = static_cast<float>(content->getChildrenCount()) * 28.0f;
        content->setContentSize({360, contentHeight});
        content->setPosition({0, 0});
        content->updateLayout();
        
        m_contentNode->addChild(content);
        m_scrollLayer->m_contentLayer->setContentSize({360, std::max(contentHeight, m_scrollLayer->getContentSize().height)});
    }
    
    void createRGBContent() {
        auto content = CCMenu::create();
        content->setLayout(ColumnLayout::create()->setGap(5.f)->setAxisReverse(true)->setAxisAlignment(AxisAlignment::Start));
        
        content->addChild(createToggle("Color 1 - RGB", "rgb-col1"));
        content->addChild(createToggle("Color 2 - RGB", "rgb-col2"));
        content->addChild(createToggle("Ignore Player 2", "ignore-p2"));
        
        content->addChild(createLabel("RGB Cycle Speed:"));
        content->addChild(createSlider("rgb-speed", 0.0f, 10.0f, "%.1f"));
        
        content->addChild(createLabel("Saturation - Color 1:"));
        content->addChild(createSlider("rgb-saturation", 0.0f, 1.0f, "%.2f"));
        
        content->addChild(createLabel("Saturation - Color 2:"));
        content->addChild(createSlider("rgb-saturation2", 0.0f, 1.0f, "%.2f"));
        
        float contentHeight = static_cast<float>(content->getChildrenCount()) * 28.0f;
        content->setContentSize({360, contentHeight});
        content->setPosition({0, 0});
        content->updateLayout();
        
        m_contentNode->addChild(content);
        m_scrollLayer->m_contentLayer->setContentSize({360, std::max(contentHeight, m_scrollLayer->getContentSize().height)});
    }
    
    void createColorsContent() {
        auto content = CCMenu::create();
        content->setLayout(ColumnLayout::create()->setGap(5.f)->setAxisReverse(true)->setAxisAlignment(AxisAlignment::Start));
        
        content->addChild(createToggle("Enable Custom Colors", "enable-customcolors"));
        
        content->addChild(createLabel("Player 1 Colors:"));
        content->addChild(createColorPicker("Primary Color", "p1-primary"));
        content->addChild(createColorPicker("Secondary Color", "p1-secondary"));
        content->addChild(createColorPicker("Wave Color", "p1-wave"));
        
        content->addChild(createLabel("Player 2 Colors:"));
        content->addChild(createColorPicker("Primary Color", "p2-primary"));
        content->addChild(createColorPicker("Secondary Color", "p2-secondary"));
        content->addChild(createColorPicker("Wave Color", "p2-wave"));
        
        content->addChild(createToggle("Tint Trail", "tint-trail"));
        content->addChild(createColorPicker("P1 Trail Color", "p1-trail"));
        content->addChild(createColorPicker("P2 Trail Color", "p2-trail"));
        
        content->addChild(createToggle("Tint Ghost Trail", "tint-ghost-trail"));
        content->addChild(createColorPicker("P1 Ghost Trail", "p1-ghost-trail"));
        content->addChild(createColorPicker("P2 Ghost Trail", "p2-ghost-trail"));
        
        content->addChild(createToggle("Tint Dash Fire", "tint-dashfire"));
        content->addChild(createColorPicker("P1 Dash Fire", "p1-dashfire"));
        content->addChild(createColorPicker("P2 Dash Fire", "p2-dashfire"));
        
        float contentHeight = static_cast<float>(content->getChildrenCount()) * 28.0f;
        content->setContentSize({360, contentHeight});
        content->setPosition({0, 0});
        content->updateLayout();
        
        m_contentNode->addChild(content);
        m_scrollLayer->m_contentLayer->setContentSize({360, std::max(contentHeight, m_scrollLayer->getContentSize().height)});
    }
    
    void createParticlesContent() {
        auto content = CCMenu::create();
        content->setLayout(ColumnLayout::create()->setGap(5.f)->setAxisReverse(true)->setAxisAlignment(AxisAlignment::Start));
        
        content->addChild(createToggle("Override All Variances", "override-all-variances"));
        content->addChild(createToggle("Tint Main Particles", "tint-mainparticles"));
        content->addChild(createToggle("Tint UFO Click Particles", "tint-ufo-click-particles"));
        
        content->addChild(createLabel("Main Particles - P1:"));
        content->addChild(createColorPicker("Start Color", "p1-main-particles-start"));
        content->addChild(createColorPicker("End Color", "p1-main-particles-end"));
        
        content->addChild(createLabel("Main Particles - P2:"));
        content->addChild(createColorPicker("Start Color", "p2-main-particles-start"));
        content->addChild(createColorPicker("End Color", "p2-main-particles-end"));
        
        content->addChild(createLabel("UFO Click Particles - P1:"));
        content->addChild(createColorPicker("Start Color", "p1-ufo-click-particles-start"));
        content->addChild(createColorPicker("End Color", "p1-ufo-click-particles-end"));
        
        content->addChild(createLabel("UFO Click Particles - P2:"));
        content->addChild(createColorPicker("Start Color", "p2-ufo-click-particles-start"));
        content->addChild(createColorPicker("End Color", "p2-ufo-click-particles-end"));
        
        float contentHeight = static_cast<float>(content->getChildrenCount()) * 28.0f;
        content->setContentSize({360, contentHeight});
        content->setPosition({0, 0});
        content->updateLayout();
        
        m_contentNode->addChild(content);
        m_scrollLayer->m_contentLayer->setContentSize({360, std::max(contentHeight, m_scrollLayer->getContentSize().height)});
    }
    
    void createLegacyContent() {
        auto content = CCMenu::create();
        content->setLayout(ColumnLayout::create()->setGap(5.f)->setAxisReverse(true)->setAxisAlignment(AxisAlignment::Start));
        
        content->addChild(createToggle("Enable Legacy Trail", "use-legacytrail"));
        content->addChild(createLabel("Trail: Max Points"));
        content->addChild(createSlider("max-points", 2, 20, "%.0f"));
        
        content->addChild(createToggle("Enable Legacy Pulse", "legacy-pulse"));
        content->addChild(createLabel("Pulse: FPS"));
        content->addChild(createSlider("pulse-fps", 60, 1080, "%.0f"));
        
        float contentHeight = static_cast<float>(content->getChildrenCount()) * 28.0f;
        content->setContentSize({360, contentHeight});
        content->setPosition({0, 0});
        content->updateLayout();
        
        m_contentNode->addChild(content);
        m_scrollLayer->m_contentLayer->setContentSize({360, std::max(contentHeight, m_scrollLayer->getContentSize().height)});
    }
    
    void createRendererContent() {
        auto content = CCMenu::create();
        content->setLayout(ColumnLayout::create()->setGap(5.f)->setAxisReverse(true)->setAxisAlignment(AxisAlignment::Start));
        
        content->addChild(createToggle("Renderer Features", "enable-renderer"));
        
        content->addChild(createLabel("Max Icons per Row:"));
        content->addChild(createSlider("max-per-row", 1, 50, "%.0f"));
        
        content->addChild(createLabel("Render Background Color:"));
        content->addChild(createColorPicker("Background", "bg-color"));
        
        float contentHeight = static_cast<float>(content->getChildrenCount()) * 28.0f;
        content->setContentSize({360, contentHeight});
        content->setPosition({0, 0});
        content->updateLayout();
        
        m_contentNode->addChild(content);
        m_scrollLayer->m_contentLayer->setContentSize({360, std::max(contentHeight, m_scrollLayer->getContentSize().height)});
    }
    
    void createMiscContent() {
        auto content = CCMenu::create();
        content->setLayout(ColumnLayout::create()->setGap(5.f)->setAxisReverse(true)->setAxisAlignment(AxisAlignment::Start));
        
        content->addChild(createToggle("Garage: Icon Select Animation", "garage-select-anim"));
        content->addChild(createToggle("Gameplay: Ball Rotation Bug", "ballrot-bug"));
        content->addChild(createToggle("Icon Listing Features", "iconlist-ft"));
        
        float contentHeight = static_cast<float>(content->getChildrenCount()) * 28.0f;
        content->setContentSize({360, contentHeight});
        content->setPosition({0, 0});
        content->updateLayout();
        
        m_contentNode->addChild(content);
        m_scrollLayer->m_contentLayer->setContentSize({360, std::max(contentHeight, m_scrollLayer->getContentSize().height)});
    }
    
    void createTotemContent() {
        auto content = CCMenu::create();
        content->setLayout(ColumnLayout::create()->setGap(5.f)->setAxisReverse(true)->setAxisAlignment(AxisAlignment::Start));
        
        content->addChild(createToggle("Enable Totem Animation", "enable-totem"));
        
        content->addChild(createLabel("Totem Cooldown:"));
        content->addChild(createSlider("totem-cooldown", 0.1f, 2.0f, "%.1fs"));
        
        content->addChild(createLabel("Totem Scale:"));
        content->addChild(createSlider("totem-scale", 0.3f, 4.0f, "%.1f"));
        
        float contentHeight = static_cast<float>(content->getChildrenCount()) * 28.0f;
        content->setContentSize({360, contentHeight});
        content->setPosition({0, 0});
        content->updateLayout();
        
        m_contentNode->addChild(content);
        m_scrollLayer->m_contentLayer->setContentSize({360, std::max(contentHeight, m_scrollLayer->getContentSize().height)});
    }
    
    CCNode* createToggle(const char* label, const char* settingKey) {
        auto container = CCNode::create();
        container->setContentSize({350, 25});
        
        auto labelNode = CCLabelBMFont::create(label, "bigFont.fnt");
        labelNode->setScale(0.35f);
        labelNode->setPosition({5, 12});
        labelNode->setAnchorPoint({0, 0.5f});
        container->addChild(labelNode);
        
        auto toggle = CCMenuItemToggler::create(
            CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
            CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
            this,
            menu_selector(CustomSettingsPopup::onToggleChanged)
        );
        toggle->setScale(0.8f);
        
        bool currentValue = Mod::get()->getSettingValue<bool>(settingKey);
        toggle->toggle(currentValue);
        toggle->setUserObject(CCString::create(settingKey));
        
        auto toggleMenu = CCMenu::create();
        toggleMenu->addChild(toggle);
        toggleMenu->setPosition({320, 12});
        container->addChild(toggleMenu);
        
        return container;
    }
    
    CCNode* createSlider(const char* settingKey, float min, float max, const char* format) {
        auto container = CCNode::create();
        container->setContentSize({350, 25});
        
        auto slider = Slider::create(this, menu_selector(CustomSettingsPopup::onSliderChanged));
        slider->setScale(0.7f);
        slider->setPosition({175, 12});
        slider->setUserObject(CCString::create(settingKey));
        
        // Get current value and set slider
        float currentValue;
        if (settingKey == std::string("random-delay") || settingKey == std::string("rgb-speed") || 
            settingKey == std::string("rgb-saturation") || settingKey == std::string("rgb-saturation2") ||
            settingKey == std::string("totem-cooldown") || settingKey == std::string("totem-scale")) {
            currentValue = static_cast<float>(Mod::get()->getSettingValue<double>(settingKey));
        } else {
            currentValue = static_cast<float>(Mod::get()->getSettingValue<int64_t>(settingKey));
        }
        slider->setValue((currentValue - min) / (max - min));
        
        auto valueLabel = CCLabelBMFont::create("", "bigFont.fnt");
        valueLabel->setScale(0.3f);
        valueLabel->setPosition({290, 12});
        valueLabel->setTag(1001);
        container->addChild(valueLabel);
        
        // Update label with initial value
        std::string key = settingKey;
        float actualMin, actualMax;
        const char* actualFormat;
        getSliderParams(key, actualMin, actualMax, actualFormat);
        valueLabel->setString(CCString::createWithFormat(actualFormat, currentValue)->getCString());
        
        container->addChild(slider);
        
        return container;
    }
    
    CCNode* createLabel(const char* text) {
        auto label = CCLabelBMFont::create(text, "goldFont.fnt");
        label->setScale(0.5f);
        return label;
    }
    
    CCNode* createStringSelector(const char* settingKey, std::vector<std::pair<std::string, std::string>> options) {
        auto container = CCNode::create();
        container->setContentSize({350, 25});
        
        std::string currentValue = Mod::get()->getSettingValue<std::string>(settingKey);
        std::string displayText = currentValue;
        
        // Find display text for current value
        for (auto& option : options) {
            if (option.first == currentValue) {
                displayText = option.second;
                break;
            }
        }
        
        auto buttonSprite = ButtonSprite::create(displayText.c_str(), "bigFont.fnt", "GJ_button_01.png", 0.5f);
        auto button = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(CustomSettingsPopup::onStringOptionClicked)
        );
        button->setUserObject(CCString::create(settingKey));
        
        auto menu = CCMenu::create();
        menu->addChild(button);
        menu->setPosition({175, 12});
        container->addChild(menu);
        
        return container;
    }
    
    CCNode* createColorPicker(const char* label, const char* settingKey) {
        auto container = CCNode::create();
        container->setContentSize({350, 25});
        
        auto labelNode = CCLabelBMFont::create(label, "bigFont.fnt");
        labelNode->setScale(0.35f);
        labelNode->setPosition({5, 12});
        labelNode->setAnchorPoint({0, 0.5f});
        container->addChild(labelNode);
        
        // Create color display sprite
        auto colorSprite = CCSprite::create("whiteSquare60_001.png");
        colorSprite->setScale(0.4f);
        
        // Get current color value
        ccColor3B currentColor = Mod::get()->getSettingValue<ccColor3B>(settingKey);
        colorSprite->setColor(currentColor);
        
        auto button = CCMenuItemSpriteExtra::create(
            colorSprite,
            this,
            menu_selector(CustomSettingsPopup::onColorClicked)
        );
        button->setUserObject(CCString::create(settingKey));
        
        auto menu = CCMenu::create();
        menu->addChild(button);
        menu->setPosition({320, 12});
        container->addChild(menu);
        
        return container;
    }
    
    void onToggleChanged(CCObject* sender) {
        auto toggle = static_cast<CCMenuItemToggler*>(sender);
        auto settingKey = static_cast<CCString*>(toggle->getUserObject())->getCString();
        
        Mod::get()->setSettingValue<bool>(settingKey, !toggle->isToggled());
    }
    
    void onSliderChanged(CCObject* sender) {
        auto slider = static_cast<Slider*>(sender);
        auto settingKeyObj = slider->getUserObject();
        
        if (!settingKeyObj) {
            geode::log::error("Slider has no user object!");
            return;
        }
        
        auto settingKey = static_cast<CCString*>(settingKeyObj)->getCString();
        
        // Update the appropriate setting based on type
        std::string key = settingKey;
        if (key == "random-delay" || key == "rgb-speed" || key == "rgb-saturation" || 
            key == "rgb-saturation2" || key == "totem-cooldown" || key == "totem-scale") {
            float min, max;
            const char* format;
            getSliderParams(key, min, max, format);
            float value = min + slider->getValue() * (max - min);
            Mod::get()->setSettingValue<double>(settingKey, value);
        } else {
            float min, max;
            const char* format;
            getSliderParams(key, min, max, format);
            int64_t value = static_cast<int64_t>(min + slider->getValue() * (max - min));
            Mod::get()->setSettingValue<int64_t>(settingKey, value);
        }
        
        updateSliderLabel(slider, 0, 0, "");
    }
    
    void onStringOptionClicked(CCObject* sender) {
        auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto settingKey = static_cast<CCString*>(button->getUserObject())->getCString();
        
        // Get current options for randomizer type
        if (std::string(settingKey) == "cond-randomizer-type") {
            std::vector<std::pair<std::string, std::string>> options = {
                {"every-gamemode", "Every Gamemode"},
                {"on-death", "On Death"},
                {"both", "Both"},
                {"on-click", "On Click"}
            };
            
            std::string currentValue = Mod::get()->getSettingValue<std::string>(settingKey);
            
            // Find current index and cycle to next
            for (size_t i = 0; i < options.size(); i++) {
                if (options[i].first == currentValue) {
                    size_t nextIndex = (i + 1) % options.size();
                    std::string newValue = options[nextIndex].first;
                    std::string newDisplay = options[nextIndex].second;
                    
                    Mod::get()->setSettingValue<std::string>(settingKey, newValue);
                    
                    // Update button text
                    auto buttonSprite = ButtonSprite::create(newDisplay.c_str(), "bigFont.fnt", "GJ_button_01.png", 0.5f);
                    button->setNormalImage(buttonSprite);
                    break;
                }
            }
        }
    }
    
    void onColorClicked(CCObject* sender) {
        auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto settingKey = static_cast<CCString*>(button->getUserObject())->getCString();
        
        // Simple color cycling for demonstration - you could implement a proper color picker
        ccColor3B currentColor = Mod::get()->getSettingValue<ccColor3B>(settingKey);
        
        // Create a simple color selection array
        std::vector<ccColor3B> colors = {
            {255, 255, 255}, // White
            {255, 0, 0},     // Red
            {0, 255, 0},     // Green
            {0, 0, 255},     // Blue
            {255, 255, 0},   // Yellow
            {255, 0, 255},   // Magenta
            {0, 255, 255},   // Cyan
            {128, 128, 128}, // Gray
            {0, 0, 0}        // Black
        };
        
        // Find current color and cycle to next
        for (size_t i = 0; i < colors.size(); i++) {
            if (colors[i].r == currentColor.r && colors[i].g == currentColor.g && colors[i].b == currentColor.b) {
                size_t nextIndex = (i + 1) % colors.size();
                ccColor3B newColor = colors[nextIndex];
                
                Mod::get()->setSettingValue<ccColor3B>(settingKey, newColor);
                
                // Update button color
                auto colorSprite = static_cast<CCSprite*>(button->getNormalImage());
                colorSprite->setColor(newColor);
                break;
            }
        }
    }
    
    void getSliderParams(const std::string& key, float& min, float& max, const char*& format) {
        if (key == "random-delay") { min = 0.1f; max = 70.0f; format = "%.1fs"; }
        else if (key == "rgb-speed") { min = 0.0f; max = 10.0f; format = "%.1f"; }
        else if (key == "rgb-saturation" || key == "rgb-saturation2") { min = 0.0f; max = 1.0f; format = "%.2f"; }
        else if (key == "totem-cooldown") { min = 0.1f; max = 2.0f; format = "%.1fs"; }
        else if (key == "totem-scale") { min = 0.3f; max = 4.0f; format = "%.1f"; }
        else if (key == "max-points") { min = 2; max = 20; format = "%.0f"; }
        else if (key == "pulse-fps") { min = 60; max = 1080; format = "%.0f"; }
        else if (key == "max-per-row") { min = 1; max = 50; format = "%.0f"; }
        else { min = 0; max = 100; format = "%.0f"; }
    }
    
    void updateSliderLabel(Slider* slider, float min, float max, const char* format) {
        auto container = slider->getParent(); // This is the container node
        auto label = static_cast<CCLabelBMFont*>(container->getChildByTag(1001));
        
        if (label) {
            std::string key = static_cast<CCString*>(slider->getUserObject())->getCString();
            float actualMin, actualMax;
            const char* actualFormat;
            getSliderParams(key, actualMin, actualMax, actualFormat);
            
            float value = actualMin + slider->getValue() * (actualMax - actualMin);
            label->setString(CCString::createWithFormat(actualFormat, value)->getCString());
        }
    }

public:
    static CustomSettingsPopup* create() {
        auto ret = new CustomSettingsPopup();
        if (ret && ret->initAnchored(400, 280)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

// MenuLayer modification
class $modify(SettingsMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        
        auto menu = this->getChildByID("bottom-menu");
        if (!menu) {
            menu = CCMenu::create();
            menu->setPosition({20, 20});
            this->addChild(menu);
        }
        
        auto sprite = CircleButtonSprite::createWithSprite("LaVerdadMeDaMuchaPutaHuevaAprenderAUsarElCosoQueHaceUnBotonConDosCapasLanfao.png"_spr, 1.0f, CircleBaseColor::Green, CircleBaseSize::Medium);
        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(SettingsMenuLayer::onOpenSettings)
        );
        button->setPosition({30, 30});
        
        menu->addChild(button);
        
        return true;
    }
    
    void onOpenSettings(CCObject*) {
        CustomSettingsPopup::create()->show();
    }
};

// GJGarageLayer modification  
class $modify(SettingsGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) return false;
        
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto menu = CCMenu::create();
        
        auto sprite = CircleButtonSprite::createWithSprite("LaVerdadMeDaMuchaPutaHuevaAprenderAUsarElCosoQueHaceUnBotonConDosCapasLanfao.png"_spr, 1.0f, CircleBaseColor::Green, CircleBaseSize::Small);
        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(SettingsGarageLayer::onOpenSettings)
        );
        
        menu->addChild(button);
        menu->setPosition({winSize.width - 30, winSize.height - 30});
        this->addChild(menu);
        
        return true;
    }
    
    void onOpenSettings(CCObject*) {
        CustomSettingsPopup::create()->show();
    }
};

// PauseLayer modification
class $modify(SettingsPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        
        auto menu = this->getChildByID("left-button-menu");
        if (!menu) {
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            menu = CCMenu::create();
            menu->setLayout(ColumnLayout::create()->setGap(4.f)->setAxisAlignment(AxisAlignment::End)->setAxisReverse(true));
            menu->setPosition({36.f, winSize.height / 2});
            menu->setContentSize({40, winSize.height - 40.f});
            menu->setZOrder(10);
            this->addChild(menu);
        }
        
        auto sprite = CCSprite::create("LaVerdadMeDaMuchaPutaHuevaAprenderAUsarElCosoQueHaceUnBotonConDosCapasLanfao.png"_spr);
        sprite->setScale(0.7f);
        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(SettingsPauseLayer::onOpenSettings)
        );
        button->setID("personaltweaks-settings-shortcut"_spr);
        
        menu->addChild(button);
        menu->updateLayout();
    }
    
    void onOpenSettings(CCObject*) {
        CustomSettingsPopup::create()->show();
    }
};