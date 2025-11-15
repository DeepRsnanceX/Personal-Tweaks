// ModernSettingsPopup.cpp
#include "ModernSettingsPopup.hpp"
#include "ShaderCache.h"

extern const char* BLUR_FRAGMENT_SHADER;

ModernSettingsPopup* ModernSettingsPopup::create() {
    auto ret = new ModernSettingsPopup();
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    if (ret->initAnchored(winSize.width * 0.9f, winSize.height * 0.85f, "modernSquare01.png"_spr)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}


bool ModernSettingsPopup::setup() {
    this->setTitle("");
    
    if (auto closeBtn = this->m_closeBtn) {
        closeBtn->setVisible(false);
    }
    if (auto title = this->m_title) {
        title->setVisible(false);
    }
    
    setupSidebar();
    setupContentArea();
    switchCategory(SettingsCategory::Randomizer);
    
    return true;
}

void ModernSettingsPopup::setupSidebar() {
    auto size = this->m_mainLayer->getContentSize();
    const float sidebarWidth = 150.0f;
    
    m_sidebar = CCNode::create();
    m_sidebar->setContentSize({sidebarWidth, size.height});
    m_sidebar->setAnchorPoint({0.0f, 0.5f});
    m_sidebar->setPosition({-size.width / 2.0f + 20.0f, 0.0f});
    m_sidebar->setID("sidebar");
    this->m_mainLayer->addChild(m_sidebar);
    
    const float sidebarMidX = sidebarWidth / 2.0f;
    const float sidebarMidY = size.height / 2.0f;
    
    auto logo = CCSprite::createWithSpriteFrameName("modLogo.png"_spr);
    logo->setScale(0.7f);
    logo->setPosition({sidebarMidX, sidebarMidY + 90.0f});
    m_sidebar->addChild(logo);
    
    auto separator = CCSprite::createWithSpriteFrameName("floorLine_001.png");
    separator->setRotation(90.0f);
    separator->setScaleY(0.4f);
    separator->setPosition({sidebarMidX, sidebarMidY + 50.0f});
    separator->setOpacity(80);
    m_sidebar->addChild(separator);
    
    auto menu = CCMenu::create();
    menu->setContentSize({sidebarWidth, 240.0f});
    menu->setAnchorPoint({0.5f, 0.5f});
    menu->setPosition({sidebarMidX, sidebarMidY - 20.0f});
    menu->setID("sidebar-menu");
    menu->setLayout(
        ColumnLayout::create()
            ->setGap(10.0f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisAlignment(AxisAlignment::Center)
    );
    m_sidebar->addChild(menu);
    
    auto randomizerBtn = createSidebarButton("randomizerIcon", "Randomizer", SettingsCategory::Randomizer);
    menu->addChild(randomizerBtn);
    m_categoryButtons[SettingsCategory::Randomizer] = randomizerBtn;
    
    auto rgbBtn = createSidebarButton("rgbIcon", "RGB Icons", SettingsCategory::RGB);
    menu->addChild(rgbBtn);
    m_categoryButtons[SettingsCategory::RGB] = rgbBtn;
    
    auto colorsBtn = createSidebarButton("colorsIcon", "Colors", SettingsCategory::Colors);
    menu->addChild(colorsBtn);
    m_categoryButtons[SettingsCategory::Colors] = colorsBtn;
    
    auto particlesBtn = createSidebarButton("particlesIcon", "Particles", SettingsCategory::Particles);
    menu->addChild(particlesBtn);
    m_categoryButtons[SettingsCategory::Particles] = particlesBtn;
    
    menu->updateLayout();
}

void ModernSettingsPopup::setupContentArea() {
    auto size = this->m_mainLayer->getContentSize();
    
    const float contentWidth = 325.0f;
    const float contentHeight = 255.0f;
    
    m_contentArea = CCNode::create();
    m_contentArea->setContentSize({contentWidth, contentHeight});
    m_contentArea->setAnchorPoint({0.f, 0.5f});
    m_contentArea->setPosition({size.width / 2.0f - 40.0f, size.height / 2.f});
    m_contentArea->setID("content-area");
    this->m_mainLayer->addChild(m_contentArea);
}

CCMenuItemSprite* ModernSettingsPopup::createSidebarButton(const std::string& iconName, const std::string& labelText, SettingsCategory category) {
    const float buttonWidth = 130.0f;
    const float buttonHeight = 38.0f;
    
    auto normalContent = CCNode::create();
    normalContent->setAnchorPoint({0.5f, 0.5f});
    normalContent->setLayout(
        RowLayout::create()
            ->setGap(8.0f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setAutoScale(false)
    );
    
    auto normalIcon = CCSprite::createWithSpriteFrameName(fmt::format("{}.png"_spr, iconName).c_str());
    normalIcon->setScale(0.6f);
    normalContent->addChild(normalIcon);
    
    auto normalLabel = CCLabelBMFont::create(labelText.c_str(), "modernMain.fnt"_spr);
    normalLabel->setScale(0.45f);
    normalLabel->setOpacity(200);
    normalContent->addChild(normalLabel);
    
    normalContent->updateLayout();
    normalContent->setPosition({buttonWidth / 2.0f, buttonHeight / 2.0f});
    normalBG->addChild(normalContent);
    
    auto selectedContent = CCNode::create();
    selectedContent->setContentSize({buttonWidth - 10.0f, buttonHeight - 10.0f});
    selectedContent->setAnchorPoint({0.5f, 0.5f});
    selectedContent->setLayout(
        RowLayout::create()
            ->setGap(8.0f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setAutoScale(false)
    );
    
    auto selectedIcon = CCSprite::createWithSpriteFrameName(fmt::format("{}Selected.png"_spr, iconName).c_str());
    selectedIcon->setScale(0.7f);
    selectedContent->addChild(selectedIcon);
    
    auto selectedLabel = CCLabelBMFont::create(labelText.c_str(), "modernMain.fnt"_spr);
    selectedLabel->setScale(0.45f);
    selectedLabel->setOpacity(230);
    selectedContent->addChild(selectedLabel);
    
    selectedContent->updateLayout();
    selectedContent->setPosition({buttonWidth / 2.0f, buttonHeight / 2.0f});
    selectedBG->addChild(selectedContent);
    
    auto button = CCMenuItemSprite::create(
        static_cast<CCNode*>(normalBG),
        static_cast<CCNode*>(selectedBG),
        this,
        menu_selector(ModernSettingsPopup::onCategoryButton)
    );
    button->setTag(static_cast<int>(category));
    button->setID((iconName + "-btn").c_str());
    
    return button;
}

void ModernSettingsPopup::onCategoryButton(CCObject* sender) {
    auto button = static_cast<CCMenuItemSprite*>(sender);
    auto category = static_cast<SettingsCategory>(button->getTag());
    switchCategory(category);
}

void ModernSettingsPopup::updateCategoryButtonStates() {
    for (auto& [cat, btn] : m_categoryButtons) {
        if (cat == m_currentCategory) {
            btn->selected();
        } else {
            btn->unselected();
        }
    }
}

void ModernSettingsPopup::switchCategory(SettingsCategory category) {
    m_currentCategory = category;
    updateCategoryButtonStates();
    
    m_contentArea->removeAllChildren();
    m_tagToSetting.clear();
    m_valueLabels.clear();
    m_currentPage = 0;
    
    switch (category) {
        case SettingsCategory::Randomizer:
            buildRandomizerContent();
            break;
        case SettingsCategory::RGB:
            buildRGBContent();
            break;
        case SettingsCategory::Colors:
            buildColorsContent();
            break;
        case SettingsCategory::Particles:
            buildParticlesContent();
            break;
    }
}

void ModernSettingsPopup::buildRandomizerContent() {
    auto size = m_contentArea->getContentSize();
    const float midX = size.width / 2.0f;
    
    auto title = CCLabelBMFont::create("Randomizer", "modernBold.fnt"_spr);
    title->setPosition({midX, size.height - 25.0f});
    title->setScale(0.7f);
    m_contentArea->addChild(title);
    
    auto separator = CCSprite::createWithSpriteFrameName("floorLine_001.png");
    separator->setPosition({midX, size.height - 45.0f});
    separator->setScaleX(size.width / separator->getContentSize().width * 0.9f);
    separator->setOpacity(80);
    m_contentArea->addChild(separator);
    
    auto contentContainer = CCNode::create();
    contentContainer->setContentSize({size.width - 20.0f, size.height - 60.0f});
    contentContainer->setPosition({10.0f, 10.0f});
    contentContainer->setAnchorPoint({0.0f, 0.0f});
    contentContainer->setLayout(
        ColumnLayout::create()
            ->setGap(15.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(false)
            ->setCrossAxisAlignment(AxisAlignment::Start)
            ->setGrowCrossAxis(true)
    );
    m_contentArea->addChild(contentContainer);
    
    auto condSection = CCNode::create();
    condSection->setContentSize({size.width - 40.0f, 160.0f});
    
    auto condTitle = CCLabelBMFont::create("Conditional", "modernBold.fnt"_spr);
    condTitle->setPosition({10.0f, 150.0f});
    condTitle->setAnchorPoint({0.0f, 1.0f});
    condTitle->setScale(0.5f);
    condTitle->setOpacity(220);
    condSection->addChild(condTitle);
    
    auto condMenu = CCMenu::create();
    condMenu->setContentSize(condSection->getContentSize());
    condMenu->setPosition({0, 0});
    condSection->addChild(condMenu);
    
    auto condLabels = CCNode::create();
    condLabels->setContentSize(condSection->getContentSize());
    condLabels->setPosition({0, 0});
    condSection->addChild(condLabels);
    
    auto enableLabel = CCLabelBMFont::create("Enable", "modernMain.fnt"_spr);
    enableLabel->setPosition({10.0f, 120.0f});
    enableLabel->setAnchorPoint({0.0f, 0.5f});
    enableLabel->setScale(0.45f);
    enableLabel->setOpacity(200);
    condLabels->addChild(enableLabel);
    
    auto enableToggle = createToggler("cond-ic-randomizer", {condSection->getContentSize().width - 25.0f, 120.0f});
    enableToggle->setScale(0.75f);
    condMenu->addChild(enableToggle);
    
    std::vector<std::pair<std::string, std::string>> condModes = {
        {"cond-ongamemodechange", "Every Gamemode"},
        {"cond-ondeath", "On Death"},
        {"cond-onclick", "On Click"}
    };
    
    float modeY = 85.0f;
    for (const auto& [settingId, name] : condModes) {
        auto label = CCLabelBMFont::create(name.c_str(), "modernMain.fnt"_spr);
        label->setPosition({10.0f, modeY});
        label->setAnchorPoint({0.0f, 0.5f});
        label->setScale(0.4f);
        label->setOpacity(180);
        condLabels->addChild(label);
        
        auto toggler = createToggler(settingId, {condSection->getContentSize().width - 25.0f, modeY});
        toggler->setScale(0.7f);
        condMenu->addChild(toggler);
        
        modeY -= 25.0f;
    }
    
    contentContainer->addChild(condSection);
    
    auto liveSection = CCNode::create();
    liveSection->setContentSize({size.width - 40.0f, 120.0f});
    
    auto liveTitle = CCLabelBMFont::create("Live", "modernBold.fnt"_spr);
    liveTitle->setPosition({10.0f, 110.0f});
    liveTitle->setAnchorPoint({0.0f, 1.0f});
    liveTitle->setScale(0.5f);
    liveTitle->setOpacity(220);
    liveSection->addChild(liveTitle);
    
    auto liveMenu = CCMenu::create();
    liveMenu->setContentSize(liveSection->getContentSize());
    liveMenu->setPosition({0, 0});
    liveSection->addChild(liveMenu);
    
    auto liveLabels = CCNode::create();
    liveLabels->setContentSize(liveSection->getContentSize());
    liveLabels->setPosition({0, 0});
    liveSection->addChild(liveLabels);
    
    // enable
    auto liveEnableLabel = CCLabelBMFont::create("Enable", "modernMain.fnt"_spr);
    liveEnableLabel->setPosition({10.0f, 80.0f});
    liveEnableLabel->setAnchorPoint({0.0f, 0.5f});
    liveEnableLabel->setScale(0.45f);
    liveEnableLabel->setOpacity(200);
    liveLabels->addChild(liveEnableLabel);
    
    auto liveEnableToggle = createToggler("live-ic-randomizer", {liveSection->getContentSize().width - 25.0f, 80.0f});
    liveEnableToggle->setScale(0.75f);
    liveMenu->addChild(liveEnableToggle);
    
    // delay
    auto delayLabel = CCLabelBMFont::create("Delay", "modernMain.fnt"_spr);
    delayLabel->setPosition({10.0f, 45.0f});
    delayLabel->setAnchorPoint({0.0f, 0.5f});
    delayLabel->setScale(0.4f);
    delayLabel->setOpacity(180);
    liveLabels->addChild(delayLabel);
    
    const float liveMidX = liveSection->getContentSize().width / 2.0f;
    auto delaySlider = createSlider("random-delay", {liveMidX, 20.0f}, liveSection->getContentSize().width - 40.0f);
    liveMenu->addChild(delaySlider);
    
    float currentDelay = Mod::get()->getSettingValue<double>("random-delay");
    auto delayValue = CCLabelBMFont::create((std::to_string(currentDelay).substr(0, 4) + "s").c_str(), "modernMain.fnt"_spr);
    delayValue->setPosition({liveMidX, 5.0f});
    delayValue->setScale(0.3f);
    delayValue->setOpacity(150);
    liveLabels->addChild(delayValue);
    m_valueLabels[delaySlider->getTag()] = delayValue;
    
    contentContainer->addChild(liveSection);
    
    // === MODES SECTION ===
    auto modesSection = CCNode::create();
    modesSection->setContentSize({size.width - 40.0f, 160.0f});
    
    auto modesTitle = CCLabelBMFont::create("Modes", "modernBold.fnt"_spr);
    modesTitle->setPosition({10.0f, 150.0f});
    modesTitle->setAnchorPoint({0.0f, 1.0f});
    modesTitle->setScale(0.5f);
    modesTitle->setOpacity(220);
    modesSection->addChild(modesTitle);
    
    auto modesMenu = CCMenu::create();
    modesMenu->setContentSize(modesSection->getContentSize());
    modesMenu->setPosition({0, 0});
    modesSection->addChild(modesMenu);
    
    auto modesLabels = CCNode::create();
    modesLabels->setContentSize(modesSection->getContentSize());
    modesLabels->setPosition({0, 0});
    modesSection->addChild(modesLabels);
    
    // icon types
    std::vector<std::pair<std::string, std::string>> iconTypes = {
        {"randomize-cube", "Cube"}, {"randomize-ship", "Ship"},
        {"randomize-ball", "Ball"}, {"randomize-ufo", "UFO"},
        {"randomize-wave", "Wave"}, {"randomize-robot", "Robot"},
        {"randomize-spider", "Spider"}, {"randomize-swing", "Swing"}
    };
    
    const int cols = 4;
    const float modesMidX = modesSection->getContentSize().width / 2.0f;
    const float colWidth = (modesSection->getContentSize().width - 20.0f) / cols;
    const float startX = modesMidX - (cols * colWidth) / 2.0f + colWidth / 2.0f;
    
    for (size_t i = 0; i < iconTypes.size(); ++i) {
        int row = i / cols;
        int col = i % cols;
        float x = startX + col * colWidth;
        float y = 110.0f - row * 35.0f;
        
        auto label = CCLabelBMFont::create(iconTypes[i].second.c_str(), "modernMain.fnt"_spr);
        label->setPosition({x, y});
        label->setScale(0.35f);
        label->setOpacity(180);
        modesLabels->addChild(label);
        
        auto toggler = createToggler(iconTypes[i].first, {x, y - 15.0f});
        toggler->setScale(0.65f);
        modesMenu->addChild(toggler);
    }
    
    // color types
    std::vector<std::pair<std::string, std::string>> colorTypes = {
        {"randomize-color1", "Color 1"}, {"randomize-color2", "Color 2"}, {"randomize-glowcolor", "Glow"}
    };
    
    const float colorColWidth = (modesSection->getContentSize().width - 20.0f) / 3.0f;
    const float colorStartX = modesMidX - (3 * colorColWidth) / 2.0f + colorColWidth / 2.0f;
    const float colorY = 30.0f;
    
    for (size_t i = 0; i < colorTypes.size(); ++i) {
        float x = colorStartX + i * colorColWidth;
        
        auto label = CCLabelBMFont::create(colorTypes[i].second.c_str(), "modernMain.fnt"_spr);
        label->setPosition({x, colorY});
        label->setScale(0.35f);
        label->setOpacity(180);
        modesLabels->addChild(label);
        
        auto toggler = createToggler(colorTypes[i].first, {x, colorY - 15.0f});
        toggler->setScale(0.65f);
        modesMenu->addChild(toggler);
    }
    
    contentContainer->addChild(modesSection);
    contentContainer->updateLayout();

    // bottom navigation buttons
    auto navMenu = CCMenu::create();
    navMenu->setContentSize({size.width, 35.0f});
    navMenu->setPosition({midX, 17.5f});
    navMenu->setAnchorPoint({0.5f, 0.5f});
    navMenu->setLayout(
        RowLayout::create()
            ->setGap(8.0f)
            ->setAxisAlignment(AxisAlignment::Center)
    );
    m_contentArea->addChild(navMenu);
    
    auto condBtn = createNavButton("Cond.");
    navMenu->addChild(condBtn);
    
    auto liveBtn = createNavButton("Live");
    navMenu->addChild(liveBtn);
    
    auto modesBtn = createNavButton("Modes");
    navMenu->addChild(modesBtn);
    
    navMenu->updateLayout();
}

void ModernSettingsPopup::buildRGBContent() {
    auto size = m_contentArea->getContentSize();
    const float midX = size.width / 2.0f;
    
    // title
    auto title = CCLabelBMFont::create("RGB Icons", "modernBold.fnt"_spr);
    title->setPosition({midX, size.height - 25.0f});
    title->setScale(0.7f);
    m_contentArea->addChild(title);
    
    // separator
    auto separator = CCSprite::createWithSpriteFrameName("floorLine_001.png");
    separator->setPosition({midX, size.height - 45.0f});
    separator->setScaleX(size.width / separator->getContentSize().width * 0.9f);
    separator->setOpacity(80);
    m_contentArea->addChild(separator);
    
    // main content
    auto contentNode = CCNode::create();
    contentNode->setContentSize({size.width - 20.0f, size.height - 60.0f});
    contentNode->setPosition({10.0f, 10.0f});
    contentNode->setAnchorPoint({0.0f, 0.0f});
    m_contentArea->addChild(contentNode);
    
    auto menu = CCMenu::create();
    menu->setContentSize(contentNode->getContentSize());
    menu->setPosition({0, 0});
    contentNode->addChild(menu);
    
    auto labels = CCNode::create();
    labels->setContentSize(contentNode->getContentSize());
    labels->setPosition({0, 0});
    contentNode->addChild(labels);
    
    const float contentMidX = contentNode->getContentSize().width / 2.0f;
    const float colOffset = 100.0f;
    float currentY = contentNode->getContentSize().height - 10.0f;
    
    // col 1 & 2 radio buttons
    auto col1Label = CCLabelBMFont::create("Col 1", "modernMain.fnt"_spr);
    col1Label->setPosition({contentMidX - colOffset, currentY});
    col1Label->setScale(0.45f);
    col1Label->setOpacity(200);
    labels->addChild(col1Label);
    
    auto col1Radio = createRadioButton("rgb-col1", {contentMidX - colOffset - 30.0f, currentY});
    menu->addChild(col1Radio);
    
    auto col2Label = CCLabelBMFont::create("Col 2", "modernMain.fnt"_spr);
    col2Label->setPosition({contentMidX + colOffset, currentY});
    col2Label->setScale(0.45f);
    col2Label->setOpacity(200);
    labels->addChild(col2Label);
    
    auto col2Radio = createRadioButton("rgb-col2", {contentMidX + colOffset - 30.0f, currentY});
    menu->addChild(col2Radio);
    
    currentY -= 40.0f;
    
    // brightness sliders
    auto bright1Label = CCLabelBMFont::create("Brightness", "modernMain.fnt"_spr);
    bright1Label->setPosition({contentMidX - colOffset, currentY});
    bright1Label->setScale(0.38f);
    bright1Label->setOpacity(180);
    labels->addChild(bright1Label);
    
    auto bright1Slider = createSlider("rgb-brightness1", {contentMidX - colOffset, currentY - 15.0f}, 120.0f);
    menu->addChild(bright1Slider);
    
    float b1Val = Mod::get()->getSettingValue<double>("rgb-brightness1");
    auto bright1Value = CCLabelBMFont::create(std::to_string(b1Val).substr(0,4).c_str(), "modernMain.fnt"_spr);
    bright1Value->setPosition({contentMidX - colOffset, currentY - 30.0f});
    bright1Value->setScale(0.28f);
    bright1Value->setOpacity(140);
    labels->addChild(bright1Value);
    m_valueLabels[bright1Slider->getTag()] = bright1Value;
    
    auto bright2Label = CCLabelBMFont::create("Brightness", "modernMain.fnt"_spr);
    bright2Label->setPosition({contentMidX + colOffset, currentY});
    bright2Label->setScale(0.38f);
    bright2Label->setOpacity(180);
    labels->addChild(bright2Label);
    
    auto bright2Slider = createSlider("rgb-brightness2", {contentMidX + colOffset, currentY - 15.0f}, 120.0f);
    menu->addChild(bright2Slider);
    
    float b2Val = Mod::get()->getSettingValue<double>("rgb-brightness2");
    auto bright2Value = CCLabelBMFont::create(std::to_string(b2Val).substr(0,4).c_str(), "modernMain.fnt"_spr);
    bright2Value->setPosition({contentMidX + colOffset, currentY - 30.0f});
    bright2Value->setScale(0.28f);
    bright2Value->setOpacity(140);
    labels->addChild(bright2Value);
    m_valueLabels[bright2Slider->getTag()] = bright2Value;
    
    currentY -= 55.0f;
    
    // saturation sliders
    auto sat1Label = CCLabelBMFont::create("Saturation", "modernMain.fnt"_spr);
    sat1Label->setPosition({contentMidX - colOffset, currentY});
    sat1Label->setScale(0.38f);
    sat1Label->setOpacity(180);
    labels->addChild(sat1Label);
    
    auto sat1Slider = createSlider("rgb-saturation", {contentMidX - colOffset, currentY - 15.0f}, 120.0f);
    menu->addChild(sat1Slider);
    
    float s1Val = Mod::get()->getSettingValue<double>("rgb-saturation");
    auto sat1Value = CCLabelBMFont::create(std::to_string(s1Val).substr(0,4).c_str(), "modernMain.fnt"_spr);
    sat1Value->setPosition({contentMidX - colOffset, currentY - 30.0f});
    sat1Value->setScale(0.28f);
    sat1Value->setOpacity(140);
    labels->addChild(sat1Value);
    m_valueLabels[sat1Slider->getTag()] = sat1Value;
    
    auto sat2Label = CCLabelBMFont::create("Saturation", "modernMain.fnt"_spr);
    sat2Label->setPosition({contentMidX + colOffset, currentY});
    sat2Label->setScale(0.38f);
    sat2Label->setOpacity(180);
    labels->addChild(sat2Label);
    
    auto sat2Slider = createSlider("rgb-saturation2", {contentMidX + colOffset, currentY - 15.0f}, 120.0f);
    menu->addChild(sat2Slider);
    
    float s2Val = Mod::get()->getSettingValue<double>("rgb-saturation2");
    auto sat2Value = CCLabelBMFont::create(std::to_string(s2Val).substr(0,4).c_str(), "modernMain.fnt"_spr);
    sat2Value->setPosition({contentMidX + colOffset, currentY - 30.0f});
    sat2Value->setScale(0.28f);
    sat2Value->setOpacity(140);
    labels->addChild(sat2Value);
    m_valueLabels[sat2Slider->getTag()] = sat2Value;
    
    currentY -= 55.0f;
    
    // rgb Speed
    auto speedLabel = CCLabelBMFont::create("Speed", "modernMain.fnt"_spr);
    speedLabel->setPosition({contentMidX, currentY});
    speedLabel->setScale(0.4f);
    speedLabel->setOpacity(180);
    labels->addChild(speedLabel);
    
    auto speedSlider = createSlider("rgb-speed", {contentMidX, currentY - 15.0f}, 220.0f);
    menu->addChild(speedSlider);
    
    float speedVal = Mod::get()->getSettingValue<double>("rgb-speed");
    auto speedValue = CCLabelBMFont::create(std::to_string(speedVal).substr(0,4).c_str(), "modernMain.fnt"_spr);
    speedValue->setPosition({contentMidX, currentY - 30.0f});
    speedValue->setScale(0.28f);
    speedValue->setOpacity(140);
    labels->addChild(speedValue);
    m_valueLabels[speedSlider->getTag()] = speedValue;
    
    currentY -= 60.0f;
    
    // extras section title
    auto extrasLabel = CCLabelBMFont::create("Extras", "modernBold.fnt"_spr);
    extrasLabel->setPosition({15.0f, currentY});
    extrasLabel->setAnchorPoint({0.0f, 0.5f});
    extrasLabel->setScale(0.45f);
    extrasLabel->setOpacity(200);
    labels->addChild(extrasLabel);
    
    currentY -= 30.0f;
    
    std::vector<std::pair<std::string, std::string>> extras = {
        {"rgb-wave", "RGB Wave"},
        {"rgb-trail", "RGB Trail"},
        {"rgb-dash", "RGB Dash"},
        {"better-immersion-mode", "Immersion"},
        {"ignore-p2", "Ignore P2"}
    };
    
    // layout extras in 2 columns
    const int extraCols = 2;
    const float extraColWidth = contentNode->getContentSize().width / extraCols;
    
    for (size_t i = 0; i < extras.size(); ++i) {
        int col = i % extraCols;
        int row = i / extraCols;
        float x = 20.0f + col * extraColWidth;
        float y = currentY - row * 25.0f;
        
        auto label = CCLabelBMFont::create(extras[i].second.c_str(), "modernMain.fnt"_spr);
        label->setPosition({x, y});
        label->setAnchorPoint({0.0f, 0.5f});
        label->setScale(0.35f);
        label->setOpacity(180);
        labels->addChild(label);
        
        auto toggler = createToggler(extras[i].first, {x + extraColWidth - 40.0f, y});
        toggler->setScale(0.65f);
        menu->addChild(toggler);
    }
    
    currentY -= 65.0f;
    
    // p2 Distance slider
    auto p2DistLabel = CCLabelBMFont::create("Col2 Distance", "modernMain.fnt"_spr);
    p2DistLabel->setPosition({contentMidX, currentY});
    p2DistLabel->setScale(0.38f);
    p2DistLabel->setOpacity(180);
    labels->addChild(p2DistLabel);
    
    auto p2DistSlider = createSlider("p2-distance", {contentMidX, currentY - 15.0f}, 200.0f);
    menu->addChild(p2DistSlider);
    
    float p2Dist = Mod::get()->getSettingValue<double>("p2-distance");
    auto p2DistValue = CCLabelBMFont::create(std::to_string(p2Dist).substr(0,4).c_str(), "modernMain.fnt"_spr);
    p2DistValue->setPosition({midX, currentY - 35.0f});
    p2DistValue->setScale(0.3f);
    p2DistValue->setOpacity(140);
    labels->addChild(p2DistValue);
    m_valueLabels[p2DistSlider->getTag()] = p2DistValue;

    // bottom navigation
    auto navMenu = CCMenu::create();
    navMenu->setContentSize({size.width, 35.0f});
    navMenu->setPosition({midX, 17.5f});
    navMenu->setAnchorPoint({0.5f, 0.5f});
    navMenu->setLayout(
        RowLayout::create()
            ->setGap(8.0f)
            ->setAxisAlignment(AxisAlignment::Center)
    );
    m_contentArea->addChild(navMenu);
    
    auto mainBtn = createNavButton("Main");
    navMenu->addChild(mainBtn);
    
    auto extrasBtn = createNavButton("Extras");
    navMenu->addChild(extrasBtn);
    
    navMenu->updateLayout();
}

void ModernSettingsPopup::buildColorsContent() {
    auto size = m_contentArea->getContentSize();
    const float midX = size.width / 2.0f;
    
    // title
    auto title = CCLabelBMFont::create("Icon Colors", "modernBold.fnt"_spr);
    title->setPosition({midX, size.height - 20.0f});
    title->setScale(0.6f);
    m_contentArea->addChild(title);
    
    // separator
    auto separator = CCSprite::createWithSpriteFrameName("floorLine_001.png");
    separator->setPosition({midX, size.height - 35.0f});
    separator->setScaleX(size.width / separator->getContentSize().width * 0.85f);
    separator->setOpacity(70);
    m_contentArea->addChild(separator);
    
    auto mainContainer = CCNode::create();
    mainContainer->setContentSize({280.0f, 240.0f});
    mainContainer->setPosition({midX, 50.0f});
    mainContainer->setAnchorPoint({0.5f, 0.0f});
    mainContainer->setScale(0.88f);
    m_contentArea->addChild(mainContainer);
    
    auto menu = CCMenu::create();
    menu->setContentSize(mainContainer->getContentSize());
    menu->setPosition({0, 0});
    mainContainer->addChild(menu);
    
    auto labels = CCNode::create();
    labels->setContentSize(mainContainer->getContentSize());
    labels->setPosition({0, 0});
    mainContainer->addChild(labels);
    
    const float containerMidX = mainContainer->getContentSize().width / 2.0f;
    const float containerMidY = mainContainer->getContentSize().height / 2.0f;
    
    auto enableLabel = CCLabelBMFont::create("Enable", "modernMain.fnt"_spr);
    enableLabel->setPosition({containerMidX, mainContainer->getContentSize().height - 15.0f});
    enableLabel->setScale(0.5f);
    enableLabel->setOpacity(200);
    labels->addChild(enableLabel);
    
    auto enableToggler = createToggler("enable-customcolors", {containerMidX, mainContainer->getContentSize().height - 30.0f});
    enableToggler->setScale(0.75f);
    menu->addChild(enableToggler);
    
    // colums
    const float colOffset = 65.0f;
    const float startY = mainContainer->getContentSize().height - 60.0f;
    const float rowGap = 25.0f;
    
    std::vector<std::pair<std::string, std::string>> p1Colors = {
        {"p1-primary", "Col 1"}, {"p1-secondary", "Col 2"}, {"p1-glow", "Glow"},
        {"p1-wave", "Wave"}, {"p1-trail", "Trail"}, {"p1-ghost-trail", "Ghost"},
        {"p1-dashfire", "Dash"}, {"p1-ufodome", "Dome"}
    };
    
    std::vector<std::pair<std::string, std::string>> p2Colors = {
        {"p2-primary", "Col 1"}, {"p2-secondary", "Col 2"}, {"p2-glow", "Glow"},
        {"p2-wave", "Wave"}, {"p2-trail", "Trail"}, {"p2-ghost-trail", "Ghost"},
        {"p2-dashfire", "Dash"}, {"p2-ufodome", "Dome"}
    };
    
    // headers
    auto p1Header = CCLabelBMFont::create("Player 1", "modernBold.fnt"_spr);
    p1Header->setPosition({containerMidX - colOffset, startY});
    p1Header->setScale(0.45f);
    p1Header->setOpacity(200);
    labels->addChild(p1Header);
    
    auto p2Header = CCLabelBMFont::create("Player 2", "modernBold.fnt"_spr);
    p2Header->setPosition({containerMidX + colOffset, startY});
    p2Header->setScale(0.45f);
    p2Header->setOpacity(200);
    labels->addChild(p2Header);
    
    for (size_t i = 0; i < p1Colors.size(); ++i) {
        float y = startY - 25.0f - (float)i * rowGap;
        
        // P1
        auto p1Label = CCLabelBMFont::create(p1Colors[i].second.c_str(), "modernMain.fnt"_spr);
        p1Label->setPosition({containerMidX - colOffset - 30.0f, y});
        p1Label->setAnchorPoint({1.0f, 0.5f});
        p1Label->setScale(0.4f);
        p1Label->setOpacity(170);
        labels->addChild(p1Label);
        
        auto p1ColorBtn = createColorPickerButton(p1Colors[i].first, {containerMidX - colOffset - 8.0f, y}, false);
        menu->addChild(p1ColorBtn);
        
        // P2
        auto p2Label = CCLabelBMFont::create(p2Colors[i].second.c_str(), "modernMain.fnt"_spr);
        p2Label->setPosition({containerMidX + colOffset - 30.0f, y});
        p2Label->setAnchorPoint({1.0f, 0.5f});
        p2Label->setScale(0.4f);
        p2Label->setOpacity(170);
        labels->addChild(p2Label);
        
        auto p2ColorBtn = createColorPickerButton(p2Colors[i].first, {containerMidX + colOffset - 8.0f, y}, false);
        menu->addChild(p2ColorBtn);
        
        // togglers
        if (i >= 3) {
            static const std::array<std::string, 5> mapTog = {"tint-wave", "tint-trail", "tint-ghost-trail", "tint-dashfire", "tint-ufodome"};
            auto settingId = mapTog[i - 3];
            
            auto t1 = createToggler((settingId + "-p1").c_str(), {containerMidX - colOffset + 15.0f, y});
            t1->setScale(0.6f);
            menu->addChild(t1);
            
            auto t2 = createToggler((settingId + "-p2").c_str(), {containerMidX + colOffset + 15.0f, y});
            t2->setScale(0.6f);
            menu->addChild(t2);
        }
    }
}

void ModernSettingsPopup::buildParticlesContent() {
    auto size = m_contentArea->getContentSize();
    
    // title
    auto title = CCLabelBMFont::create("Particles", "modernBold.fnt"_spr);
    title->setPosition({size.width / 2.0f, size.height - 30.0f});
    title->setScale(0.8f);
    m_contentArea->addChild(title);
    
    // separator
    auto separator = CCSprite::createWithSpriteFrameName("modernSeparatorHorizontal.png"_spr);
    separator->setPosition({size.width / 2.0f, size.height - 50.0f});
    separator->setScaleX(size.width / separator->getContentSize().width * 0.95f);
    separator->setOpacity(80);
    m_contentArea->addChild(separator);
    
    // main content
    auto contentNode = CCNode::create();
    contentNode->setContentSize({size.width, size.height - 70.0f});
    contentNode->setPosition({0, 0});
    m_contentArea->addChild(contentNode);
    
    auto menu = CCMenu::create();
    menu->setContentSize(contentNode->getContentSize());
    menu->setPosition({0, 0});
    contentNode->addChild(menu);
    
    auto labels = CCNode::create();
    labels->setContentSize(contentNode->getContentSize());
    labels->setPosition({0, 0});
    contentNode->addChild(labels);
    
    float currentY = contentNode->getContentSize().height - 20.0f;
    const float midX = size.width / 2.0f;
    const float colOffset = 135.0f;
    const float colBtnGap = 18.0f;
    
    // player labels
    auto p1Header = CCLabelBMFont::create("Player 1", "modernBold.fnt"_spr);
    p1Header->setPosition({midX - colOffset, currentY});
    p1Header->setScale(0.5f);
    p1Header->setOpacity(220);
    labels->addChild(p1Header);
    
    auto p2Header = CCLabelBMFont::create("Player 2", "modernBold.fnt"_spr);
    p2Header->setPosition({midX + colOffset, currentY});
    p2Header->setScale(0.5f);
    p2Header->setOpacity(220);
    labels->addChild(p2Header);
    
    currentY -= 45.0f;
    
    // main section
    auto mainLabel1 = CCLabelBMFont::create("Main", "modernMain.fnt"_spr);
    mainLabel1->setPosition({midX - colOffset, currentY});
    mainLabel1->setScale(0.45f);
    mainLabel1->setOpacity(200);
    labels->addChild(mainLabel1);
    
    auto mainT1 = createToggler("tint-mainparticles-p1", {midX - colOffset + 35.0f, currentY});
    mainT1->setScale(0.7f);
    menu->addChild(mainT1);
    
    auto mainLabel2 = CCLabelBMFont::create("Main", "modernMain.fnt"_spr);
    mainLabel2->setPosition({midX + colOffset, currentY});
    mainLabel2->setScale(0.45f);
    mainLabel2->setOpacity(200);
    labels->addChild(mainLabel2);
    
    auto mainT2 = createToggler("tint-mainparticles-p2", {midX + colOffset + 35.0f, currentY});
    mainT2->setScale(0.7f);
    menu->addChild(mainT2);
    
    currentY -= 35.0f;
    
    // pickers
    auto p1MainStart = createColorPickerButton("p1-main-particles-start", {midX - colOffset - colBtnGap, currentY}, true);
    menu->addChild(p1MainStart);
    
    auto p1MainEnd = createColorPickerButton("p1-main-particles-end", {midX - colOffset + colBtnGap, currentY}, true);
    menu->addChild(p1MainEnd);
    
    auto p2MainStart = createColorPickerButton("p2-main-particles-start", {midX + colOffset - colBtnGap, currentY}, true);
    menu->addChild(p2MainStart);
    
    auto p2MainEnd = createColorPickerButton("p2-main-particles-end", {midX + colOffset + colBtnGap, currentY}, true);
    menu->addChild(p2MainEnd);
    
    // indicator labels
    auto s1 = CCLabelBMFont::create("S", "modernMain.fnt"_spr);
    s1->setPosition({midX - colOffset - colBtnGap, currentY});
    s1->setScale(0.45f);
    s1->setOpacity(50);
    labels->addChild(s1);
    
    auto e1 = CCLabelBMFont::create("E", "modernMain.fnt"_spr);
    e1->setPosition({midX - colOffset + colBtnGap, currentY});
    e1->setScale(0.45f);
    e1->setOpacity(50);
    labels->addChild(e1);
    
    auto s2 = CCLabelBMFont::create("S", "modernMain.fnt"_spr);
    s2->setPosition({midX + colOffset - colBtnGap, currentY});
    s2->setScale(0.45f);
    s2->setOpacity(50);
    labels->addChild(s2);
    
    auto e2 = CCLabelBMFont::create("E", "modernMain.fnt"_spr);
    e2->setPosition({midX + colOffset + colBtnGap, currentY});
    e2->setScale(0.45f);
    e2->setOpacity(50);
    labels->addChild(e2);
    
    currentY -= 55.0f;
    
    // UFO click section
    auto ufoLabel1 = CCLabelBMFont::create("UFO Click", "modernMain.fnt"_spr);
    ufoLabel1->setPosition({midX - colOffset, currentY});
    ufoLabel1->setScale(0.45f);
    ufoLabel1->setOpacity(200);
    labels->addChild(ufoLabel1);
    
    auto ufoT1 = createToggler("tint-ufo-click-particles-p1", {midX - colOffset + 50.0f, currentY});
    ufoT1->setScale(0.7f);
    menu->addChild(ufoT1);
    
    auto ufoLabel2 = CCLabelBMFont::create("UFO Click", "modernMain.fnt"_spr);
    ufoLabel2->setPosition({midX + colOffset, currentY});
    ufoLabel2->setScale(0.45f);
    ufoLabel2->setOpacity(200);
    labels->addChild(ufoLabel2);
    
    auto ufoT2 = createToggler("tint-ufo-click-particles-p2", {midX + colOffset + 50.0f, currentY});
    ufoT2->setScale(0.7f);
    menu->addChild(ufoT2);
    
    currentY -= 35.0f;
    
    // UFO color buttons
    auto p1UfoStart = createColorPickerButton("p1-ufo-click-particles-start", {midX - colOffset - colBtnGap, currentY}, true);
    menu->addChild(p1UfoStart);
    
    auto p1UfoEnd = createColorPickerButton("p1-ufo-click-particles-end", {midX - colOffset + colBtnGap, currentY}, true);
    menu->addChild(p1UfoEnd);
    
    auto p2UfoStart = createColorPickerButton("p2-ufo-click-particles-start", {midX + colOffset - colBtnGap, currentY}, true);
    menu->addChild(p2UfoStart);
    
    auto p2UfoEnd = createColorPickerButton("p2-ufo-click-particles-end", {midX + colOffset + colBtnGap, currentY}, true);
    menu->addChild(p2UfoEnd);
    
    // indicator labels
    auto s3 = CCLabelBMFont::create("S", "modernMain.fnt"_spr);
    s3->setPosition({midX - colOffset - colBtnGap, currentY});
    s3->setScale(0.45f);
    s3->setOpacity(50);
    labels->addChild(s3);
    
    auto e3 = CCLabelBMFont::create("E", "modernMain.fnt"_spr);
    e3->setPosition({midX - colOffset + colBtnGap, currentY});
    e3->setScale(0.45f);
    e3->setOpacity(50);
    labels->addChild(e3);
    
    auto s4 = CCLabelBMFont::create("S", "modernMain.fnt"_spr);
    s4->setPosition({midX + colOffset - colBtnGap, currentY});
    s4->setScale(0.45f);
    s4->setOpacity(50);
    labels->addChild(s4);
    
    auto e4 = CCLabelBMFont::create("E", "modernMain.fnt"_spr);
    e4->setPosition({midX + colOffset + colBtnGap, currentY});
    e4->setScale(0.45f);
    e4->setOpacity(50);
    labels->addChild(e4);
}

CCMenuItemSpriteExtra* ModernSettingsPopup::createNavButton(const std::string& labelText) {
    const float buttonWidth = 70.0f;
    const float buttonHeight = 28.0f;
    
    // btn bg
    auto buttonBG = CCScale9Sprite::create("modernButton01.png"_spr);
    buttonBG->setContentSize({buttonWidth, buttonHeight});
    
    // label
    auto label = CCLabelBMFont::create(labelText.c_str(), "modernMain.fnt"_spr);
    label->setPosition({buttonWidth / 2.0f, buttonHeight / 2.0f});
    label->setScale(0.4f);
    label->setOpacity(200);
    buttonBG->addChild(label);
    
    // the button (real)
    auto button = CCMenuItemSpriteExtra::create(
        buttonBG,
        this,
        menu_selector(ModernSettingsPopup::onNavButton)
    );
    
    return button;
}

void ModernSettingsPopup::onNavButton(CCObject* sender) {
    geode::log::info("Nav button clicked");
    // still not sure abt this
}

CCNode* ModernSettingsPopup::createSectionTitle(const std::string& title) {
    auto node = CCNode::create();
    node->setContentSize({400.0f, 25.0f});
    
    auto label = CCLabelBMFont::create(title.c_str(), "modernBold.fnt"_spr);
    label->setPosition({200.0f, 12.5f});
    label->setScale(0.55f);
    label->setOpacity(220);
    node->addChild(label);
    
    return node;
}

CCMenuItemToggler* ModernSettingsPopup::createToggler(std::string settingId, CCPoint position) {
    auto offSprite = CCSprite::createWithSpriteFrameName("modernToggleOff.png"_spr);
    auto onSprite = CCSprite::createWithSpriteFrameName("modernToggleOn.png"_spr);
    
    auto toggler = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(ModernSettingsPopup::onToggle));
    toggler->setPosition(position);

    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    toggler->setTag(tag);
    this->m_tagToSetting[tag] = settingId;

    bool initial = Mod::get()->getSettingValue<bool>(settingId);
    toggler->toggle(initial);

    toggler->setUserObject(CCString::create(settingId));
    
    return toggler;
}

CCMenuItemSprite* ModernSettingsPopup::createRadioButton(std::string settingId, CCPoint position) {
    auto offSprite = CCSprite::createWithSpriteFrameName("modernRadioOff.png"_spr);
    auto onSprite = CCSprite::createWithSpriteFrameName("modernRadioOn.png"_spr);
    
    auto radio = CCMenuItemSprite::create(offSprite, onSprite, this, menu_selector(ModernSettingsPopup::onRadioButton));
    radio->setPosition(position);
    
    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    radio->setTag(tag);
    this->m_tagToSetting[tag] = settingId;
    
    bool initial = Mod::get()->getSettingValue<bool>(settingId);
    if (initial) {
        radio->selected();
    } else {
        radio->unselected();
    }
    
    radio->setUserObject(CCString::create(settingId));
    
    return radio;
}

Slider* ModernSettingsPopup::createSlider(std::string settingId, CCPoint position, float width) {
    auto slider = Slider::create(this, menu_selector(ModernSettingsPopup::onSlider), 0.75f);
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
    
    // set initial values
    if (settingId == "random-delay") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue((value - 0.1f) / (70.0f - 0.1f));
    } else if (settingId == "rgb-speed") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue(value / 10.0f);
    } else if (settingId == "rgb-saturation" || settingId == "rgb-saturation2") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue(value);
    } else if (settingId == "rgb-brightness1" || settingId == "rgb-brightness2") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue(value);
    } else if (settingId == "p2-distance") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue(value);
    }
    
    return slider;
}

