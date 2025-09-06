#include "CustomSettingsPopup.hpp"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>

using namespace geode::prelude;

CustomSettingsPopup* CustomSettingsPopup::create() {
    auto ret = new CustomSettingsPopup();
    // use requested background and make popup slightly smaller so the ScrollLayer better covers it
    if (ret->initAnchored(420.0f, 280.0f, "geode.loader/GE_square03.png")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool CustomSettingsPopup::setup() {
    this->setTitle("Personal Tweaks Settings");
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto contentSize = this->m_mainLayer->getContentSize();
    
    // Create main scroll layer
    float contentW = contentSize.width - 40.0f;
    // reduce vertical padding so scroll area is larger
    float contentH = contentSize.height - 40.0f;
    auto scrollLayer = ScrollLayer::create({contentW, contentH});
    // center the scroll within the main layer padding
    scrollLayer->setPosition({20.0f, 20.0f});
    
    // Create content node for all sections
    auto contentNode = CCNode::create();
    // Start from top inside the scroll area (leave a small top padding)
    float currentY = contentH - 10.0f;
    float startY = currentY;

    // Add sections with spacing
    const float spacing = 10.0f;

    auto randomizerSection = createRandomizerSection();
    // center horizontally: sections assume 0 is center in their internals
    randomizerSection->setPosition({contentW / 2.0f, currentY});
    contentNode->addChild(randomizerSection);
    currentY -= 180.0f + spacing;
    
    auto rgbSection = createRGBSection();
    rgbSection->setPosition({contentW / 2.0f, currentY});
    contentNode->addChild(rgbSection);
    currentY -= 140.0f + spacing;
    
    auto customColorsSection = createCustomColorsSection();
    customColorsSection->setPosition({contentW / 2.0f, currentY});
    contentNode->addChild(customColorsSection);
    currentY -= 60.0f + spacing;
    
    // Compute total content height (ensure at least scroll area height so content isn't smaller)
    float totalHeight = startY - currentY + 10.0f;
    if (totalHeight < contentH) totalHeight = contentH;
    
    // Set content size and add to scroll layer
    contentNode->setContentSize({contentW, totalHeight});
    scrollLayer->m_contentLayer->addChild(contentNode);
    scrollLayer->m_contentLayer->setContentSize({contentW, totalHeight});

    this->m_mainLayer->addChild(scrollLayer);
    
    return true;
}

CCNode* CustomSettingsPopup::createRandomizerSection() {
    auto section = CCNode::create();
    auto menu = CCMenu::create();
    menu->setPosition(CCPointZero);
    section->addChild(menu);
    
    // Section title
    auto title = CCLabelBMFont::create("Randomizer", "bigFont.fnt");
    title->setPosition({0.0f, 150.0f});
    title->setScale(0.7f);
    section->addChild(title);
    
    // Randomizer type toggles
    auto condLabel = CCLabelBMFont::create("Conditional", "chatFont.fnt");
    condLabel->setPosition({-150.0f, 120.0f});
    condLabel->setScale(0.6f);
    section->addChild(condLabel);
    
    auto condToggler = createToggler("cond-ic-randomizer", {-50.0f, 120.0f});
    menu->addChild(condToggler);
    
    auto liveLabel = CCLabelBMFont::create("Live", "chatFont.fnt");
    liveLabel->setPosition({50.0f, 120.0f});
    liveLabel->setScale(0.6f);
    section->addChild(liveLabel);
    
    auto liveToggler = createToggler("live-ic-randomizer", {120.0f, 120.0f});
    menu->addChild(liveToggler);
    
    // Delay slider
    auto delayLabel = CCLabelBMFont::create("Delay", "chatFont.fnt");
    delayLabel->setPosition({-180.0f, 90.0f});
    delayLabel->setScale(0.5f);
    section->addChild(delayLabel);
    
    auto delaySlider = createSlider("random-delay", {0.0f, 90.0f}, 150.0f);
    // ADD slider to the menu so touches reach it (CCMenu otherwise swallows touches)
    menu->addChild(delaySlider);
    
    // Icon type toggles (arranged in rows)
    std::vector<std::pair<std::string, std::string>> iconTypes = {
        {"randomize-cube", "Cube"}, {"randomize-ship", "Ship"},
        {"randomize-ball", "Ball"}, {"randomize-ufo", "UFO"},
        {"randomize-wave", "Wave"}, {"randomize-robot", "Robot"},
        {"randomize-spider", "Spider"}, {"randomize-swing", "Swing"}
    };
    
    for (int i = 0; i < (int)iconTypes.size(); i++) {
        int row = i / 4;
        int col = i % 4;
        float x = -150.0f + col * 100.0f;
        float y = 50.0f - row * 35.0f;
        
        auto label = CCLabelBMFont::create(iconTypes[i].second.c_str(), "chatFont.fnt");
        label->setPosition({x, y + 15.0f});
        label->setScale(0.4f);
        section->addChild(label);
        
        auto toggler = createToggler(iconTypes[i].first, {x, y - 5.0f});
        menu->addChild(toggler);
    }
    
    // Color toggles
    std::vector<std::pair<std::string, std::string>> colorTypes = {
        {"randomize-color1", "Color 1"}, {"randomize-color2", "Color 2"}, {"randomize-glowcolor", "Glow"}
    };
    
    for (int i = 0; i < (int)colorTypes.size(); i++) {
        float x = -100.0f + i * 100.0f;
        float y = -30.0f;
        
        auto label = CCLabelBMFont::create(colorTypes[i].second.c_str(), "chatFont.fnt");
        label->setPosition({x, y + 15.0f});
        label->setScale(0.4f);
        section->addChild(label);
        
        auto toggler = createToggler(colorTypes[i].first, {x, y - 5.0f});
        menu->addChild(toggler);
    }
    
    return section;
}

CCNode* CustomSettingsPopup::createRGBSection() {
    auto section = CCNode::create();
    auto menu = CCMenu::create();
    menu->setPosition(CCPointZero);
    section->addChild(menu);
    
    // Section title
    auto title = CCLabelBMFont::create("RGB Icons", "bigFont.fnt");
    title->setPosition({0.0f, 110.0f});
    title->setScale(0.7f);
    section->addChild(title);
    
    // RGB toggles
    auto col1Label = CCLabelBMFont::create("Color 1 RGB", "chatFont.fnt");
    col1Label->setPosition({-120.0f, 80.0f});
    col1Label->setScale(0.5f);
    section->addChild(col1Label);
    
    auto col1Toggler = createToggler("rgb-col1", {-20.0f, 80.0f});
    menu->addChild(col1Toggler);
    
    auto col2Label = CCLabelBMFont::create("Color 2 RGB", "chatFont.fnt");
    col2Label->setPosition({80.0f, 80.0f});
    col2Label->setScale(0.5f);
    section->addChild(col2Label);
    
    auto col2Toggler = createToggler("rgb-col2", {180.0f, 80.0f});
    menu->addChild(col2Toggler);
    
    // RGB Speed slider
    auto speedLabel = CCLabelBMFont::create("Speed", "chatFont.fnt");
    speedLabel->setPosition({-180.0f, 50.0f});
    speedLabel->setScale(0.5f);
    section->addChild(speedLabel);
    
    auto speedSlider = createSlider("rgb-speed", {0.0f, 50.0f});
    menu->addChild(speedSlider);
    
    // Saturation sliders
    auto sat1Label = CCLabelBMFont::create("Sat 1", "chatFont.fnt");
    sat1Label->setPosition({-150.0f, 20.0f});
    sat1Label->setScale(0.45f);
    section->addChild(sat1Label);
    
    auto sat1Slider = createSlider("rgb-saturation", {-50.0f, 20.0f}, 120.0f);
    menu->addChild(sat1Slider);
    
    auto sat2Label = CCLabelBMFont::create("Sat 2", "chatFont.fnt");
    sat2Label->setPosition({50.0f, 20.0f});
    sat2Label->setScale(0.45f);
    section->addChild(sat2Label);
    
    auto sat2Slider = createSlider("rgb-saturation2", {130.0f, 20.0f}, 120.0f);
    menu->addChild(sat2Slider);
    
    // Ignore P2 toggle
    auto ignoreLabel = CCLabelBMFont::create("Ignore P2", "chatFont.fnt");
    ignoreLabel->setPosition({-60.0f, -10.0f});
    ignoreLabel->setScale(0.5f);
    section->addChild(ignoreLabel);
    
    auto ignoreToggler = createToggler("ignore-p2", {40.0f, -10.0f});
    menu->addChild(ignoreToggler);
    
    return section;
}

CCNode* CustomSettingsPopup::createCustomColorsSection() {
    auto section = CCNode::create();
    auto menu = CCMenu::create();
    menu->setPosition(CCPointZero);
    section->addChild(menu);
    
    // Section title
    auto title = CCLabelBMFont::create("Custom Colors", "bigFont.fnt");
    title->setPosition({0.0f, 30.0f});
    title->setScale(0.7f);
    section->addChild(title);
    
    // Enable custom colors toggle
    auto enableLabel = CCLabelBMFont::create("Enable Custom Colors", "chatFont.fnt");
    enableLabel->setPosition({-80.0f, 0.0f});
    enableLabel->setScale(0.5f);
    section->addChild(enableLabel);
    
    auto enableToggler = createToggler("enable-customcolors", {80.0f, 0.0f});
    menu->addChild(enableToggler);
    
    return section;
}

CCMenuItemToggler* CustomSettingsPopup::createToggler(std::string settingId, CCPoint position) {
    auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    
    offSprite->setScale(0.8f);
    onSprite->setScale(0.8f);
    
    auto toggler = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(CustomSettingsPopup::onRandomizerToggle));
    toggler->setPosition(position);

    // use stable hash-based tag (match original working approach)
    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    toggler->setTag(tag);
    this->m_tagToSetting[tag] = settingId;

    // set visual initial state from the mod
    bool initial = Mod::get()->getSettingValue<bool>(settingId);
    toggler->toggle(initial);

    // store id as user object too (fallback)
    toggler->setUserObject(CCString::create(settingId));
    
    return toggler;
}

Slider* CustomSettingsPopup::createSlider(std::string settingId, CCPoint position, float width) {
    auto slider = Slider::create(this, menu_selector(CustomSettingsPopup::onDelaySlider), 0.8f);
    auto m_thumb = slider->getThumb();
    slider->setPosition(position);
    slider->m_sliderBar->setContentSize({width, slider->m_sliderBar->getContentSize().height});
    slider->setUserObject(CCString::create(settingId));
    
    // tag/map using hashed id (fallback uses userObject)
    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    slider->setTag(tag);
    this->m_tagToSetting[tag] = settingId;

    // Also apply the same tag/userObject to the thumb so callbacks coming from the thumb still carry the id
    if (m_thumb) {
        m_thumb->setUserObject(CCString::create(settingId));
        m_thumb->setTag(tag);
    }
    
    // Set initial value based on setting type
    if (settingId == "random-delay") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue((value - 0.1f) / (70.0f - 0.1f));
    } else if (settingId == "rgb-speed") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue(value / 10.0f);
    } else if (settingId == "rgb-saturation" || settingId == "rgb-saturation2") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue(value);
    }
    
    return slider;
}

