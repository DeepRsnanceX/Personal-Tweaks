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
    const float topY = size.height * 0.65f;
    const float midY = size.height * 0.35f;
    const float centerX = size.width / 2.0f;

    // Randomizer section (left title + three buttons to the right)
    auto randomizerSection = createRandomizerSection();
    randomizerSection->setPosition({centerX, topY});
    this->m_mainLayer->addChild(randomizerSection);

    // RGB Icons section (left title + two buttons to the right)
    auto rgbSection = createRGBSection();
    rgbSection->setPosition({centerX, midY});
    this->m_mainLayer->addChild(rgbSection);

    // Custom Colors section stays below RGB (if needed)
    auto customColorsSection = createCustomColorsSection();
    customColorsSection->setPosition({centerX, midY - 70.0f});
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
    menu->setPosition({contentSize.width - 10.0f, 0.0f});
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
    title->setScale(0.5f);
    section->addChild(title);

    auto menu = CCMenu::create();
    menu->setContentSize(section->getContentSize());
    menu->setAnchorPoint({1.0f, 0.5f});
    menu->setPosition({contentSize.width - 10.0f, 0.0f});
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
    auto menu = CCMenu::create();
    menu->setPosition(CCPointZero);
    this->m_mainLayer->addChild(menu);

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
        float y = 40.0f - row * 35.0f;

        auto label = CCLabelBMFont::create(iconTypes[i].second.c_str(), "chatFont.fnt");
        label->setPosition({x, y + 15.0f});
        label->setScale(0.5f);
        this->m_mainLayer->addChild(label);

        auto toggler = createToggler(iconTypes[i].first, {x, y - 5.0f});
        menu->addChild(toggler);
    }

    // Color toggles
    std::vector<std::pair<std::string, std::string>> colorTypes = {
        {"randomize-color1", "Color 1"}, {"randomize-color2", "Color 2"}, {"randomize-glowcolor", "Glow"}
    };

    for (int i = 0; i < (int)colorTypes.size(); i++) {
        float x = -100.0f + i * 100.0f;
        float y = -60.0f;

        auto label = CCLabelBMFont::create(colorTypes[i].second.c_str(), "chatFont.fnt");
        label->setPosition({x, y + 15.0f});
        label->setScale(0.5f);
        this->m_mainLayer->addChild(label);

        auto toggler = createToggler(colorTypes[i].first, {x, y - 5.0f});
        menu->addChild(toggler);
    }

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
    if (ret->initAnchored(420.0f, 260.0f, "SquareThing01.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool RGBMainPopup::setup() {
    this->setTitle("RGB Icons - Main");
    auto size = this->m_mainLayer->getContentSize();
    auto menu = CCMenu::create();
    menu->setPosition(CCPointZero);
    this->m_mainLayer->addChild(menu);

    // Color toggles
    auto col1Label = CCLabelBMFont::create("Color 1 RGB", "chatFont.fnt");
    col1Label->setPosition({-120.0f, 80.0f});
    col1Label->setScale(0.5f);
    this->m_mainLayer->addChild(col1Label);
    auto col1Toggler = createToggler("rgb-col1", {-20.0f, 80.0f});
    menu->addChild(col1Toggler);

    auto col2Label = CCLabelBMFont::create("Color 2 RGB", "chatFont.fnt");
    col2Label->setPosition({80.0f, 80.0f});
    col2Label->setScale(0.5f);
    this->m_mainLayer->addChild(col2Label);
    auto col2Toggler = createToggler("rgb-col2", {180.0f, 80.0f});
    menu->addChild(col2Toggler);

    // RGB Speed slider
    auto speedLabel = CCLabelBMFont::create("Speed", "chatFont.fnt");
    speedLabel->setPosition({-180.0f, 50.0f});
    speedLabel->setScale(0.5f);
    this->m_mainLayer->addChild(speedLabel);
    auto speedSlider = createSlider("rgb-speed", {0.0f, 50.0f});
    menu->addChild(speedSlider);

    // Saturation sliders
    auto sat1Label = CCLabelBMFont::create("Sat 1", "chatFont.fnt");
    sat1Label->setPosition({-150.0f, 20.0f});
    sat1Label->setScale(0.45f);
    this->m_mainLayer->addChild(sat1Label);
    auto sat1Slider = createSlider("rgb-saturation", {-50.0f, 20.0f}, 120.0f);
    menu->addChild(sat1Slider);

    auto sat2Label = CCLabelBMFont::create("Sat 2", "chatFont.fnt");
    sat2Label->setPosition({50.0f, 20.0f});
    sat2Label->setScale(0.45f);
    this->m_mainLayer->addChild(sat2Label);
    auto sat2Slider = createSlider("rgb-saturation2", {130.0f, 20.0f}, 120.0f);
    menu->addChild(sat2Slider);

    // Ignore P2 toggle
    auto ignoreLabel = CCLabelBMFont::create("Ignore P2", "chatFont.fnt");
    ignoreLabel->setPosition({-60.0f, -10.0f});
    ignoreLabel->setScale(0.5f);
    this->m_mainLayer->addChild(ignoreLabel);
    auto ignoreToggler = createToggler("ignore-p2", {40.0f, -10.0f});
    menu->addChild(ignoreToggler);

    return true;
}

CCMenuItemToggler* RGBMainPopup::createToggler(std::string settingId, CCPoint position) {
    auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
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
    if (settingId == "rgb-speed") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue(value / 10.0f);
    } else if (settingId == "rgb-saturation" || settingId == "rgb-saturation2") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue(value);
    }
    return slider;
}

void RGBMainPopup::onToggle(CCObject* sender) {
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
    geode::log::debug("RGBMain: toggled {} -> {}", settingIdStr, newValue ? "true" : "false");
}

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

    if (settingIdStr == "rgb-speed") {
        float mapped = value * 10.0f;
        Mod::get()->setSettingValue<double>(settingIdStr, mapped);
    } else if (settingIdStr == "rgb-saturation" || settingIdStr == "rgb-saturation2") {
        Mod::get()->setSettingValue<double>(settingIdStr, value);
    }
}

RGBExtrasPopup* RGBExtrasPopup::create() {
    auto ret = new RGBExtrasPopup();
    if (ret->initAnchored(320.0f, 180.0f, "SquareThing01.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool RGBExtrasPopup::setup() {
    this->setTitle("RGB Icons - Extras");
    // Stub - fill later
    return true;
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