CCMenuItemSpriteExtra* ModernSettingsPopup::createColorPickerButton(const std::string& settingId, CCPoint position, bool is4B) {
    auto colorSprite = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    
    if (is4B) {
        auto color = Mod::get()->getSettingValue<cocos2d::ccColor4B>(settingId);
        colorSprite->setColor({color.r, color.g, color.b});
    } else {
        auto color = Mod::get()->getSettingValue<cocos2d::ccColor3B>(settingId);
        colorSprite->setColor(color);
    }
    
    colorSprite->setScale(0.65f);
    
    auto button = CCMenuItemSpriteExtra::create(
        colorSprite,
        this,
        menu_selector(ModernSettingsPopup::onColorPicker)
    );
    
    button->setPosition(position);
    button->setUserObject(CCString::create(settingId));
    
    int tag = static_cast<int>(std::hash<std::string>{}(settingId));
    if (is4B) tag |= (1 << 31);
    button->setTag(tag);
    
    return button;
}

void ModernSettingsPopup::onToggle(CCObject* sender) {
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
        if (auto cs = typeinfo_cast<CCString*>(obj)) {
            settingIdStr = std::string(cs->getCString());
        } else {
            return;
        }
    }
    
    bool newValue = !toggler->isToggled();
    Mod::get()->setSettingValue<bool>(settingIdStr, newValue);
    
    if (settingIdStr == "cond-ic-randomizer" && newValue) {
        Mod::get()->setSettingValue<bool>("live-ic-randomizer", false);
    } else if (settingIdStr == "live-ic-randomizer" && newValue) {
        Mod::get()->setSettingValue<bool>("cond-ic-randomizer", false);
    }

    geode::log::debug("ModernUI: toggled {} -> {}", settingIdStr, newValue ? "true" : "false");
}

