#include "CustomSettingsPopup.hpp"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>

using namespace geode::prelude;

// ===== MAIN POPUP =====

CustomSettingsPopup* CustomSettingsPopup::create() {
    auto ret = new CustomSettingsPopup();
    // use requested background and make popup slightly smaller so the ScrollLayer better covers it
    if (ret->initAnchored(420.0f, 280.0f, "SquareThing02.png"_spr)) {
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
    
    // No ScrollLayer: create discrete section nodes and place them in the popup
    auto size = contentSize;
    const float topY = size.height;
    const float centerX = size.width / 2.0f;

    // Randomizer section (left title + three buttons to the right)
    auto randomizerSection = createRandomizerSection();
    randomizerSection->setPosition({centerX, topY - 60.f});
    this->m_mainLayer->addChild(randomizerSection);

    // RGB Icons section (left title + two buttons to the right)
    auto rgbSection = createRGBSection();
    rgbSection->setPosition({centerX, topY - 95.f});
    this->m_mainLayer->addChild(rgbSection);

    // Custom Colors section stays below RGB (if needed)
    auto customColorsSection = createCustomColorsSection();
    customColorsSection->setPosition({centerX, topY - 130.0f});
    this->m_mainLayer->addChild(customColorsSection);
     
    return true;
}

CCNode* CustomSettingsPopup::createRandomizerSection() {
    auto section = CCNode::create();
    // make section sized to popup and centered content
    auto contentSize = this->m_mainLayer->getContentSize();
    const float sectionH = 40.f;
    section->setContentSize({contentSize.width, sectionH});
    section->setAnchorPoint({0.5f, 0.5f});
    section->setID("randomizer-section"_spr);

    // Left-aligned title
    auto title = CCLabelBMFont::create("Randomizer", "bigFont.fnt");
    title->setAnchorPoint({0.f, 0.5f});
    title->setPosition({10.0f, sectionH / 2.0f});
    title->setScale(0.7f);
    section->addChild(title);

    // menu holds the sprite buttons (right side)
    auto menu = CCMenu::create();
    menu->setContentSize(section->getContentSize());
    menu->setAnchorPoint({1.0f, 0.5f});
    menu->setPosition({contentSize.width - 10.0f, sectionH / 2.0f});
    menu->setScale(0.7f);
    menu->setID("randomizer-menu"_spr);
    menu->setLayout(
        RowLayout::create()
            ->setGap(20.f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setAxisReverse(false)
            ->setCrossAxisOverflow(true)
            ->setAutoGrowAxis(300.f)
    );
    section->addChild(menu);

    auto condSpr = ButtonSprite::create("Cond.", "bigFont.fnt", "BlackButton.png"_spr, 0.7f);
    auto conditionalBtn = CCMenuItemSpriteExtra::create(condSpr, this, menu_selector(CustomSettingsPopup::onOpenConditional));
    conditionalBtn->setScale(0.8f);
    conditionalBtn->setID("randomizer-cond-btn"_spr);
    menu->addChild(conditionalBtn);

    auto liveSpr = ButtonSprite::create("Live", "bigFont.fnt", "BlackButton.png"_spr, 0.7f);
    auto liveBtn = CCMenuItemSpriteExtra::create(liveSpr, this, menu_selector(CustomSettingsPopup::onOpenLive));
    liveBtn->setScale(0.8f);
    liveBtn->setID("randomizer-live-btn"_spr);
    menu->addChild(liveBtn);

    auto modesSpr = ButtonSprite::create("Modes", "bigFont.fnt", "BlackButton.png"_spr, 0.7f);
    auto modesBtn = CCMenuItemSpriteExtra::create(modesSpr, this, menu_selector(CustomSettingsPopup::onOpenModes));
    modesBtn->setScale(0.8f);
    modesBtn->setID("randomizer-modes-btn"_spr);
    menu->addChild(modesBtn);
    menu->updateLayout();

     return section;
}

CCNode* CustomSettingsPopup::createRGBSection() {
    auto section = CCNode::create();
    auto contentSize = this->m_mainLayer->getContentSize();
    const float sectionH = 40.f;
    section->setContentSize({contentSize.width, sectionH});
    section->setAnchorPoint({0.5f, 0.5f});
    section->setID("rgb-section"_spr);

    // Title left
    auto title = CCLabelBMFont::create("RGB Icons", "bigFont.fnt");
    title->setAnchorPoint({0.f, 0.5f});
    title->setPosition({10.0f, sectionH / 2.0f});
    title->setScale(0.7f);
    section->addChild(title);

    // Right side buttons (Main / Extras)
    auto menu = CCMenu::create();
    menu->setContentSize(section->getContentSize());
    menu->setAnchorPoint({1.0f, 0.5f});
    menu->setPosition({contentSize.width - 10.0f, sectionH / 2.0f});
    menu->setScale(0.7f);
    menu->setID("rgb-menu"_spr);
    menu->setLayout(
        RowLayout::create()
            ->setGap(20.f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setAxisReverse(false)
            ->setCrossAxisOverflow(true)
            ->setAutoGrowAxis(300.f)
    );
    section->addChild(menu);

    auto mainSpr = ButtonSprite::create("Main", "bigFont.fnt", "BlackButton.png"_spr, 0.7f);
    auto mainBtn = CCMenuItemSpriteExtra::create(mainSpr, this, menu_selector(CustomSettingsPopup::onOpenRGBMain));
    mainBtn->setScale(0.8f);
    mainBtn->setID("rgb-main-btn"_spr);
    menu->addChild(mainBtn);

    auto extrasSpr = ButtonSprite::create("Extras", "bigFont.fnt", "BlackButton.png"_spr, 0.7f);
    auto extrasBtn = CCMenuItemSpriteExtra::create(extrasSpr, this, menu_selector(CustomSettingsPopup::onOpenRGBExtras));
    extrasBtn->setScale(0.8f);
    extrasBtn->setID("rgb-extras-btn"_spr);
    menu->addChild(extrasBtn);
    menu->updateLayout();

     return section;
}
 
 CCNode* CustomSettingsPopup::createCustomColorsSection() {
      auto section = CCNode::create();
    // make this section like the others and add two buttons that open its popup
    auto contentSize = this->m_mainLayer->getContentSize();
    const float sectionH = 40.f;
    section->setContentSize({contentSize.width, sectionH});
    section->setAnchorPoint({0.5f, 0.5f});
    section->setID("customcolors-section"_spr);

    // Section title
    auto title = CCLabelBMFont::create("Custom Colors", "bigFont.fnt");
    title->setAnchorPoint({0.f, 0.5f});
    title->setPosition({10.0f, sectionH / 2.0f});
    title->setScale(0.6f);
    section->addChild(title);

    auto menu = CCMenu::create();
    menu->setContentSize(section->getContentSize());
    menu->setAnchorPoint({1.0f, 0.5f});
    menu->setPosition({contentSize.width - 10.0f, sectionH / 2.0f});
    menu->setScale(0.7f);
    menu->setID("customcolors-menu"_spr);
    menu->setLayout(
        RowLayout::create()
            ->setGap(20.f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setAxisReverse(false)
            ->setCrossAxisOverflow(true)
            ->setAutoGrowAxis(300.f)
    );
    section->addChild(menu);

    auto colorsSpr = ButtonSprite::create("Colors", "bigFont.fnt", "BlackButton.png"_spr, 0.7f);
    auto colorsBtn = CCMenuItemSpriteExtra::create(colorsSpr, this, menu_selector(CustomSettingsPopup::onOpenColors));
    colorsBtn->setScale(0.8f);
    colorsBtn->setID("customcolors-colors-btn"_spr);
    menu->addChild(colorsBtn);

    auto particlesSpr = ButtonSprite::create("Particles", "bigFont.fnt", "BlackButton.png"_spr, 0.7f);
    auto particlesBtn = CCMenuItemSpriteExtra::create(particlesSpr, this, menu_selector(CustomSettingsPopup::onOpenParticles));
    particlesBtn->setScale(0.8f);
    particlesBtn->setID("customcolors-particles-btn"_spr);
    menu->addChild(particlesBtn);
    menu->updateLayout();
      
      return section;
 }
 
 void CustomSettingsPopup::onOpenConditional(CCObject*) {
     ConditionalRandomizerPopup::create()->show();
 }
 
 void CustomSettingsPopup::onOpenLive(CCObject*) {
     LiveRandomizerPopup::create()->show();
 }
 void CustomSettingsPopup::onOpenModes(CCObject*) {
     ModesPopup::create()->show();
 }
 void CustomSettingsPopup::onOpenRGBMain(CCObject*) {
     RGBMainPopup::create()->show();
 }
 void CustomSettingsPopup::onOpenRGBExtras(CCObject*) {
     RGBExtrasPopup::create()->show();
 }
 void CustomSettingsPopup::onOpenColors(CCObject*) {
     CustomColorsPopup::create()->show();
 }
 void CustomSettingsPopup::onOpenParticles(CCObject*) {
     CustomColorsPopup::create()->show();
 }
 
 // [Rest of the helper methods remain the same]
 CCMenuItemToggler* CustomSettingsPopup::createToggler(std::string settingId, CCPoint position) {
     auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
     auto onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    
    offSprite->setScale(0.8f);
    onSprite->setScale(0.8f);
    
    auto toggler = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(CustomSettingsPopup::onRandomizerToggle));
    toggler->setPosition(position);

    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    toggler->setTag(tag);
    this->m_tagToSetting[tag] = settingId;

    bool initial = Mod::get()->getSettingValue<bool>(settingId);
    toggler->toggle(initial);

    toggler->setUserObject(CCString::create(settingId));
    
    return toggler;
}

Slider* CustomSettingsPopup::createSlider(std::string settingId, CCPoint position, float width) {
    auto slider = Slider::create(this, menu_selector(CustomSettingsPopup::onDelaySlider), 0.8f);
    auto m_thumb = slider->getThumb();
    slider->setPosition(position);
    slider->m_sliderBar->setContentSize({width, slider->m_sliderBar->getContentSize().height});
    slider->setUserObject(CCString::create(settingId));
    
    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    slider->setTag(tag);
    this->m_tagToSetting[tag] = settingId;

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

    int tag = toggler->getTag();
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) {
        settingIdStr = it->second;
    } else {
        auto obj = toggler->getUserObject();
        if (!obj) return;
        if (auto cs = dynamic_cast<CCString*>(obj)) {
            settingIdStr = std::string(cs->getCString());
        } else {
            return;
        }
    }
    
    bool newValue = !toggler->isToggled();
    Mod::get()->setSettingValue<bool>(settingIdStr, newValue);

    geode::log::debug("PersonalTweaks: toggled {} -> {}", settingIdStr, newValue ? "true" : "false");
}

void CustomSettingsPopup::onDelaySlider(CCObject* sender) {
    auto slider = dynamic_cast<Slider*>(sender);
    CCObject* userObj = nullptr;
    int tag = 0;
    float value = 0.0f;

    if (slider) {
        userObj = slider->getUserObject();
        tag = slider->getTag();
        value = slider->getValue();
    } else {
        auto thumb = dynamic_cast<SliderThumb*>(sender);
        if (thumb) {
            userObj = thumb->getUserObject();
            tag = thumb->getTag();
            value = thumb->getValue();
            if (!slider) {
                if (auto parentNode = dynamic_cast<CCNode*>(thumb->getParent())) {
                    slider = dynamic_cast<Slider*>(parentNode);
                }
            }
        } else {
            return;
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
    onRandomizerToggle(sender);
}

void CustomSettingsPopup::onRGBSpeedSlider(CCObject* sender) {
    onDelaySlider(sender);
}

void CustomSettingsPopup::onRGBSaturationSlider(CCObject* sender) {
    onDelaySlider(sender);
}

// ===== CONDITIONAL RANDOMIZER POPUP =====

ConditionalRandomizerPopup* ConditionalRandomizerPopup::create() {
    auto ret = new ConditionalRandomizerPopup();
    if (ret->initAnchored(360.0f, 240.0f, "SquareThing01.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ConditionalRandomizerPopup::setup() {
    this->setTitle("Conditional Randomizer");
    
    auto size = this->m_mainLayer->getContentSize();

    // Menu for interactive elements: give it the popup content size and position its origin
    auto menu = CCMenu::create();
    menu->setContentSize(size);
    // place menu so its bottom-left aligns relative to the popup center,
    // this makes child positions computed as (size.width/2 + offset, size.height/2 + offset)
    menu->setPosition({-size.width / 2.0f, -size.height / 2.0f});
    this->m_mainLayer->addChild(menu);
    
    // Node for labels / non-interactive elements
    auto labelNode = CCNode::create();
    labelNode->setContentSize(size);
    labelNode->setPosition({-size.width / 2.0f, -size.height / 2.0f});
    this->m_mainLayer->addChild(labelNode);
    
    // Main enable toggle (label) -- replicate previous offsets but based on container center
    auto enableLabel = CCLabelBMFont::create("Enable Conditional Randomizer", "bigFont.fnt");
    enableLabel->setPosition({size.width / 2.0f + 0.0f, size.height / 2.0f + 40.0f});
    enableLabel->setScale(0.6f);
    labelNode->addChild(enableLabel);
    
    auto enableToggler = createToggler("cond-ic-randomizer", {0.0f, 10.0f});
    // place toggler relative to container center
    enableToggler->setPosition({size.width / 2.0f + 0.0f, size.height / 2.0f + 10.0f});
    menu->addChild(enableToggler);
    
    // Mode toggles
    std::vector<std::pair<std::string, std::string>> modes = {
        {"cond-ongamemodechange", "Every Gamemode"},
        {"cond-ondeath", "On Death"}, 
        {"cond-onclick", "On Click"}
    };
    
    // Mode label
    auto modeLabel = CCLabelBMFont::create("Randomizer Modes:", "goldFont.fnt");
    modeLabel->setPosition({size.width / 2.0f + 0.0f, size.height / 2.0f - 20.0f});
    modeLabel->setScale(0.6f);
    labelNode->addChild(modeLabel);
    
    // Add each mode: labels in labelNode, toggles in menu (positions replicate prior layout)
    for (int i = 0; i < (int)modes.size(); i++) {
        float y = -50.0f - i * 28.0f;
        
        auto label = CCLabelBMFont::create(modes[i].second.c_str(), "bigFont.fnt");
        label->setPosition({size.width / 2.0f + 30.0f, size.height / 2.0f + y});
        label->setAnchorPoint({1.f, 0.5f});
        label->setScale(0.45f);
        labelNode->addChild(label);
        
        auto toggler = createToggler(modes[i].first, {60.0f, y});
        toggler->setPosition({size.width / 2.0f + 60.0f, size.height / 2.0f + y});
        toggler->setScale(0.8f);
        menu->addChild(toggler);
    }

    labelNode->setPosition({0, 20});
    menu->setPosition({0, 20});
    
    return true;
}

CCMenuItemToggler* ConditionalRandomizerPopup::createToggler(std::string settingId, CCPoint position) {
    auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    
    offSprite->setScale(0.8f);
    onSprite->setScale(0.8f);
    
    auto toggler = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(ConditionalRandomizerPopup::onToggle));
    toggler->setPosition(position);

    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    toggler->setTag(tag);
    this->m_tagToSetting[tag] = settingId;

    bool initial = Mod::get()->getSettingValue<bool>(settingId);
    toggler->toggle(initial);

    toggler->setUserObject(CCString::create(settingId));
    
    return toggler;
}

void ConditionalRandomizerPopup::onToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    if (!toggler) return;

    std::string settingIdStr;
    int tag = toggler->getTag();
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) {
        settingIdStr = it->second;
    } else {
        auto obj = toggler->getUserObject();
        if (!obj) return;
        if (auto cs = dynamic_cast<CCString*>(obj)) {
            settingIdStr = std::string(cs->getCString());
        } else {
            return;
        }
    }
    
    if (settingIdStr == "cond-ic-randomizer") {
        bool newValue = !toggler->isToggled();
        Mod::get()->setSettingValue<bool>(settingIdStr, newValue);
        if (newValue) {
            Mod::get()->setSettingValue<bool>("live-ic-randomizer", !newValue);
        }
    } else {
        bool newModeValue = !toggler->isToggled();
        Mod::get()->setSettingValue<bool>(settingIdStr, newModeValue);
    }

    geode::log::debug("ConditionalRandomizer: toggled {} -> {}", settingIdStr, !toggler->isToggled() ? "true" : "false");
}

// ===== LIVE RANDOMIZER POPUP =====

LiveRandomizerPopup* LiveRandomizerPopup::create() {
    auto ret = new LiveRandomizerPopup();
    if (ret->initAnchored(320.0f, 180.0f, "SquareThing01.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LiveRandomizerPopup::setup() {
    this->setTitle("Live Randomizer");
    
    auto size = this->m_mainLayer->getContentSize();

    // Menu for interactive elements: sized to popup and positioned so children can use center offsets
    auto menu = CCMenu::create();
    menu->setContentSize(size);
    menu->setPosition({-size.width / 2.0f, -size.height / 2.0f});
    this->m_mainLayer->addChild(menu);
    
    // Node for labels / non-interactive elements
    auto labelNode = CCNode::create();
    labelNode->setContentSize(size);
    labelNode->setPosition({-size.width / 2.0f, -size.height / 2.0f});
    this->m_mainLayer->addChild(labelNode);
    
    // Main enable toggle (label)
    auto enableLabel = CCLabelBMFont::create("Enable Live Randomizer", "bigFont.fnt");
    enableLabel->setPosition({size.width / 2.0f + 0.0f, size.height / 2.0f + 40.0f});
    enableLabel->setScale(0.6f);
    labelNode->addChild(enableLabel);
    
    auto enableToggler = createToggler("live-ic-randomizer", {0.0f, 10.0f});
    enableToggler->setPosition({size.width / 2.0f + 0.0f, size.height / 2.0f + 5.0f});
    menu->addChild(enableToggler);
    
    // Delay slider (label in labelNode, slider in menu)
    auto delayLabel = CCLabelBMFont::create("Randomize Delay:", "goldFont.fnt");
    delayLabel->setPosition({size.width / 2.0f + 0.0f, size.height / 2.0f - 30.0f});
    delayLabel->setScale(0.5f);
    labelNode->addChild(delayLabel);
    
    auto delaySlider = createSlider("random-delay", {0.0f, -50.0f}, 150.0f);
    delaySlider->setPosition({size.width / 2.0f + 0.0f, size.height / 2.0f - 50.0f});
    menu->addChild(delaySlider);
    
    // Value display label
    float currentDelay = Mod::get()->getSettingValue<double>("random-delay");
    std::string valueText = "Current: " + std::to_string(currentDelay).substr(0, 4);
    m_delayValueLabel = CCLabelBMFont::create(valueText.c_str(), "bigFont.fnt");
    m_delayValueLabel->setPosition({size.width / 2.0f + 0.0f, size.height / 2.0f - 60.0f});
    m_delayValueLabel->setScale(0.3f);
    m_delayValueLabel->setOpacity(150);
    labelNode->addChild(m_delayValueLabel);

    labelNode->setPosition({0, 0});
    menu->setPosition({0, 0});
    
    return true;
}

CCMenuItemToggler* LiveRandomizerPopup::createToggler(std::string settingId, CCPoint position) {
    auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    
    offSprite->setScale(0.8f);
    onSprite->setScale(0.8f);
    
    auto toggler = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(LiveRandomizerPopup::onToggle));
    toggler->setPosition(position);

    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    toggler->setTag(tag);
    this->m_tagToSetting[tag] = settingId;

    bool initial = Mod::get()->getSettingValue<bool>(settingId);
    toggler->toggle(initial);

    toggler->setUserObject(CCString::create(settingId));
    
    return toggler;
}

Slider* LiveRandomizerPopup::createSlider(std::string settingId, CCPoint position, float width) {
    auto slider = Slider::create(this, menu_selector(LiveRandomizerPopup::onDelaySlider), 0.8f);
    auto m_thumb = slider->getThumb();
    slider->setPosition(position);
    slider->m_sliderBar->setContentSize({width, slider->m_sliderBar->getContentSize().height});
    slider->setUserObject(CCString::create(settingId));
    
    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    slider->setTag(tag);
    this->m_tagToSetting[tag] = settingId;

    if (m_thumb) {
        m_thumb->setUserObject(CCString::create(settingId));
        m_thumb->setTag(tag);
    }
    
    // Set initial value for delay slider
    if (settingId == "random-delay") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue((value - 0.1f) / (70.0f - 0.1f));
    }
    
    return slider;
}

void LiveRandomizerPopup::onToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    if (!toggler) return;

    std::string settingIdStr;
    int tag = toggler->getTag();
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) {
        settingIdStr = it->second;
    } else {
        auto obj = toggler->getUserObject();
        if (!obj) return;
        if (auto cs = dynamic_cast<CCString*>(obj)) {
            settingIdStr = std::string(cs->getCString());
        } else {
            return;
        }
    }
    
    bool newValue = !toggler->isToggled();
    Mod::get()->setSettingValue<bool>(settingIdStr, newValue);
    if (newValue) {
        Mod::get()->setSettingValue<bool>("cond-ic-randomizer", !newValue);
    }

    geode::log::debug("LiveRandomizer: toggled {} -> {}", settingIdStr, newValue ? "true" : "false");
}

void LiveRandomizerPopup::onDelaySlider(CCObject* sender) {
    auto slider = dynamic_cast<Slider*>(sender);
    CCObject* userObj = nullptr;
    int tag = 0;
    float value = 0.0f;

    if (slider) {
        userObj = slider->getUserObject();
        tag = slider->getTag();
        value = slider->getValue();
    } else {
        auto thumb = dynamic_cast<SliderThumb*>(sender);
        if (thumb) {
            userObj = thumb->getUserObject();
            tag = thumb->getTag();
            value = thumb->getValue();
        } else return;
    }
    std::string settingIdStr;
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) settingIdStr = it->second;
    else if (userObj) {
        if (auto cs = dynamic_cast<CCString*>(userObj)) settingIdStr = std::string(cs->getCString());
        else return;
    } else return;

    if (settingIdStr == "random-delay") {
        float mapped = 0.1f + value * (70.0f - 0.1f);
        Mod::get()->setSettingValue<double>(settingIdStr, mapped);
        
        // Update the value display label
        if (m_delayValueLabel) {
            std::string valueText = "Current: " + std::to_string(mapped).substr(0, 4) + "s";
            m_delayValueLabel->setString(valueText.c_str());
        }
        
        geode::log::debug("LiveRandomizer: {} = {}", settingIdStr, mapped);
    }
}