void CustomSettingsPopup::onRandomizerToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    if (!toggler) return;

    std::string settingIdStr;

    // prefer our mapping by tag
    int tag = toggler->getTag();
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) {
        settingIdStr = it->second;
    } else {
        // fallback to userObject (old behavior)
        auto obj = toggler->getUserObject();
        if (!obj) return;
        if (auto cs = dynamic_cast<CCString*>(obj)) {
            settingIdStr = std::string(cs->getCString());
        } else {
            return;
        }
    }
    
    // Note: original working code saved !isToggled() — replicate that behavior
    bool newValue = !toggler->isToggled();
    Mod::get()->setSettingValue<bool>(settingIdStr, newValue);

    // debug log
    geode::log::debug("PersonalTweaks: toggled {} -> {}", settingIdStr, newValue ? "true" : "false");
}

void CustomSettingsPopup::onDelaySlider(CCObject* sender) {
    // Support being called from either the Slider or its Thumb
    auto slider = dynamic_cast<Slider*>(sender);
    CCObject* userObj = nullptr;
    int tag = 0;
    float value = 0.0f;

    if (slider) {
        userObj = slider->getUserObject();
        tag = slider->getTag();
        value = slider->getValue();
    } else {
        // maybe it's the thumb that triggered the callback
        auto thumb = dynamic_cast<SliderThumb*>(sender);
        if (thumb) {
            userObj = thumb->getUserObject();
            tag = thumb->getTag();
            // thumb might expose getValue() directly
            value = thumb->getValue();
            // try to find parent Slider for completeness (some implementations keep the slider as parent)
            if (!slider) {
                if (auto parentNode = dynamic_cast<CCNode*>(thumb->getParent())) {
                    slider = dynamic_cast<Slider*>(parentNode);
                }
            }
        } else {
            return; // unknown sender
        }
    }

    std::string settingIdStr;
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) {
        settingIdStr = it->second;
    } else if (userObj) {
        if (auto cs = dynamic_cast<CCString*>(userObj)) {
            settingIdStr = std::string(cs->getCString());
        } else {
            return;
        }
    } else {
        return;
    }

    // Use the computed 'value' (0..1 slider range) and map it per-setting
    if (settingIdStr == "random-delay") {
        float mapped = 0.1f + value * (70.0f - 0.1f);
        Mod::get()->setSettingValue<double>(settingIdStr, mapped);
        geode::log::debug("PersonalTweaks: {} = {}", settingIdStr, mapped);
    } else if (settingIdStr == "rgb-speed") {
        float mapped = value * 10.0f;
        Mod::get()->setSettingValue<double>(settingIdStr, mapped);
        geode::log::debug("PersonalTweaks: {} = {}", settingIdStr, mapped);
    } else if (settingIdStr == "rgb-saturation" || settingIdStr == "rgb-saturation2") {
        Mod::get()->setSettingValue<double>(settingIdStr, value);
        geode::log::debug("PersonalTweaks: {} = {}", settingIdStr, value);
    }
}

