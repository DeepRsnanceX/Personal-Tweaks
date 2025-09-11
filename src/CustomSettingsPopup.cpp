#include "CustomSettingsPopup.hpp"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>

using namespace geode::prelude;

// ===== MAIN POPUP =====

CustomSettingsPopup* CustomSettingsPopup::create() {
    auto ret = new CustomSettingsPopup();
    if (ret->initAnchored(420.0f, 280.0f, "SquareThing01.png"_spr)) {
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
    
    auto size = contentSize;
    const float topY = size.height;
    const float centerX = size.width / 2.0f;

    // Randomizer section
    auto randomizerSection = createRandomizerSection();
    randomizerSection->setPosition({centerX, topY - 60.f});
    this->m_mainLayer->addChild(randomizerSection);

    // RGB Icons section
    auto rgbSection = createRGBSection();
    rgbSection->setPosition({centerX, topY - 95.f});
    this->m_mainLayer->addChild(rgbSection);

    // Custom Colors section
    auto customColorsSection = createCustomColorsSection();
    customColorsSection->setPosition({centerX, topY - 130.0f});
    this->m_mainLayer->addChild(customColorsSection);
     
    return true;
}

CCNode* CustomSettingsPopup::createRandomizerSection() {
    auto section = CCNode::create();
    auto contentSize = this->m_mainLayer->getContentSize();
    const float sectionH = 40.f;
    section->setContentSize({contentSize.width, sectionH});
    section->setAnchorPoint({0.5f, 0.5f});
    section->setID("randomizer-section"_spr);

    // Left-aligned
    auto title = CCLabelBMFont::create("Randomizer", "bigFont.fnt");
    title->setAnchorPoint({0.f, 0.5f});
    title->setPosition({15.0f, sectionH / 2.0f});
    title->setScale(0.7f);
    section->addChild(title);

    // menu holds sprite buttons
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

    auto condSpr = ButtonSprite::create("Cond.", "bigFont.fnt", "transparentButtonThing.png"_spr, 0.7f);
    auto conditionalBtn = CCMenuItemSpriteExtra::create(condSpr, this, menu_selector(CustomSettingsPopup::onOpenConditional));
    conditionalBtn->setScale(0.8f);
    conditionalBtn->setID("randomizer-cond-btn"_spr);
    menu->addChild(conditionalBtn);

    auto liveSpr = ButtonSprite::create("Live", "bigFont.fnt", "transparentButtonThing.png"_spr, 0.7f);
    auto liveBtn = CCMenuItemSpriteExtra::create(liveSpr, this, menu_selector(CustomSettingsPopup::onOpenLive));
    liveBtn->setScale(0.8f);
    liveBtn->setID("randomizer-live-btn"_spr);
    menu->addChild(liveBtn);

    auto modesSpr = ButtonSprite::create("Modes", "bigFont.fnt", "transparentButtonThing.png"_spr, 0.7f);
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
    title->setPosition({15.0f, sectionH / 2.0f});
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

    auto mainSpr = ButtonSprite::create("Main", "bigFont.fnt", "transparentButtonThing.png"_spr, 0.7f);
    auto mainBtn = CCMenuItemSpriteExtra::create(mainSpr, this, menu_selector(CustomSettingsPopup::onOpenRGBMain));
    mainBtn->setScale(0.8f);
    mainBtn->setID("rgb-main-btn"_spr);
    menu->addChild(mainBtn);

    auto extrasSpr = ButtonSprite::create("Extras", "bigFont.fnt", "transparentButtonThing.png"_spr, 0.7f);
    auto extrasBtn = CCMenuItemSpriteExtra::create(extrasSpr, this, menu_selector(CustomSettingsPopup::onOpenRGBExtras));
    extrasBtn->setScale(0.8f);
    extrasBtn->setID("rgb-extras-btn"_spr);
    menu->addChild(extrasBtn);
    menu->updateLayout();

    return section;
}
 
CCNode* CustomSettingsPopup::createCustomColorsSection() {
    auto section = CCNode::create();
    auto contentSize = this->m_mainLayer->getContentSize();
    const float sectionH = 40.f;
    section->setContentSize({contentSize.width, sectionH});
    section->setAnchorPoint({0.5f, 0.5f});
    section->setID("customcolors-section"_spr);

    // Section title
    auto title = CCLabelBMFont::create("Custom Colors", "bigFont.fnt");
    title->setAnchorPoint({0.f, 0.5f});
    title->setPosition({15.0f, sectionH / 2.0f});
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

    auto colorsSpr = ButtonSprite::create("Colors", "bigFont.fnt", "transparentButtonThing.png"_spr, 0.7f);
    auto colorsBtn = CCMenuItemSpriteExtra::create(colorsSpr, this, menu_selector(CustomSettingsPopup::onOpenColors));
    colorsBtn->setScale(0.8f);
    colorsBtn->setID("customcolors-colors-btn"_spr);
    menu->addChild(colorsBtn);

    auto particlesSpr = ButtonSprite::create("Particles", "bigFont.fnt", "transparentButtonThing.png"_spr, 0.7f);
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
    ParticlesPopup::create()->show();
}
 
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
    
    // Set initial value
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

    // Menu for interactive elements
    auto menu = CCMenu::create();
    menu->setContentSize(size);
    menu->setPosition({-size.width / 2.0f, -size.height / 2.0f});
    this->m_mainLayer->addChild(menu);
    
    // Node for labels
    auto labelNode = CCNode::create();
    labelNode->setContentSize(size);
    labelNode->setPosition({-size.width / 2.0f, -size.height / 2.0f});
    this->m_mainLayer->addChild(labelNode);
    
    // Main enable toggle (label)
    auto enableLabel = CCLabelBMFont::create("Enable Conditional Randomizer", "bigFont.fnt");
    enableLabel->setPosition({size.width / 2.0f + 0.0f, size.height / 2.0f + 40.0f});
    enableLabel->setScale(0.6f);
    labelNode->addChild(enableLabel);
    
    auto enableToggler = createToggler("cond-ic-randomizer", {0.0f, 10.0f});
    enableToggler->setPosition({size.width / 2.0f + 0.0f, size.height / 2.0f + 10.0f});
    menu->addChild(enableToggler);
    
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

    // Menu for interactive elements
    auto menu = CCMenu::create();
    menu->setContentSize(size);
    menu->setPosition({-size.width / 2.0f, -size.height / 2.0f});
    this->m_mainLayer->addChild(menu);
    
    // Node for labels
    auto labelNode = CCNode::create();
    labelNode->setContentSize(size);
    labelNode->setPosition({-size.width / 2.0f, -size.height / 2.0f});
    this->m_mainLayer->addChild(labelNode);
    
    // Main enable toggle label
    auto enableLabel = CCLabelBMFont::create("Enable Live Randomizer", "bigFont.fnt");
    enableLabel->setPosition({size.width / 2.0f + 0.0f, size.height / 2.0f + 40.0f});
    enableLabel->setScale(0.6f);
    labelNode->addChild(enableLabel);
    
    auto enableToggler = createToggler("live-ic-randomizer", {0.0f, 10.0f});
    enableToggler->setPosition({size.width / 2.0f + 0.0f, size.height / 2.0f + 5.0f});
    menu->addChild(enableToggler);
    
    // Delay slider
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

// ----------------------
//
// ===== MENU HOOKS =====
// 
// ----------------------

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
    if (ret->initAnchored(300.0f, 250.0f, "SquareThing01.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ModesPopup::setup() {
    this->setTitle("Randomizer Modes");

    auto size = this->m_mainLayer->getContentSize();

    // Menu for togglers
    auto menu = CCMenu::create();
    menu->setContentSize(size);
    menu->setPosition({-size.width / 2.0f, -size.height / 2.0f});
    this->m_mainLayer->addChild(menu);

    // Node for labels
    auto labelNode = CCNode::create();
    labelNode->setContentSize(size);
    labelNode->setPosition({-size.width / 2.0f, -size.height / 2.0f});
    this->m_mainLayer->addChild(labelNode);

    // Icon type toggles
    std::vector<std::pair<std::string, std::string>> iconTypes = {
        {"randomize-cube", "Cube"}, {"randomize-ship", "Ship"},
        {"randomize-ball", "Ball"}, {"randomize-ufo", "UFO"},
        {"randomize-wave", "Wave"}, {"randomize-robot", "Robot"},
        {"randomize-spider", "Spider"}, {"randomize-swing", "Swing"}
    };

    const int cols = 4;
    const float originalGap = 75.0f;
    const float horizGap = originalGap - 15.0f; // extra spacing
    const float startX = -((cols - 1) * horizGap) / 2.0f;
    const float extraGapPerGroup = 24.0f;

    for (int i = 0; i < (int)iconTypes.size(); i++) {
        int row = i / cols;
        int col = i % cols;
        float x = startX + col * horizGap;
        float y = 40.0f - row * 35.0f;
        // add extra downward offset for groups after the first (every 4 items)
        float extraYOffset = (i / cols) * extraGapPerGroup;
        y -= extraYOffset;

        auto label = CCLabelBMFont::create(iconTypes[i].second.c_str(), "bigFont.fnt");
        label->setPosition({size.width / 2.0f + x, size.height / 2.0f + y + 20.0f});
        label->setScale(0.45f);
        labelNode->addChild(label);

        auto toggler = createToggler(iconTypes[i].first, {x, y - 5.0f});
        toggler->setPosition({size.width / 2.0f + x, size.height / 2.0f + y - 5.0f});
        menu->addChild(toggler);
    }

    std::vector<std::pair<std::string, std::string>> colorTypes = {
        {"randomize-color1", "Color 1"}, {"randomize-color2", "Color 2"}, {"randomize-glowcolor", "Glow"}
    };

    const int colorCols = 3;
    const float colorOriginalGap = 100.0f;
    const float colorGap = colorOriginalGap - 25.0f; // extra spacing for colors
    const float startXColor = -((colorCols - 1) * colorGap) / 2.0f;
    const float colorY = -90.0f;

    for (int i = 0; i < (int)colorTypes.size(); i++) {
        float x = startXColor + i * colorGap;
        float y = colorY;

        auto label = CCLabelBMFont::create(colorTypes[i].second.c_str(), "bigFont.fnt");
        label->setPosition({size.width / 2.0f + x, size.height / 2.0f + y + 15.0f});
        label->setScale(0.45f);
        labelNode->addChild(label);

        auto toggler = createToggler(colorTypes[i].first, {x, y - 5.0f});
        toggler->setPosition({size.width / 2.0f + x, size.height / 2.0f + y - 5.0f});
        menu->addChild(toggler);
    }

    labelNode->setPosition({0, 15});
    menu->setPosition({0, 15});

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
    if (ret->initAnchored(420.0f, 280.0f, "SquareThing01.png"_spr)) {
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

    // containers
    auto menu = CCMenu::create();
    menu->setContentSize(size);
    this->m_mainLayer->addChild(menu);

    auto labelNode = CCNode::create();
    labelNode->setContentSize(size);
    this->m_mainLayer->addChild(labelNode);

    const float midX = size.width / 2.0f;
    const float midY = size.height / 2.0f;

    const float sidesPadding = 95.f;
    const float fuck = 9.f;
    const float leftX = -140.0f;
    const float rightX = 140.0f;
    const float topY = 72.0f;      // toggler row
    const float satY = 24.0f;      // saturation slider row
    const float brightY = -18.0f;  // brightness slider row
    const float speedY = -84.0f;   // rgb speed center row
    const float nameLabelNudge = 18.0f; // move name labels upward

    // Color 1 column
    auto col1Title = CCLabelBMFont::create("Color 1", "bigFont.fnt");
    col1Title->setPosition({midX - sidesPadding, midY + topY + nameLabelNudge});
    col1Title->setScale(0.5f);
    labelNode->addChild(col1Title);

    auto col1Toggler = createToggler("rgb-col1", {leftX, topY - 7.0f});
    col1Toggler->setPosition({midX - sidesPadding, midY + topY - 7.0f});
    menu->addChild(col1Toggler);

    auto sat1Label = CCLabelBMFont::create("Saturation", "bigFont.fnt");
    sat1Label->setPosition({midX - sidesPadding, midY + satY + nameLabelNudge - fuck});
    sat1Label->setScale(0.45f);
    labelNode->addChild(sat1Label);

    auto sat1Slider = createSlider("rgb-saturation", {leftX, satY}, 140.0f);
    sat1Slider->setPosition({midX - sidesPadding, midY + satY - fuck});
    menu->addChild(sat1Slider);
    int sat1Tag = sat1Slider->getTag();
    {
        auto vl = CCLabelBMFont::create(std::to_string(Mod::get()->getSettingValue<double>("rgb-saturation")).substr(0,4).c_str(), "bigFont.fnt");
        vl->setPosition({midX - sidesPadding, midY + satY - 18.0f - fuck});
        vl->setScale(0.3f);
        vl->setOpacity(150);
        labelNode->addChild(vl);
        this->m_valueLabels[sat1Tag] = vl;
    }

    auto bright1Label = CCLabelBMFont::create("Brightness", "bigFont.fnt");
    bright1Label->setPosition({midX - sidesPadding, midY + brightY + nameLabelNudge - fuck*2 - 5.f});
    bright1Label->setScale(0.45f);
    labelNode->addChild(bright1Label);

    auto bright1Slider = createSlider("rgb-brightness1", {leftX, brightY}, 140.0f);
    bright1Slider->setPosition({midX - sidesPadding, midY + brightY - fuck*2 - 5.f});
    menu->addChild(bright1Slider);
    int bright1Tag = bright1Slider->getTag();
    {
        auto vl = CCLabelBMFont::create(std::to_string(Mod::get()->getSettingValue<double>("rgb-brightness1")).substr(0,4).c_str(), "bigFont.fnt");
        vl->setPosition({midX - sidesPadding, midY + brightY - 18.0f - fuck*2 - 5.f});
        vl->setScale(0.3f);
        vl->setOpacity(150);
        labelNode->addChild(vl);
        this->m_valueLabels[bright1Tag] = vl;
    }

    // Color 2 column
    auto col2Title = CCLabelBMFont::create("Color 2", "bigFont.fnt");
    col2Title->setPosition({midX + sidesPadding, midY + topY + nameLabelNudge});
    col2Title->setScale(0.5f);
    labelNode->addChild(col2Title);

    auto col2Toggler = createToggler("rgb-col2", {rightX, topY - 7.0f});
    col2Toggler->setPosition({midX + sidesPadding, midY + topY - 7.0f});
    menu->addChild(col2Toggler);

    auto sat2Label = CCLabelBMFont::create("Saturation", "bigFont.fnt");
    sat2Label->setPosition({midX + sidesPadding, midY + satY + nameLabelNudge - fuck});
    sat2Label->setScale(0.45f);
    labelNode->addChild(sat2Label);

    auto sat2Slider = createSlider("rgb-saturation2", {rightX, satY}, 140.0f);
    sat2Slider->setPosition({midX + sidesPadding, midY + satY - fuck});
    menu->addChild(sat2Slider);
    int sat2Tag = sat2Slider->getTag();
    {
        auto vl = CCLabelBMFont::create(std::to_string(Mod::get()->getSettingValue<double>("rgb-saturation2")).substr(0,4).c_str(), "bigFont.fnt");
        vl->setPosition({midX + sidesPadding, midY + satY - 18.0f - fuck});
        vl->setScale(0.3f);
        vl->setOpacity(150);
        labelNode->addChild(vl);
        this->m_valueLabels[sat2Tag] = vl;
    }

    auto bright2Label = CCLabelBMFont::create("Brightness", "bigFont.fnt");
    bright2Label->setPosition({midX + sidesPadding, midY + brightY + nameLabelNudge - fuck*2 - 5.f});
    bright2Label->setScale(0.45f);
    labelNode->addChild(bright2Label);

    auto bright2Slider = createSlider("rgb-brightness2", {rightX, brightY}, 140.0f);
    bright2Slider->setPosition({midX + sidesPadding, midY + brightY - fuck*2 - 5.f});
    menu->addChild(bright2Slider);
    int bright2Tag = bright2Slider->getTag();
    {
        auto vl = CCLabelBMFont::create(std::to_string(Mod::get()->getSettingValue<double>("rgb-brightness2")).substr(0,4).c_str(), "bigFont.fnt");
        vl->setPosition({midX + sidesPadding, midY + brightY - 18.0f - fuck*2 - 5.f});
        vl->setScale(0.3f);
        vl->setOpacity(150);
        labelNode->addChild(vl);
        this->m_valueLabels[bright2Tag] = vl;
    }

    // RGB speed
    auto speedLabel = CCLabelBMFont::create("RGB Speed", "bigFont.fnt");
    speedLabel->setPosition({midX, midY + speedY + nameLabelNudge - fuck*1.5f});
    speedLabel->setScale(0.45f);
    labelNode->addChild(speedLabel);

    auto speedSlider = createSlider("rgb-speed", {0.0f, speedY}, 240.0f);
    speedSlider->setPosition({midX, midY + speedY - fuck*1.5f});
    menu->addChild(speedSlider);
    int speedTag = speedSlider->getTag();
    {
        float initSpeed = Mod::get()->getSettingValue<double>("rgb-speed");
        auto vl = CCLabelBMFont::create(std::to_string(initSpeed).substr(0,4).c_str(), "bigFont.fnt");
        vl->setPosition({midX, midY + speedY - 18.0f - fuck*1.5f});
        vl->setScale(0.3f);
        vl->setOpacity(150);
        labelNode->addChild(vl);
        this->m_valueLabels[speedTag] = vl;
    }

    menu->setPosition({0, 0});
    labelNode->setPosition({0, 0});

    return true;
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
    if (ret->initAnchored(280.f, 250.0f, "SquareThing01.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool RGBExtrasPopup::setup() {
    this->setTitle("RGB Icons - Extras");
    auto size = this->m_mainLayer->getContentSize();

    // containers
    auto menu = CCMenu::create();
    menu->setContentSize(size);
    this->m_mainLayer->addChild(menu);

    auto labelNode = CCNode::create();
    labelNode->setContentSize(size);
    this->m_mainLayer->addChild(labelNode);

    const float midX = size.width / 2.0f;
    const float midY = size.height / 2.0f;

    std::vector<std::pair<std::string, std::string>> extras = {
        {"rgb-wave", "RGB Wave"},
        {"rgb-trail", "RGB Trail"},
        {"rgb-dash", "RGB Dash"},
        {"better-immersion-mode", "Immersion Mode"},
        {"ignore-p2", "Ignore P2"}
    };

    const float leftX = -120.0f;
    const float rightX = 120.0f;
    const float startY = 72.0f;
    const float rowGap = 25.0f;
    const float fixCenter = 35.f;

    for (size_t i = 0; i < extras.size(); ++i) {
        float y = startY - (float)i * rowGap;
        auto label = CCLabelBMFont::create(extras[i].second.c_str(), "bigFont.fnt");
        label->setAnchorPoint({1.0f, 0.5f});
        label->setPosition({midX - 20.f + fixCenter, midY + y});
        label->setScale(0.45f);
        labelNode->addChild(label);

        auto offS = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto onS  = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        offS->setScale(0.7f);
        onS->setScale(0.7f);
        auto toggler = CCMenuItemToggler::create(offS, onS, this, menu_selector(RGBExtrasPopup::onToggle));
        toggler->setPosition({midX + 20.f + fixCenter, midY + y - 2.f});
        toggler->setScale(0.95f);
        int tag = static_cast<int>(std::hash<std::string>{}(extras[i].first));
        toggler->setTag(tag);
        this->m_tagToSetting[tag] = extras[i].first;
        bool initial = Mod::get()->getSettingValue<bool>(extras[i].first);
        toggler->toggle(initial);
        toggler->setUserObject(CCString::create(extras[i].first));
        menu->addChild(toggler);
    }

    // p2-distance slider
    const float sliderY = startY - extras.size() * rowGap - 30.0f;
    auto pdLabel = CCLabelBMFont::create("Col2 Distance", "bigFont.fnt");
    pdLabel->setPosition({midX, midY + sliderY + 19.0f});
    pdLabel->setScale(0.45f);
    labelNode->addChild(pdLabel);

    auto pdSlider = Slider::create(this, menu_selector(RGBExtrasPopup::onSlider), 0.8f);
    pdSlider->setAnchorPoint({0.f, 0.f});
    pdSlider->setPosition({midX, midY + sliderY});
    pdSlider->m_sliderBar->setContentSize({200.0f, pdSlider->m_sliderBar->getContentSize().height});
    int pdTag = static_cast<int>(std::hash<std::string>{}("p2-distance"));
    pdSlider->setTag(pdTag);
    pdSlider->setUserObject(CCString::create("p2-distance"));
    float pdValue = Mod::get()->getSettingValue<double>("p2-distance");
    pdSlider->setValue(pdValue);
    this->m_tagToSetting[pdTag] = "p2-distance";
    menu->addChild(pdSlider);

    auto pdValueLabel = CCLabelBMFont::create(std::to_string(pdValue).substr(0,4).c_str(), "bigFont.fnt");
    pdValueLabel->setPosition({midX, midY + sliderY - 18.0f});
    pdValueLabel->setScale(0.4f);
    pdValueLabel->setOpacity(150);
    labelNode->addChild(pdValueLabel);
    this->m_valueLabels[pdTag] = pdValueLabel;

    menu->setPosition({0, -5});
    labelNode->setPosition({0, -5});

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
        } else {
            return;
        }
    }

    std::string id;
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) {
        id = it->second;
    } else {
        if (auto cs = dynamic_cast<CCString*>(userObj)) {
            id = cs->getCString();
        } else {
            return;
        }
    }

    if (id == "p2-distance") {
        Mod::get()->setSettingValue<double>(id, value);
        if (this->m_valueLabels.count(tag)) {
            this->m_valueLabels[tag]->setString(std::to_string(value).substr(0, 4).c_str());
        }
    }
}

// ===== CUSTOM COLORS POPUP (ccColor3B) =====
CustomColorsPopup* CustomColorsPopup::create() {
    auto ret = new CustomColorsPopup();
    if (ret->initAnchored(320.0f, 300.0f, "SquareThing01.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool CustomColorsPopup::setup() {
    this->setTitle("Custom Colors");

    auto size = this->m_mainLayer->getContentSize();
    const float midX = size.width / 2.0f;
    const float midY = size.height / 2.0f;

    // containers
    auto menu = CCMenu::create();
    menu->setContentSize(size);
    this->m_mainLayer->addChild(menu);

    auto labelNode = CCNode::create();
    labelNode->setContentSize(size);
    this->m_mainLayer->addChild(labelNode);

    auto enableToggler = createToggler("enable-customcolors", {midX, size.height - 45.f});
    enableToggler->setID("enable-custom-colors");
    menu->addChild(enableToggler);

    // Columns X offsets from center
    const float colOffset = 75.0f;
    const float startY = 75.0f;
    const float rowGap = 28.0f;

    // Left (Player 1) labels + buttons + toggles
    std::vector<std::pair<std::string, std::string>> p1Colors = {
        {"p1-primary", "Col 1"},
        {"p1-secondary", "Col 2"},
        {"p1-glow", "Glow"},
        {"p1-wave", "Wave"},
        {"p1-trail", "Trail"},
        {"p1-ghost-trail", "Ghost"},
        {"p1-dashfire", "Dash"},
        {"p1-ufodome", "Dome"}
    };

    // Right (Player 2)
    std::vector<std::pair<std::string, std::string>> p2Colors = {
        {"p2-primary", "Col 1"},
        {"p2-secondary", "Col 2"},
        {"p2-glow", "Glow"},
        {"p2-wave", "Wave"},
        {"p2-trail", "Trail"},
        {"p2-ghost-trail", "Ghost"},
        {"p2-dashfire", "Dash"},
        {"p2-ufodome", "Dome"}
    };

    // Add rows for both players (labels in labelNode, color buttons in menu, toggles in menu)
    for (size_t i = 0; i < p1Colors.size(); ++i) {
        float y = startY - 5.f - (float)i * rowGap;

        // Player 1 label
        auto labL = CCLabelBMFont::create(p1Colors[i].second.c_str(), "bigFont.fnt");
        labL->setPosition({midX - colOffset - 36.0f, midY + y});
        labL->setScale(0.45f);
        labelNode->addChild(labL);

        // Player 1 color button
        auto p1btn = createColorPickerButton(p1Colors[i].first, {midX - colOffset + 10.0f, midY + y}, Mod::get()->getSettingValue<cocos2d::ccColor3B>(p1Colors[i].first));
        p1btn->setID((p1Colors[i].first + "-btn").c_str());
        menu->addChild(p1btn);

        // Player 2 label
        auto labR = CCLabelBMFont::create(p2Colors[i].second.c_str(), "bigFont.fnt");
        labR->setPosition({midX + colOffset - 36.0f, midY + y});
        labR->setScale(0.45f);
        labelNode->addChild(labR);

        // Player 2 color button
        auto p2btn = createColorPickerButton(p2Colors[i].first, {midX + colOffset + 10.0f, midY + y}, Mod::get()->getSettingValue<cocos2d::ccColor3B>(p2Colors[i].first));
        p2btn->setID((p2Colors[i].first + "-btn").c_str());
        menu->addChild(p2btn);

        // For the rows that require a toggler (Trail/Ghost/Dash/Dome) add a toggler to each side
        if (i >= 4) {
            // map row index 4..7 to global toggles (existing settings)
            // trail  -> tint-trail
            // ghost  -> tint-ghost-trail
            // dash   -> tint-dashfire
            // dome   -> tint-ufodome
            static const std::array<std::string,4> mapTog = {"tint-trail", "tint-ghost-trail", "tint-dashfire", "tint-ufodome"};
            auto settingId = mapTog[i - 4];

            auto t1 = createToggler((settingId + "-p1").c_str(), {midX - colOffset + 26.0f + 13.f, midY + y});
            t1->setID((settingId + "-p1").c_str());
            menu->addChild(t1);

            auto t2 = createToggler((settingId + "-p2").c_str(), {midX + colOffset + 26.0f + 13.f, midY + y});
            t2->setID((settingId + "-p2").c_str());
            menu->addChild(t2);
        }
    }

    // Anchor containers to center
    menu->setPosition({0, 0});
    labelNode->setPosition({0, 0});

    return true;
}

// helper toggler for CustomColorsPopup
CCMenuItemToggler* CustomColorsPopup::createToggler(std::string settingId, CCPoint position) {
    auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto onSprite  = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    offSprite->setScale(0.8f);
    onSprite->setScale(0.8f);
    auto toggler = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(CustomColorsPopup::onToggle));
    toggler->setPosition(position);

    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    toggler->setTag(tag);
    this->m_tagToSetting[tag] = settingId;

    bool initial = false;
    // for toggles that are global, read their current setting; if not present, default false
    try { initial = Mod::get()->getSettingValue<bool>(settingId); } catch(...) {}

    toggler->toggle(initial);
    toggler->setUserObject(CCString::create(settingId));
    return toggler;
}

void CustomColorsPopup::onToggle(CCObject* sender) {
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

// update: set ID on color picker creation (preserve existing function)
CCMenuItemSpriteExtra* CustomColorsPopup::createColorPickerButton(const std::string& settingId, CCPoint position, cocos2d::ccColor3B currentColor) {
    auto colorSprite = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    colorSprite->setColor(currentColor);
    colorSprite->setScale(0.8f);

    auto button = CCMenuItemSpriteExtra::create(
        colorSprite,
        this,
        menu_selector(CustomColorsPopup::onColorPicker)
    );

    button->setPosition(position);
    button->setUserObject(CCString::create(settingId));
    // set ID to help later organization
    button->setID((settingId + "-btn").c_str());

    return button;
}

void CustomColorsPopup::onColorPicker(CCObject* sender) {
    auto menuItem = static_cast<CCMenuItemSpriteExtra*>(sender);
    if (!menuItem) return;
    
    // Get the setting ID from user data
    auto userObj = menuItem->getUserObject();
    if (!userObj) return;
    
    auto settingIdStr = dynamic_cast<CCString*>(userObj);
    if (!settingIdStr) return;
    
    m_currentSettingId = std::string(settingIdStr->getCString());

    // Store direct reference to the button sprite for instant updates
    m_currentButtonSprite = dynamic_cast<CCSprite*>(menuItem->getNormalImage());
    
    // Get current color from settings
    auto currentColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>(m_currentSettingId);
    
    // Create and show color picker popup
    auto colorPopup = geode::ColorPickPopup::create(currentColor);
    colorPopup->setDelegate(this);
    colorPopup->show();
}

void CustomColorsPopup::updateColor(cocos2d::ccColor4B const& color) {
    if (m_currentSettingId.empty()) return;
    
    // Convert ccColor4B to ccColor3B (ignore alpha)
    cocos2d::ccColor3B color3B = {color.r, color.g, color.b};
    
    // Save to mod settings
    Mod::get()->setSettingValue<cocos2d::ccColor3B>(m_currentSettingId, color3B);
    
    // Update the button color instantly using direct reference
    if (m_currentButtonSprite) {
        // Update button color (ignoring alpha for display)
        m_currentButtonSprite->setColor({color.r, color.g, color.b});
    }
    
    geode::log::debug("CustomColors: Updated {} to ({}, {}, {})", m_currentSettingId, color.r, color.g, color.b);
}

// ===== PARTICLES POPUP (ccColor4B) =====
ParticlesPopup* ParticlesPopup::create() {
    auto ret = new ParticlesPopup();
    if (ret->initAnchored(280.0f, 210.0f, "SquareThing01.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ParticlesPopup::setup() {
    this->setTitle("Particle Colors");
    auto size = this->m_mainLayer->getContentSize();
    const float midX = size.width / 2.0f;
    const float midY = size.height / 2.0f;

    const float togglerScale = 0.7f;
    const float colBtnGap = 15.f;
    const float togglerGap = 30.f;

    // containers
    auto menu = CCMenu::create();
    menu->setContentSize(size);
    this->m_mainLayer->addChild(menu);

    auto labelNode = CCNode::create();
    labelNode->setContentSize(size);
    this->m_mainLayer->addChild(labelNode);

    // Layout values
    const float colOffset = 60.f;
    const float startY = 36.f;
    const float rowGap = 32.f;

    // Main tint toggles (global) -- show per-player toggler for easy access
    auto mainLabel = CCLabelBMFont::create("Main", "bigFont.fnt");
    mainLabel->setPosition({midX - colOffset, midY + startY});
    mainLabel->setScale(0.45f);
    labelNode->addChild(mainLabel);

    auto mainLabelR = CCLabelBMFont::create("Main", "bigFont.fnt");
    mainLabelR->setPosition({midX + colOffset, midY + startY});
    mainLabelR->setScale(0.45f);
    labelNode->addChild(mainLabelR);

    auto mainT1 = createToggler("tint-mainparticles-p1", {midX - colOffset + togglerGap, midY + startY});
    mainT1->setID("tint-mainparticles-p1");
    mainT1->setScale(togglerScale);
    menu->addChild(mainT1);
    auto mainT2 = createToggler("tint-mainparticles-p2", {midX + colOffset + togglerGap, midY + startY});
    mainT2->setID("tint-mainparticles-p2");
    mainT2->setScale(togglerScale);
    menu->addChild(mainT2);

    // Color buttons (Main start/end) for each player
    // Player 1 two buttons
    auto p1b1 = createColorPickerButton("p1-main-particles-start", {midX - colOffset - colBtnGap, midY + startY - rowGap}, Mod::get()->getSettingValue<cocos2d::ccColor4B>("p1-main-particles-start"));
    p1b1->setID("p1-main-particles-start-btn");
    menu->addChild(p1b1);
    auto p1b2 = createColorPickerButton("p1-main-particles-end", {midX - colOffset + colBtnGap, midY + startY - rowGap}, Mod::get()->getSettingValue<cocos2d::ccColor4B>("p1-main-particles-end"));
    p1b2->setID("p1-main-particles-end-btn");
    menu->addChild(p1b2);

    // Player 2 two buttons
    auto p2b1 = createColorPickerButton("p2-main-particles-start", {midX + colOffset - colBtnGap, midY + startY - rowGap}, Mod::get()->getSettingValue<cocos2d::ccColor4B>("p2-main-particles-start"));
    p2b1->setID("p2-main-particles-start-btn");
    menu->addChild(p2b1);
    auto p2b2 = createColorPickerButton("p2-main-particles-end", {midX + colOffset + colBtnGap, midY + startY - rowGap}, Mod::get()->getSettingValue<cocos2d::ccColor4B>("p2-main-particles-end"));
    p2b2->setID("p2-main-particles-end-btn");
    menu->addChild(p2b2);

    // UFO Click toggles + color buttons
    auto ufoLabelL = CCLabelBMFont::create("UFO Click", "bigFont.fnt");
    ufoLabelL->setPosition({midX - colOffset, midY + startY - rowGap * 2});
    ufoLabelL->setScale(0.45f);
    labelNode->addChild(ufoLabelL);

    auto ufoLabelR = CCLabelBMFont::create("UFO Click", "bigFont.fnt");
    ufoLabelR->setPosition({midX + colOffset, midY + startY - rowGap * 2});
    ufoLabelR->setScale(0.45f);
    labelNode->addChild(ufoLabelR);

    auto ufoT1 = createToggler("tint-ufo-click-particles-p1", {midX - colOffset + togglerGap + 20.f, midY + startY - rowGap * 2});
    ufoT1->setID("tint-ufo-click-particles-p1");
    ufoT1->setScale(togglerScale);
    menu->addChild(ufoT1);
    auto ufoT2 = createToggler("tint-ufo-click-particles-p2", {midX + colOffset + togglerGap + 20.f, midY + startY - rowGap * 2});
    ufoT2->setID("tint-ufo-click-particles-p2");
    ufoT2->setScale(togglerScale);
    menu->addChild(ufoT2);

    // color buttons for UFO click
    auto up1 = createColorPickerButton("p1-ufo-click-particles-start", {midX - colOffset - colBtnGap, midY + startY - rowGap * 3}, Mod::get()->getSettingValue<cocos2d::ccColor4B>("p1-ufo-click-particles-start"));
    up1->setID("p1-ufo-click-particles-start-btn");
    menu->addChild(up1);
    auto up2 = createColorPickerButton("p1-ufo-click-particles-end", {midX - colOffset + colBtnGap, midY + startY - rowGap * 3}, Mod::get()->getSettingValue<cocos2d::ccColor4B>("p1-ufo-click-particles-end"));
    up2->setID("p1-ufo-click-particles-end-btn");
    menu->addChild(up2);

    auto up3 = createColorPickerButton("p2-ufo-click-particles-start", {midX + colOffset - colBtnGap, midY + startY - rowGap * 3}, Mod::get()->getSettingValue<cocos2d::ccColor4B>("p2-ufo-click-particles-start"));
    up3->setID("p2-ufo-click-particles-start-btn");
    menu->addChild(up3);
    auto up4 = createColorPickerButton("p2-ufo-click-particles-end", {midX + colOffset + colBtnGap, midY + startY - rowGap * 3}, Mod::get()->getSettingValue<cocos2d::ccColor4B>("p2-ufo-click-particles-end"));
    up4->setID("p2-ufo-click-particles-end-btn");
    menu->addChild(up4);

    // start/end labels
    auto mainP1StartLabel = CCLabelBMFont::create("S", "bigFont.fnt");
    auto mainP2StartLabel = CCLabelBMFont::create("S", "bigFont.fnt");
    auto mainP1EndLabel = CCLabelBMFont::create("E", "bigFont.fnt");
    auto mainP2EndLabel = CCLabelBMFont::create("E", "bigFont.fnt");
    auto ufoClickP1StartLabel = CCLabelBMFont::create("S", "bigFont.fnt");
    auto ufoClickP2StartLabel = CCLabelBMFont::create("S", "bigFont.fnt");
    auto ufoClickP1EndLabel = CCLabelBMFont::create("E", "bigFont.fnt");
    auto ufoClickP2EndLabel = CCLabelBMFont::create("E", "bigFont.fnt");

    mainP1StartLabel->setPosition(p1b1->getPosition());
    mainP1StartLabel->setOpacity(50);
    mainP1StartLabel->setScale(0.6f);
    labelNode->addChild(mainP1StartLabel);

    mainP1EndLabel->setPosition(p1b2->getPosition());
    mainP1EndLabel->setOpacity(50);
    mainP1EndLabel->setScale(0.6f);
    labelNode->addChild(mainP1EndLabel);

    mainP2StartLabel->setPosition(p2b1->getPosition());
    mainP2StartLabel->setOpacity(50);
    mainP2StartLabel->setScale(0.6f);
    labelNode->addChild(mainP2StartLabel);

    mainP2EndLabel->setPosition(p2b2->getPosition());
    mainP2EndLabel->setOpacity(50);
    mainP2EndLabel->setScale(0.6f);
    labelNode->addChild(mainP2EndLabel);

    ufoClickP1StartLabel->setPosition(up1->getPosition());
    ufoClickP1StartLabel->setOpacity(50);
    ufoClickP1StartLabel->setScale(0.6f);
    labelNode->addChild(ufoClickP1StartLabel);

    ufoClickP1EndLabel->setPosition(up2->getPosition());
    ufoClickP1EndLabel->setOpacity(50);
    ufoClickP1EndLabel->setScale(0.6f);
    labelNode->addChild(ufoClickP1EndLabel);

    ufoClickP2StartLabel->setPosition(up3->getPosition());
    ufoClickP2StartLabel->setOpacity(50);
    ufoClickP2StartLabel->setScale(0.6f);
    labelNode->addChild(ufoClickP2StartLabel);

    ufoClickP2EndLabel->setPosition(up4->getPosition());
    ufoClickP2EndLabel->setOpacity(50);
    ufoClickP2EndLabel->setScale(0.6f);
    labelNode->addChild(ufoClickP2EndLabel);

    auto p1LabelThing = CCLabelBMFont::create("Player 1", "goldFont.fnt");
    p1LabelThing->setPosition({midX - colOffset, midY + startY + 30.f});
    p1LabelThing->setScale(0.7f);
    labelNode->addChild(p1LabelThing);

    auto p2LabelThing = CCLabelBMFont::create("Player 2", "goldFont.fnt");
    p2LabelThing->setPosition({midX + colOffset, midY + startY + 30.f});
    p2LabelThing->setScale(0.7f);
    labelNode->addChild(p2LabelThing);

    // finalize positions
    menu->setPosition({0, -10});
    labelNode->setPosition({0, -10});

    return true;
}

CCMenuItemToggler* ParticlesPopup::createToggler(std::string settingId, CCPoint position) {
    auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto onSprite  = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    offSprite->setScale(0.8f);
    onSprite->setScale(0.8f);
    auto toggler = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(ParticlesPopup::onToggle));
    toggler->setPosition(position);

    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    toggler->setTag(tag);
    this->m_tagToSetting[tag] = settingId;

    bool initial = false;
    try { initial = Mod::get()->getSettingValue<bool>(settingId); } catch(...) {}
    toggler->toggle(initial);
    toggler->setUserObject(CCString::create(settingId));
    return toggler;
}

void ParticlesPopup::onToggle(CCObject* sender) {
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

CCMenuItemSpriteExtra* ParticlesPopup::createColorPickerButton(const std::string& settingId, CCPoint position, cocos2d::ccColor4B currentColor) {
    // Create colored sprite from the color button texture
    auto colorSprite = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    // Set color (ignoring alpha for display)
    colorSprite->setColor({currentColor.r, currentColor.g, currentColor.b});
    colorSprite->setScale(0.8f);
    
    auto button = CCMenuItemSpriteExtra::create(
        colorSprite,
        this,
        menu_selector(ParticlesPopup::onColorPicker)
    );
    
    button->setPosition(position);
    
    // Store the setting ID in user data
    button->setUserObject(CCString::create(settingId));
    
    return button;
}

void ParticlesPopup::onColorPicker(CCObject* sender) {
    auto menuItem = static_cast<CCMenuItemSpriteExtra*>(sender);
    if (!menuItem) return;
    
    // Get the setting ID from user data
    auto userObj = menuItem->getUserObject();
    if (!userObj) return;
    
    auto settingIdStr = dynamic_cast<CCString*>(userObj);
    if (!settingIdStr) return;
    
    m_currentSettingId = std::string(settingIdStr->getCString());
    
    // Store direct reference to the button sprite for instant updates
    m_currentButtonSprite = dynamic_cast<CCSprite*>(menuItem->getNormalImage());
    
    // Get current color from settings
    auto currentColor = Mod::get()->getSettingValue<cocos2d::ccColor4B>(m_currentSettingId);
    
    // Create and show color picker popup (with alpha support)
    auto colorPopup = geode::ColorPickPopup::create(currentColor);
    colorPopup->setDelegate(this);
    colorPopup->show();
}

void ParticlesPopup::updateColor(cocos2d::ccColor4B const& color) {
    if (m_currentSettingId.empty()) return;
    
    // Save to mod settings (keep full ccColor4B with alpha)
    Mod::get()->setSettingValue<cocos2d::ccColor4B>(m_currentSettingId, color);
    
    // Update the button color instantly using direct reference
    if (m_currentButtonSprite) {
        // Update button color (ignoring alpha for display)
        m_currentButtonSprite->setColor({color.r, color.g, color.b});
    }
    
    geode::log::debug("Particles: Updated {} to ({}, {}, {}, {})", m_currentSettingId, color.r, color.g, color.b, color.a);
}