// ===== MENU LAYER MODIFICATIONS =====

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

// ===== MODES POPUP =====
ModesPopup* ModesPopup::create() {
    auto ret = new ModesPopup();
    if (ret->initAnchored(420.0f, 260.0f, "SquareThing01.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ModesPopup::setup() {
    this->setTitle("Randomizer Modes");

    auto size = this->m_mainLayer->getContentSize();

    // Menu for togglers (interactive items)
    auto menu = CCMenu::create();
    menu->setContentSize(size);
    // position so child coordinates can be expressed as (size.width/2 + offset, size.height/2 + offset)
    menu->setPosition({-size.width / 2.0f, -size.height / 2.0f});
    this->m_mainLayer->addChild(menu);

    // Node for labels / non-interactive elements
    auto labelNode = CCNode::create();
    labelNode->setContentSize(size);
    labelNode->setPosition({-size.width / 2.0f, -size.height / 2.0f});
    this->m_mainLayer->addChild(labelNode);

    // Icon type toggles (arranged in rows)
    std::vector<std::pair<std::string, std::string>> iconTypes = {
        {"randomize-cube", "Cube"}, {"randomize-ship", "Ship"},
        {"randomize-ball", "Ball"}, {"randomize-ufo", "UFO"},
        {"randomize-wave", "Wave"}, {"randomize-robot", "Robot"},
        {"randomize-spider", "Spider"}, {"randomize-swing", "Swing"}
    };

    // extra vertical offset added for each group of 4 items (tweak this value)
    const float extraGapPerGroup = 18.0f;

    for (int i = 0; i < (int)iconTypes.size(); i++) {
        int row = i / 4;
        int col = i % 4;
        float x = -150.0f + col * 100.0f;
        float y = 40.0f - row * 35.0f;
        // add extra downward offset for groups after the first (every 4 items)
        float extraYOffset = (i / 4) * extraGapPerGroup;
        y -= extraYOffset;

        auto label = CCLabelBMFont::create(iconTypes[i].second.c_str(), "bigFont.fnt");
        label->setPosition({size.width / 2.0f + x, size.height / 2.0f + y + 15.0f});
        label->setScale(0.45f);
        labelNode->addChild(label);

        auto toggler = createToggler(iconTypes[i].first, {x, y - 5.0f});
        // place toggler relative to container center
        toggler->setPosition({size.width / 2.0f + x, size.height / 2.0f + y - 5.0f});
        menu->addChild(toggler);
    }

    // Color toggles
    std::vector<std::pair<std::string, std::string>> colorTypes = {
        {"randomize-color1", "Color 1"}, {"randomize-color2", "Color 2"}, {"randomize-glowcolor", "Glow"}
    };

    for (int i = 0; i < (int)colorTypes.size(); i++) {
        float x = -100.0f + i * 100.0f;
        float y = -60.0f;

        auto label = CCLabelBMFont::create(colorTypes[i].second.c_str(), "bigFont.fnt");
        label->setPosition({size.width / 2.0f + x, size.height / 2.0f + y + 15.0f});
        label->setScale(0.45f);
        labelNode->addChild(label);

        auto toggler = createToggler(colorTypes[i].first, {x, y - 5.0f});
        toggler->setPosition({size.width / 2.0f + x, size.height / 2.0f + y - 5.0f});
        menu->addChild(toggler);
    }

    // small nudge if you want space from top
    labelNode->setPosition({0, 20});
    menu->setPosition({0, 20});

    return true;
}

CCMenuItemToggler* ModesPopup::createToggler(std::string settingId, CCPoint position) {
    auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    offSprite->setScale(0.8f);
    onSprite->setScale(0.8f);
    auto toggler = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(ModesPopup::onToggle));
    toggler->setPosition(position);
    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    toggler->setTag(tag);
    this->m_tagToSetting[tag] = settingId;
    bool initial = Mod::get()->getSettingValue<bool>(settingId);
    toggler->toggle(initial);
    toggler->setUserObject(CCString::create(settingId));
    return toggler;
}

void ModesPopup::onToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    if (!toggler) return;
    std::string settingIdStr;
    int tag = toggler->getTag();
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) {
        settingIdStr = it->second;
    } else {
        auto obj = toggler->getUserObject();
        if (!obj) return;
        if (auto cs = dynamic_cast<CCString*>(obj)) settingIdStr = std::string(cs->getCString());
        else return;
    }
    bool newValue = !toggler->isToggled();
    Mod::get()->setSettingValue<bool>(settingIdStr, newValue);
    geode::log::debug("ModesPopup: toggled {} -> {}", settingIdStr, newValue ? "true" : "false");
}