void CustomSettingsPopup::onRGBToggle(CCObject* sender) {
    onRandomizerToggle(sender); // Same implementation
}

void CustomSettingsPopup::onRGBSpeedSlider(CCObject* sender) {
    onDelaySlider(sender); // Same implementation
}

void CustomSettingsPopup::onRGBSaturationSlider(CCObject* sender) {
    onDelaySlider(sender); // Same implementation
}

// Add button to MenuLayer
class $modify(CustomMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        
        auto settingsSprite = CCSprite::create("PTMenuAccess.png"_spr);
        settingsSprite->setScale(0.8f);
        
        auto settingsBtn = CCMenuItemSpriteExtra::create(
            settingsSprite,
            this,
            menu_selector(CustomMenuLayer::onOpenSettings)
        );
        
        auto menu = this->getChildByID("bottom-menu");
        if (menu) {
            settingsBtn->setID("personal-tweaks-btn"_spr);
            menu->addChild(settingsBtn);
            menu->updateLayout();
        }
        
        return true;
    }
    
    void onOpenSettings(CCObject*) {
        CustomSettingsPopup::create()->show();
    }
};

// Add button to PauseLayer
class $modify(CustomPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        
        auto settingsSprite = CCSprite::create("PTMenuAccess.png"_spr);
        settingsSprite->setScale(0.7f);
        
        auto settingsBtn = CCMenuItemSpriteExtra::create(
            settingsSprite,
            this,
            menu_selector(CustomPauseLayer::onOpenSettings)
        );
        
        settingsBtn->setPosition({-150.0f, -65.0f});
        
        auto menu = this->getChildByID("left-button-menu");
        if (menu) {
            settingsBtn->setID("personal-tweaks-btn"_spr);
            menu->addChild(settingsBtn);
        }
    }
    
    void onOpenSettings(CCObject*) {
        CustomSettingsPopup::create()->show();
    }
};

// Add button to GJGarageLayer
class $modify(CustomGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) return false;
        
        auto settingsSprite = CCSprite::create("PTMenuAccess.png"_spr);
        settingsSprite->setScale(0.7f);
        
        auto settingsBtn = CCMenuItemSpriteExtra::create(
            settingsSprite,
            this,
            menu_selector(CustomGarageLayer::onOpenSettings)
        );
        
        settingsBtn->setPosition({-200.0f, 100.0f});
        
        auto menu = CCMenu::create();
        menu->addChild(settingsBtn);
        menu->setPosition(CCDirector::sharedDirector()->getWinSize() / 2);
        this->addChild(menu);
        
        return true;
    }
    
    void onOpenSettings(CCObject*) {
        CustomSettingsPopup::create()->show();
    }
};