void ModernSettingsPopup::onRadioButton(CCObject* sender) {
    auto radio = static_cast<CCMenuItemSprite*>(sender);
    if (!radio) return;
    
    std::string settingIdStr;
    int tag = radio->getTag();
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) {
        settingIdStr = it->second;
    } else {
        auto obj = radio->getUserObject();
        if (!obj) return;
        if (auto cs = typeinfo_cast<CCString*>(obj)) {
            settingIdStr = std::string(cs->getCString());
        } else {
            return;
        }
    }
    
    bool currentValue = Mod::get()->getSettingValue<bool>(settingIdStr);
    bool newValue = !currentValue;
    Mod::get()->setSettingValue<bool>(settingIdStr, newValue);
    
    if (newValue) {
        radio->selected();
    } else {
        radio->unselected();
    }
    
    if (settingIdStr == "rgb-col1" && newValue) {
        Mod::get()->setSettingValue<bool>("rgb-col2", false);
    } else if (settingIdStr == "rgb-col2" && newValue) {
        Mod::get()->setSettingValue<bool>("rgb-col1", false);
    }
    
    geode::log::debug("ModernUI: radio {} -> {}", settingIdStr, newValue ? "on" : "off");
}

void ModernSettingsPopup::onSlider(CCObject* sender) {
    auto slider = typeinfo_cast<Slider*>(sender);
    CCObject* userObj = nullptr;
    int tag = 0;
    float value = 0.0f;

    if (slider) {
        userObj = slider->getUserObject();
        tag = slider->getTag();
        value = slider->getValue();
    } else {
        auto thumb = typeinfo_cast<SliderThumb*>(sender);
        if (thumb) {
            userObj = thumb->getUserObject();
            tag = thumb->getTag();
            value = thumb->getValue();
        } else {
            return;
        }
    }

    std::string settingIdStr;
    auto it = this->m_tagToSetting.find(tag);
    if (it != this->m_tagToSetting.end()) {
        settingIdStr = it->second;
    } else if (userObj) {
        if (auto cs = typeinfo_cast<CCString*>(userObj)) {
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
        if (this->m_valueLabels.count(tag)) {
            std::string t = std::to_string(mapped).substr(0,4) + "s";
            this->m_valueLabels[tag]->setString(t.c_str());
        }
    } else if (settingIdStr == "rgb-speed") {
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
    } else if (settingIdStr == "p2-distance") {
        Mod::get()->setSettingValue<double>(settingIdStr, value);
        if (this->m_valueLabels.count(tag)) {
            std::string t = std::to_string(value).substr(0,4);
            this->m_valueLabels[tag]->setString(t.c_str());
        }
    }
}

void ModernSettingsPopup::onColorPicker(CCObject* sender) {
    auto menuItem = static_cast<CCMenuItemSpriteExtra*>(sender);
    if (!menuItem) return;
    
    auto userObj = menuItem->getUserObject();
    if (!userObj) return;
    
    auto settingIdStr = typeinfo_cast<CCString*>(userObj);
    if (!settingIdStr) return;
    
    m_currentColorSettingId = std::string(settingIdStr->getCString());
    m_currentButtonSprite = typeinfo_cast<CCSprite*>(menuItem->getNormalImage());
    
    int tag = menuItem->getTag();
    m_isColor4B = (tag & (1 << 31)) != 0;
    
    cocos2d::ccColor4B currentColor;
    if (m_isColor4B) {
        currentColor = Mod::get()->getSettingValue<cocos2d::ccColor4B>(m_currentColorSettingId);
    } else {
        auto color3B = Mod::get()->getSettingValue<cocos2d::ccColor3B>(m_currentColorSettingId);
        currentColor = {color3B.r, color3B.g, color3B.b, 255};
    }
    
    auto colorPopup = geode::ColorPickPopup::create(currentColor);
    colorPopup->setDelegate(this);
    colorPopup->show();
}

void ModernSettingsPopup::updateColor(cocos2d::ccColor4B const& color) {
    if (m_currentColorSettingId.empty()) return;
    
    if (m_isColor4B) {
        Mod::get()->setSettingValue<cocos2d::ccColor4B>(m_currentColorSettingId, color);
    } else {
        cocos2d::ccColor3B color3B = {color.r, color.g, color.b};
        Mod::get()->setSettingValue<cocos2d::ccColor3B>(m_currentColorSettingId, color3B);
    }
    
    if (m_currentButtonSprite) {
        m_currentButtonSprite->setColor({color.r, color.g, color.b});
    }
    
    geode::log::debug("ModernUI: Updated {} to ({}, {}, {}, {})", 
        m_currentColorSettingId, color.r, color.g, color.b, color.a);
}