// ===== RGB MAIN / EXTRAS POPUPS =====
RGBMainPopup* RGBMainPopup::create() {
    auto ret = new RGBMainPopup();
    // wider popup to fit two columns + sliders
    if (ret->initAnchored(480.0f, 300.0f, "SquareThing01.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

CCMenuItemToggler* RGBMainPopup::createToggler(std::string settingId, CCPoint position) {
    auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto onSprite  = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    offSprite->setScale(0.8f);
    onSprite->setScale(0.8f);

    auto toggler = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(RGBMainPopup::onToggle));
    toggler->setPosition(position);

    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    toggler->setTag(tag);
    this->m_tagToSetting[tag] = settingId;

    bool initial = Mod::get()->getSettingValue<bool>(settingId);
    toggler->toggle(initial);
    toggler->setUserObject(CCString::create(settingId));
    return toggler;
}

Slider* RGBMainPopup::createSlider(std::string settingId, CCPoint position, float width) {
    auto slider = Slider::create(this, menu_selector(RGBMainPopup::onSlider), 0.8f);
    auto thumb  = slider->getThumb();
    slider->setPosition(position);
    slider->m_sliderBar->setContentSize({width, slider->m_sliderBar->getContentSize().height});
    slider->setUserObject(CCString::create(settingId));

    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    slider->setTag(tag);
    this->m_tagToSetting[tag] = settingId;

    if (thumb) {
        thumb->setUserObject(CCString::create(settingId));
        thumb->setTag(tag);
    }

    // initial values mapping
    if (settingId == "rgb-speed") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue(value / 10.0f);
    } else if (settingId == "rgb-saturation" || settingId == "rgb-saturation2") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue(value);
    } else if (settingId == "rgb-brightness1" || settingId == "rgb-brightness2") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue(value);
    } else if (settingId == "random-delay") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue((value - 0.1f) / (70.0f - 0.1f));
    }

    return slider;
}

bool RGBMainPopup::setup() {
    this->setTitle("RGB Icons - Main");
    auto size = this->m_mainLayer->getContentSize();

    auto menu = CCMenu::create();
    menu->setContentSize(size);
    this->m_mainLayer->addChild(menu);

    auto labelNode = CCNode::create();
    labelNode->setContentSize(size);
    this->m_mainLayer->addChild(labelNode);

    // layout coordinates (relative offsets from center)
    // increased vertical spacing and name labels moved up
    const float leftX = -140.0f;
    const float rightX = 140.0f;
    const float topY = 72.0f;      // toggler row
    const float satY = 24.0f;      // saturation slider row
    const float brightY = -18.0f;  // brightness slider row
    const float speedY = -84.0f;   // rgb speed center row
    const float nameLabelNudge = 18.0f; // move name labels upward
    const float elementVertGap = 18.0f; // additional spacing between elements (tweak as needed)

    // Color 1 column
    auto col1Title = CCLabelBMFont::create("Color 1", "bigFont.fnt");
    col1Title->setPosition({leftX, topY + nameLabelNudge});
    col1Title->setScale(0.5f);
    labelNode->addChild(col1Title);

    auto col1Toggler = createToggler("rgb-col1", {leftX, topY - 15.0f});
    col1Toggler->setPosition({leftX, topY - 15.0f});
    menu->addChild(col1Toggler);

    // Saturation 1
    auto sat1Label = CCLabelBMFont::create("Saturation", "bigFont.fnt");
    sat1Label->setPosition({leftX, satY + nameLabelNudge});
    sat1Label->setScale(0.45f);
    labelNode->addChild(sat1Label);

    auto sat1Slider = createSlider("rgb-saturation", {leftX, satY}, 140.0f);
    sat1Slider->setPosition({leftX, satY});
    menu->addChild(sat1Slider);
    // value label under slider
    int sat1Tag = sat1Slider->getTag();
    {
        auto vl = CCLabelBMFont::create(std::to_string(Mod::get()->getSettingValue<double>("rgb-saturation")).substr(0,4).c_str(), "bigFont.fnt");
        vl->setPosition({leftX, satY - 18.0f});
        vl->setScale(0.4f);
        vl->setOpacity(150);
        labelNode->addChild(vl);
        this->m_valueLabels[sat1Tag] = vl;
    }

    // Brightness 1
    auto bright1Label = CCLabelBMFont::create("Brightness", "bigFont.fnt");
    bright1Label->setPosition({leftX, brightY + nameLabelNudge});
    bright1Label->setScale(0.45f);
    labelNode->addChild(bright1Label);

    auto bright1Slider = createSlider("rgb-brightness1", {leftX, brightY}, 140.0f);
    bright1Slider->setPosition({leftX, brightY});
    menu->addChild(bright1Slider);
    int bright1Tag = bright1Slider->getTag();
    {
        auto vl = CCLabelBMFont::create(std::to_string(Mod::get()->getSettingValue<double>("rgb-brightness1")).substr(0,4).c_str(), "bigFont.fnt");
        vl->setPosition({leftX, brightY - 18.0f});
        vl->setScale(0.4f);
        vl->setOpacity(150);
        labelNode->addChild(vl);
        this->m_valueLabels[bright1Tag] = vl;
    }

    // Color 2 column
    auto col2Title = CCLabelBMFont::create("Color 2", "bigFont.fnt");
    col2Title->setPosition({rightX, topY + nameLabelNudge});
    col2Title->setScale(0.5f);
    labelNode->addChild(col2Title);

    auto col2Toggler = createToggler("rgb-col2", {rightX, topY - 15.0f});
    col2Toggler->setPosition({rightX, topY - 15.0f});
    menu->addChild(col2Toggler);

    // Saturation 2
    auto sat2Label = CCLabelBMFont::create("Saturation", "bigFont.fnt");
    sat2Label->setPosition({rightX, satY + nameLabelNudge});
    sat2Label->setScale(0.45f);
    labelNode->addChild(sat2Label);

    auto sat2Slider = createSlider("rgb-saturation2", {rightX, satY}, 140.0f);
    sat2Slider->setPosition({rightX, satY});
    menu->addChild(sat2Slider);
    int sat2Tag = sat2Slider->getTag();
    {
        auto vl = CCLabelBMFont::create(std::to_string(Mod::get()->getSettingValue<double>("rgb-saturation2")).substr(0,4).c_str(), "bigFont.fnt");
        vl->setPosition({rightX, satY - 18.0f});
        vl->setScale(0.4f);
        vl->setOpacity(150);
        labelNode->addChild(vl);
        this->m_valueLabels[sat2Tag] = vl;
    }

    // Brightness 2
    auto bright2Label = CCLabelBMFont::create("Brightness", "bigFont.fnt");
    bright2Label->setPosition({rightX, brightY + nameLabelNudge});
    bright2Label->setScale(0.45f);
    labelNode->addChild(bright2Label);

    auto bright2Slider = createSlider("rgb-brightness2", {rightX, brightY}, 140.0f);
    bright2Slider->setPosition({rightX, brightY});
    menu->addChild(bright2Slider);
    int bright2Tag = bright2Slider->getTag();
    {
        auto vl = CCLabelBMFont::create(std::to_string(Mod::get()->getSettingValue<double>("rgb-brightness2")).substr(0,4).c_str(), "bigFont.fnt");
        vl->setPosition({rightX, brightY - 18.0f});
        vl->setScale(0.4f);
        vl->setOpacity(150);
        labelNode->addChild(vl);
        this->m_valueLabels[bright2Tag] = vl;
    }

    // RGB speed centered
    auto speedLabel = CCLabelBMFont::create("RGB Speed", "bigFont.fnt");
    speedLabel->setPosition({0.0f, speedY + nameLabelNudge});
    speedLabel->setScale(0.45f);
    labelNode->addChild(speedLabel);

    auto speedSlider = createSlider("rgb-speed", {0.0f, speedY}, 240.0f);
    speedSlider->setPosition({0.0f, speedY});
    menu->addChild(speedSlider);
    int speedTag = speedSlider->getTag();
    {
        float initSpeed = Mod::get()->getSettingValue<double>("rgb-speed");
        auto vl = CCLabelBMFont::create(std::to_string(initSpeed).substr(0,4).c_str(), "bigFont.fnt");
        vl->setPosition({0.0f, speedY - 18.0f});
        vl->setScale(0.4f);
        vl->setOpacity(150);
        labelNode->addChild(vl);
        this->m_valueLabels[speedTag] = vl;
    }

    menu->setPosition({0, 0});
    labelNode->setPosition({0, 0});

    // done
    return true;
}

// update handler: update value labels when sliders change
void RGBMainPopup::onSlider(CCObject* sender) {
    auto slider = dynamic_cast<Slider*>(sender);
    CCObject* userObj = nullptr;
    int tag = 0;
    float value = 0.0f;

    if (slider) {
        userObj = slider->getUserObject();
        tag = slider->getTag();
        value = slider->getValue();
    } else {
        auto thumb = dynamic_cast<SliderThumb*>(sender);
        if (thumb) {
            userObj = thumb->getUserObject();
            tag = thumb->getTag();
            value = thumb->getValue();
        } else return;
    }

    std::string settingIdStr;
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) settingIdStr = it->second;
    else if (userObj) {
        if (auto cs = dynamic_cast<CCString*>(userObj)) settingIdStr = std::string(cs->getCString());
        else return;
    } else return;

    // map slider value to actual setting and update label
    if (settingIdStr == "rgb-speed") {
        float mapped = value * 10.0f;
        Mod::get()->setSettingValue<double>(settingIdStr, mapped);
        if (this->m_valueLabels.count(tag)) {
            std::string t = std::to_string(mapped).substr(0,4);
            this->m_valueLabels[tag]->setString(t.c_str());
        }
    } else if (settingIdStr == "rgb-saturation" || settingIdStr == "rgb-saturation2") {
        Mod::get()->setSettingValue<double>(settingIdStr, value);
        if (this->m_valueLabels.count(tag)) {
            std::string t = std::to_string(value).substr(0,4);
            this->m_valueLabels[tag]->setString(t.c_str());
        }
    } else if (settingIdStr == "rgb-brightness1" || settingIdStr == "rgb-brightness2") {
        Mod::get()->setSettingValue<double>(settingIdStr, value);
        if (this->m_valueLabels.count(tag)) {
            std::string t = std::to_string(value).substr(0,4);
            this->m_valueLabels[tag]->setString(t.c_str());
        }
    }
}

void RGBMainPopup::onToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    if (!toggler) return;

    int tag = toggler->getTag();
    std::string id;
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) {
        id = it->second;
    } else {
        auto obj = toggler->getUserObject();
        if (!obj) return;
        if (auto cs = dynamic_cast<CCString*>(obj)) id = cs->getCString();
        else return;
    }

    bool newValue = !toggler->isToggled();
    Mod::get()->setSettingValue<bool>(id, newValue);
    geode::log::debug("RGBMainPopup: toggled {} -> {}", id, newValue ? "true" : "false");
}

// ===== RGB EXTRAS POPUP =====
RGBExtrasPopup* RGBExtrasPopup::create() {
    auto ret = new RGBExtrasPopup();
    if (ret->initAnchored(380.0f, 220.0f, "SquareThing01.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool RGBExtrasPopup::setup() {
    this->setTitle("RGB Icons - Extras");
    auto size = this->m_mainLayer->getContentSize();

    auto menu = CCMenu::create();
    menu->setContentSize(size);
    this->m_mainLayer->addChild(menu);

    auto labelNode = CCNode::create();
    labelNode->setContentSize(size);
    this->m_mainLayer->addChild(labelNode);

    // toggles - layout with more vertical spacing and smaller togglers
    std::vector<std::pair<std::string, std::string>> extras = {
        {"rgb-wave", "RGB Wave"},
        {"rgb-trail", "RGB Trail"},
        {"rgb-dash", "RGB Dash"},
        {"better-immersion-mode", "RGB Immersion Mode"},
        {"ignore-p2", "Ignore Player 2"}
    };

    const float leftX = -120.0f;
    const float rightX = 120.0f;
    const float startY = 72.0f;
    const float rowGap = 36.0f; // increased vertical spacing
    for (size_t i = 0; i < extras.size(); ++i) {
        float y = startY - (float)i * rowGap;
        auto label = CCLabelBMFont::create(extras[i].second.c_str(), "bigFont.fnt");
        label->setPosition({leftX, y});
        label->setScale(0.45f);
        labelNode->addChild(label);

        auto offS = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto onS  = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        offS->setScale(0.7f);
        onS->setScale(0.7f);
        auto toggler = CCMenuItemToggler::create(offS, onS, this, menu_selector(RGBExtrasPopup::onToggle));
        toggler->setPosition({rightX, y});
        toggler->setScale(0.95f);
        int tag = static_cast<int>(std::hash<std::string>{}(extras[i].first));
        toggler->setTag(tag);
        this->m_tagToSetting[tag] = extras[i].first;
        bool initial = Mod::get()->getSettingValue<bool>(extras[i].first);
        toggler->toggle(initial);
        toggler->setUserObject(CCString::create(extras[i].first));
        menu->addChild(toggler);
    }

    // p2-distance slider (placed under the list with spacing)
    const float sliderY = startY - extras.size() * rowGap - 12.0f;
    auto pdLabel = CCLabelBMFont::create("Player 2 Distance", "bigFont.fnt");
    pdLabel->setPosition({-20.0f, sliderY + 12.0f});
    pdLabel->setScale(0.45f);
    labelNode->addChild(pdLabel);

    auto pdSlider = Slider::create(this, menu_selector(RGBExtrasPopup::onSlider), 0.8f);
    pdSlider->setPosition({80.0f, sliderY});
    pdSlider->m_sliderBar->setContentSize({200.0f, pdSlider->m_sliderBar->getContentSize().height});
    int pdTag = static_cast<int>(std::hash<std::string>{}("p2-distance"));
    pdSlider->setTag(pdTag);
    pdSlider->setUserObject(CCString::create("p2-distance"));
    float pdValue = Mod::get()->getSettingValue<double>("p2-distance");
    pdSlider->setValue(pdValue);
    this->m_tagToSetting[pdTag] = "p2-distance";
    menu->addChild(pdSlider);

    auto pdValueLabel = CCLabelBMFont::create(std::to_string(pdValue).substr(0,4).c_str(), "bigFont.fnt");
    pdValueLabel->setPosition({80.0f, sliderY - 18.0f});
    pdValueLabel->setScale(0.4f);
    pdValueLabel->setOpacity(150);
    labelNode->addChild(pdValueLabel);
    this->m_valueLabels[pdTag] = pdValueLabel;

    menu->setPosition({0, 0});
    labelNode->setPosition({0, 0});

    return true;
}

void RGBExtrasPopup::onToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    if (!toggler) return;
    int tag = toggler->getTag();
    std::string id;
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) id = it->second;
    else {
        auto obj = toggler->getUserObject();
        if (!obj) return;
        if (auto cs = dynamic_cast<CCString*>(obj)) id = cs->getCString();
        else return;
    }
    bool newValue = !toggler->isToggled();
    Mod::get()->setSettingValue<bool>(id, newValue);
}

void RGBExtrasPopup::onSlider(CCObject* sender) {
    auto slider = dynamic_cast<Slider*>(sender);
    if (!slider) return;
    int tag = slider->getTag();
    float value = slider->getValue();
    std::string id;
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) id = it->second;
    else {
        auto obj = slider->getUserObject();
        if (!obj) return;
        if (auto cs = dynamic_cast<CCString*>(obj)) id = cs->getCString();
        else return;
    }

    if (id == "p2-distance") {
        Mod::get()->setSettingValue<double>(id, value);
        if (this->m_valueLabels.count(tag)) {
            this->m_valueLabels[tag]->setString(std::to_string(value).substr(0,4).c_str());
        }
    }
}

// ===== CUSTOM COLORS POPUP =====
CustomColorsPopup* CustomColorsPopup::create() {
    auto ret = new CustomColorsPopup();
    if (ret->initAnchored(360.0f, 220.0f, "SquareThing01.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool CustomColorsPopup::setup() {
    this->setTitle("Custom Colors");
    // empty for now
    
    return true;
}