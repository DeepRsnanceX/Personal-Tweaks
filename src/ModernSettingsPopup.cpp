#include "ModernSettingsPopup.hpp"
#include "ShaderCache.h"
#include <Geode/ui/GeodeUI.hpp>

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
    auto size = this->m_mainLayer->getContentSize();
    this->setTitle("SEXO");
    this->m_noElasticity = true;
    this->setOpacity(0);
    
    if (this->m_closeBtn) {
        this->m_closeBtn->setSprite(CCSprite::createWithSpriteFrameName("closeBtn.png"_spr));
        this->m_closeBtn->updateSprite();

        this->m_closeBtn->setPosition({size.width - 10.f, size.height - 10.f});
    }

    if (this->m_title) this->m_title->setVisible(false);

    m_blurLayer = BlurLayer::create();
    m_blurLayer->setID("modern-popup-blur");
    m_blurLayer->setZOrder(-100);
    m_blurLayer->setOpacity(0);
    this->addChild(m_blurLayer);
    
    m_blurLayer->runAction(CCFadeTo::create(0.2f, 200));
    
    setupSidebar();
    setupContentArea();
    switchCategory(SettingsCategory::Randomizer);

    // open settings btn
    auto optSpr = CCSprite::createWithSpriteFrameName("settingsBtn.png"_spr);
    auto optBtn = CCMenuItemSpriteExtra::create(optSpr, this, menu_selector(ModernSettingsPopup::onModSettings));
    auto optMenu = CCMenu::create();

    optMenu->setContentSize(optSpr->getContentSize());
    optBtn->setPosition({optMenu->getContentSize().width / 2.0f, optMenu->getContentSize().height / 2.0f});
    
    optMenu->addChild(optBtn);
    optMenu->setScale(0.8f);
    optMenu->setPosition({10.f, 10.f});
    this->m_mainLayer->addChild(optMenu);
    
    return true;
}

void ModernSettingsPopup::setupSidebar() {
    auto size = this->m_mainLayer->getContentSize();
    const float sidebarWidth = 150.0f;
    
    m_sidebar = CCNode::create();
    m_sidebar->setContentSize({sidebarWidth, size.height});
    m_sidebar->setAnchorPoint({0.0f, 0.5f});
    m_sidebar->setPosition({-10.f, size.height / 2.f});
    m_sidebar->setID("sidebar");
    this->m_mainLayer->addChild(m_sidebar);
    
    const float sidebarMidX = sidebarWidth / 2.0f;
    const float sidebarMidY = size.height / 2.0f;
    
    auto logo = CCSprite::createWithSpriteFrameName("modLogo.png"_spr);
    logo->setScale(0.9f);
    logo->setPosition({sidebarMidX, sidebarMidY + 105.0f});
    m_sidebar->addChild(logo);
    
    // sidebar separator
    auto separator = CCSprite::createWithSpriteFrameName("floorLine_02_001.png");
    separator->setScale(0.25f);
    separator->setPosition({sidebarMidX, sidebarMidY + 80.0f});
    separator->setOpacity(90);
    m_sidebar->addChild(separator);
    
    // contents
    struct ButtonData {
        CCNode* normalContent;
        CCNode* selectedContent;
        std::string iconName;
        std::string labelText;
        SettingsCategory category;
    };
    
    std::vector<ButtonData> buttonDataList;
    std::vector<std::pair<std::string, std::pair<std::string, SettingsCategory>>> buttonConfigs = {
        {"randomizerIcon", {"Randomizer", SettingsCategory::Randomizer}},
        {"rgbIcon", {"RGB Icons", SettingsCategory::RGB}},
        {"colorsIcon", {"Colors", SettingsCategory::Colors}},
        {"particlesIcon", {"Particles", SettingsCategory::Particles}},
        {"legacyIcon", {"Legacy", SettingsCategory::Legacy}}
    };
    
    CCSize maxContentSize = {0, 0};
    
    for (const auto& [iconName, pair] : buttonConfigs) {
        const auto& [labelText, category] = pair;
        
        // normal
        auto normalContent = CCNode::create();
        normalContent->setAnchorPoint({0.5f, 0.5f});
        normalContent->setLayout(
            RowLayout::create()
                ->setGap(8.0f)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setCrossAxisAlignment(AxisAlignment::Center)
                ->setAutoScale(false)
                ->setAutoGrowAxis(true)
        );
        
        auto normalIcon = CCSprite::createWithSpriteFrameName(fmt::format("{}.png"_spr, iconName).c_str());
        normalIcon->setScale(0.7f);
        normalContent->addChild(normalIcon);
        
        auto normalLabel = CCLabelBMFont::create(labelText.c_str(), "modernBold.fnt"_spr);
        normalLabel->setScale(0.35f);
        normalLabel->setAnchorPoint({0.5f, 0.45f});
        normalContent->addChild(normalLabel);
        
        normalContent->updateLayout();
        
        // selected
        auto selectedContent = CCNode::create();
        selectedContent->setAnchorPoint({0.5f, 0.5f});
        selectedContent->setLayout(
            RowLayout::create()
                ->setGap(8.0f)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setCrossAxisAlignment(AxisAlignment::Center)
                ->setAutoScale(false)
                ->setAutoGrowAxis(true)
        );
        
        auto selectedIcon = CCSprite::createWithSpriteFrameName(fmt::format("{}Selected.png"_spr, iconName).c_str());
        selectedIcon->setScale(0.7f);
        selectedContent->addChild(selectedIcon);
        
        auto selectedLabel = CCLabelBMFont::create(labelText.c_str(), "modernBold.fnt"_spr);
        selectedLabel->setScale(0.35f);
        selectedLabel->setAnchorPoint({0.5f, 0.45f});
        selectedContent->addChild(selectedLabel);
        
        selectedContent->updateLayout();
        
        // sizes
        auto nSize = normalContent->getContentSize();
        auto sSize = selectedContent->getContentSize();
        maxContentSize.width = std::max({maxContentSize.width, nSize.width, sSize.width});
        maxContentSize.height = std::max({maxContentSize.height, nSize.height, sSize.height});
        
        buttonDataList.push_back({normalContent, selectedContent, iconName, labelText, category});
    }
    
    const float buttonWidth = maxContentSize.width + 22.0f;
    const float buttonHeight = maxContentSize.height + 10.0f;
    
    // sidebar menu
    auto menu = CCMenu::create();
    menu->setContentSize({sidebarWidth, 0.f});
    menu->setAnchorPoint({0.5f, 1.f});
    menu->setPosition({sidebarMidX, sidebarMidY + 65.0f});
    menu->setID("sidebar-menu");
    menu->setLayout(
        ColumnLayout::create()
            ->setGap(10.0f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setAutoGrowAxis(true)
            ->setAxisReverse(true)
    );
    m_sidebar->addChild(menu);
    
    for (auto& data : buttonDataList) {
        auto normalBG = CCScale9Sprite::create("modernButtonUnselected01.png"_spr);
        normalBG->setContentSize({buttonWidth, buttonHeight});
        
        auto selectedBG = CCScale9Sprite::create("modernButton01.png"_spr);
        selectedBG->setContentSize({buttonWidth, buttonHeight});
        selectedBG->setOpacity(255);
        
        data.normalContent->setAnchorPoint({0.f, 0.5f});
        data.normalContent->setPosition({8.f, buttonHeight / 2.0f});
        data.selectedContent->setAnchorPoint({0.f, 0.5f});
        data.selectedContent->setPosition({8.f, buttonHeight / 2.0f});
        
        normalBG->addChild(data.normalContent);
        selectedBG->addChild(data.selectedContent);
        
        auto button = CCMenuItemSprite::create(
            static_cast<CCNode*>(normalBG),
            static_cast<CCNode*>(selectedBG),
            this,
            menu_selector(ModernSettingsPopup::onCategoryButton)
        );
        button->setTag(static_cast<int>(data.category));
        button->setID((data.iconName + "-btn").c_str());
        button->setScale(0.8f);
        
        menu->addChild(button);
        m_categoryButtons[data.category] = button;
    }
    
    menu->updateLayout();

    // separator (between sidebar and content)
    m_areaSeparator = CCSprite::createWithSpriteFrameName("floorLine_02_001.png");
    m_areaSeparator->setPosition({size.width / 2.f - 120.f, size.height / 2.f});
    m_areaSeparator->setScale(0.6f);
    m_areaSeparator->setOpacity(75);
    m_areaSeparator->setRotation(90);
    m_areaSeparator->setID("area-separator");
    this->m_mainLayer->addChild(m_areaSeparator);
}

void ModernSettingsPopup::setupContentArea() {
    auto size = this->m_mainLayer->getContentSize();
    
    const float contentWidth = 365.0f;
    const float contentHeight = 260.0f;
    
    m_contentArea = CCNode::create();
    m_contentArea->setContentSize({contentWidth, contentHeight});
    m_contentArea->setAnchorPoint({0.5f, 0.5f});
    m_contentArea->setPosition({size.width / 2.0f + 70.0f, size.height / 2.0f});
    m_contentArea->setID("content-area");
    this->m_mainLayer->addChild(m_contentArea);
    
    m_titleLabel = CCLabelBMFont::create("Randomizer", "modernBold.fnt"_spr);
    m_titleLabel->setPosition({10.f, contentHeight - 20.0f});
    m_titleLabel->setScale(0.6f);
    m_titleLabel->setAnchorPoint({0.f, 0.5f});
    m_titleLabel->setAlignment(kCCTextAlignmentLeft);
    m_contentArea->addChild(m_titleLabel);
    
    m_separator = CCSprite::createWithSpriteFrameName("floorLine_02_001.png");
    m_separator->setPosition({contentWidth / 2.0f, contentHeight - 40.0f});
    m_separator->setScaleX(contentWidth / m_separator->getContentSize().width * 0.9f + 0.05f);
    m_separator->setScaleY(0.25f);
    m_separator->setOpacity(75);
    m_contentArea->addChild(m_separator);
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
    
    if (m_contentContainer) {
        m_contentContainer->removeFromParent();
        m_contentContainer = nullptr;
    }
    
    m_tagToSetting.clear();
    m_valueLabels.clear();
    
    switch (category) {
        case SettingsCategory::Randomizer:
            m_titleLabel->setString("Randomizer");
            buildRandomizerContent();
            break;
        case SettingsCategory::RGB:
            m_titleLabel->setString("RGB Icons");
            buildRGBContent();
            break;
        case SettingsCategory::Colors:
            m_titleLabel->setString("Icon Colors");
            buildColorsContent();
            break;
        case SettingsCategory::Particles:
            m_titleLabel->setString("Particles");
            buildParticlesContent();
            break;
        case SettingsCategory::Legacy:
            m_titleLabel->setString("Legacy");
            buildLegacyContent();
            break;
    }
}


void ModernSettingsPopup::buildRandomizerContent() {
    auto size = m_contentArea->getContentSize();
    
    m_contentContainer = CCNode::create();
    m_contentContainer->setContentSize({size.width - 30.0f, size.height - 50.0f});
    m_contentContainer->setPosition({15.0f, 10.0f});
    m_contentContainer->setAnchorPoint({0.0f, 0.0f});
    m_contentContainer->setLayout(
        RowLayout::create()
            ->setGap(15.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(false)
            ->setGrowCrossAxis(true)
            ->setCrossAxisLineAlignment(AxisAlignment::End)
    );
    m_contentArea->addChild(m_contentContainer);
    
    // === LEFT COLUMN ===
    auto leftColumn = CCNode::create();
    leftColumn->setContentSize({0, 0});
    leftColumn->setLayout(
        ColumnLayout::create()
            ->setGap(0.f)
            ->setAxisAlignment(AxisAlignment::Even)
            ->setAxisReverse(true)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
            ->setCrossAxisLineAlignment(AxisAlignment::Start)
    );
    
    // cond.
    auto condTitle = createSectionTitle("Conditional");
    leftColumn->addChild(condTitle);
    
    leftColumn->addChild(createLabelToggleRow("Enable", "cond-ic-randomizer", 140.0f, 0.38f, 0.6f));
    leftColumn->addChild(createLabelToggleRow("Every Gamemode", "cond-ongamemodechange", 140.0f, 0.32f, 0.55f));
    leftColumn->addChild(createLabelToggleRow("On Death", "cond-ondeath", 140.0f, 0.32f, 0.55f));
    leftColumn->addChild(createLabelToggleRow("On Click", "cond-onclick", 140.0f, 0.32f, 0.55f));
    
    auto spacer1 = CCNode::create();
    spacer1->setContentSize({1, 8});
    leftColumn->addChild(spacer1);
    
    // live
    auto liveTitle = createSectionTitle("Live");
    leftColumn->addChild(liveTitle);
    
    leftColumn->addChild(createLabelToggleRow("Enable", "live-ic-randomizer", 140.0f, 0.38f, 0.6f));
    leftColumn->addChild(createLabelSliderRow("Delay", "random-delay", 140.0f, 0.32f));
    
    leftColumn->updateLayout();
    m_contentContainer->addChild(leftColumn);
    
    // === RIGHT COLUMN ===
    auto rightColumn = CCNode::create();
    rightColumn->setContentSize({0, 0});
    rightColumn->setLayout(
        ColumnLayout::create()
            ->setGap(-6.0f)
            ->setAxisAlignment(AxisAlignment::Even)
            ->setAxisReverse(true)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
            ->setCrossAxisLineAlignment(AxisAlignment::Start)
    );
    
    // modes
    auto modesTitle = createSectionTitle("Modes");
    rightColumn->addChild(modesTitle);
    
    std::vector<std::pair<std::string, std::string>> iconTypes = {
        {"randomize-cube", "Cube"}, {"randomize-ship", "Ship"},
        {"randomize-ball", "Ball"}, {"randomize-ufo", "UFO"},
        {"randomize-wave", "Wave"}, {"randomize-robot", "Robot"},
        {"randomize-spider", "Spider"}, {"randomize-swing", "Swing"}
    };
    
    for (const auto& [settingId, name] : iconTypes) {
        rightColumn->addChild(createLabelToggleRow(name, settingId, 140.0f, 0.32f, 0.55f));
    }
    
    auto spacer2 = CCNode::create();
    spacer2->setContentSize({1, 8});
    rightColumn->addChild(spacer2);
    
    std::vector<std::pair<std::string, std::string>> colorTypes = {
        {"randomize-color1", "Color 1"},
        {"randomize-color2", "Color 2"},
        {"randomize-glowcolor", "Glow"}
    };
    
    for (const auto& [settingId, name] : colorTypes) {
        rightColumn->addChild(createLabelToggleRow(name, settingId, 140.0f, 0.32f, 0.55f));
    }
    
    rightColumn->updateLayout();
    m_contentContainer->addChild(rightColumn);
    
    m_contentContainer->updateLayout();
}

void ModernSettingsPopup::buildRGBContent() {
    auto size = m_contentArea->getContentSize();
    
    m_contentContainer = CCNode::create();
    m_contentContainer->setContentSize({size.width - 30.0f, size.height - 50.0f});
    m_contentContainer->setPosition({15.0f, 10.0f});
    m_contentContainer->setAnchorPoint({0.0f, 0.0f});
    m_contentContainer->setLayout(
        RowLayout::create()
            ->setGap(20.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(false)
            ->setGrowCrossAxis(true)
    );
    m_contentArea->addChild(m_contentContainer);
    
    // === LEFT COLUMN ===
    auto leftColumn = CCNode::create();
    leftColumn->setContentSize({0, 0});
    leftColumn->setLayout(
        ColumnLayout::create()
            ->setGap(6.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(true)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    leftColumn->addChild(createLabelSliderRow("Speed", "rgb-speed", 140.0f, 0.35f));
    
    auto spacer1 = CCNode::create();
    spacer1->setContentSize({1, 8});
    leftColumn->addChild(spacer1);
    
    // col 1 stuff
    leftColumn->addChild(createSectionTitleWithToggle("Color 1", "rgb-col1", 140.0f));
    leftColumn->addChild(createLabelSliderRow("Brightness", "rgb-brightness1", 140.0f, 0.32f));
    leftColumn->addChild(createLabelSliderRow("Saturation", "rgb-saturation", 140.0f, 0.32f));
    
    auto spacer2 = CCNode::create();
    spacer2->setContentSize({1, 8});
    leftColumn->addChild(spacer2);
    
    // col 2 stuff
    leftColumn->addChild(createSectionTitleWithToggle("Color 2", "rgb-col2", 140.0f));
    leftColumn->addChild(createLabelSliderRow("Brightness", "rgb-brightness2", 140.0f, 0.32f));
    leftColumn->addChild(createLabelSliderRow("Saturation", "rgb-saturation2", 140.0f, 0.32f));
    
    leftColumn->updateLayout();
    m_contentContainer->addChild(leftColumn);
    
    // === RIGHT COLUMN ===
    auto rightColumn = CCNode::create();
    rightColumn->setContentSize({0, 0});
    rightColumn->setLayout(
        ColumnLayout::create()
            ->setGap(6.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(true)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    rightColumn->addChild(createSectionTitle("Extras"));
    
    std::vector<std::pair<std::string, std::string>> extras = {
        {"rgb-wave", "RGB Wave"},
        {"rgb-trail", "RGB Trail"},
        {"rgb-dash", "RGB Dash"},
        {"better-immersion-mode", "Immersion"},
        {"ignore-p2", "Ignore P2"}
    };
    
    for (const auto& [settingId, name] : extras) {
        rightColumn->addChild(createLabelToggleRow(name, settingId, 140.0f, 0.32f, 0.55f));
    }
    
    auto spacer3 = CCNode::create();
    spacer3->setContentSize({1, 8});
    rightColumn->addChild(spacer3);
    
    rightColumn->addChild(createLabelSliderRow("Col2 Distance", "p2-distance", 140.0f, 0.32f));
    
    rightColumn->updateLayout();
    m_contentContainer->addChild(rightColumn);
    
    m_contentContainer->updateLayout();
}

void ModernSettingsPopup::buildColorsContent() {
    auto size = m_contentArea->getContentSize();
    
    m_contentContainer = CCNode::create();
    m_contentContainer->setContentSize({size.width - 30.0f, size.height - 50.0f});
    m_contentContainer->setPosition({15.0f, 10.0f});
    m_contentContainer->setAnchorPoint({0.0f, 0.0f});
    m_contentContainer->setLayout(
        ColumnLayout::create()
            ->setGap(10.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(true)
            ->setAutoScale(false)
            ->setGrowCrossAxis(true)
    );
    m_contentArea->addChild(m_contentContainer);
    
    m_contentContainer->addChild(createLabelToggleRow("Enable", "enable-customcolors", size.width - 30.0f, 0.38f, 0.65f));
    
    auto playersRow = CCNode::create();
    playersRow->setContentSize({0, 0});
    playersRow->setLayout(
        RowLayout::create()
            ->setGap(15.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    // === PLAYER 1 COLUMN ===
    auto p1Column = CCNode::create();
    p1Column->setContentSize({0, 0});
    p1Column->setLayout(
        ColumnLayout::create()
            ->setGap(5.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(true)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    auto p1Header = CCLabelBMFont::create("Player 1", "modernBold.fnt"_spr);
    p1Header->setScale(0.4f);
    p1Header->setOpacity(200);
    p1Column->addChild(p1Header);
    
    std::vector<std::pair<std::string, std::string>> p1Colors = {
        {"p1-primary", "Col 1"}, {"p1-secondary", "Col 2"}, {"p1-glow", "Glow"},
        {"p1-wave", "Wave"}, {"p1-trail", "Trail"}, {"p1-ghost-trail", "Ghost"},
        {"p1-dashfire", "Dash"}, {"p1-ufodome", "Dome"}
    };
    
    static const std::array<std::string, 5> toggleSettings = {
        "tint-wave", "tint-trail", "tint-ghost-trail", "tint-dashfire", "tint-ufodome"
    };
    
    for (size_t i = 0; i < p1Colors.size(); ++i) {
        bool hasToggle = i >= 3;
        p1Column->addChild(createLabelColorRow(
            p1Colors[i].second,
            p1Colors[i].first,
            70.0f,
            hasToggle ? (toggleSettings[i - 3] + "-p1") : "",
            0.32f
        ));
    }
    
    p1Column->updateLayout();
    playersRow->addChild(p1Column);
    
    // === PLAYER 2 COLUMN ===
    auto p2Column = CCNode::create();
    p2Column->setContentSize({0, 0});
    p2Column->setLayout(
        ColumnLayout::create()
            ->setGap(5.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(true)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    auto p2Header = CCLabelBMFont::create("Player 2", "modernBold.fnt"_spr);
    p2Header->setScale(0.4f);
    p2Header->setOpacity(200);
    p2Column->addChild(p2Header);
    
    std::vector<std::pair<std::string, std::string>> p2Colors = {
        {"p2-primary", "Col 1"}, {"p2-secondary", "Col 2"}, {"p2-glow", "Glow"},
        {"p2-wave", "Wave"}, {"p2-trail", "Trail"}, {"p2-ghost-trail", "Ghost"},
        {"p2-dashfire", "Dash"}, {"p2-ufodome", "Dome"}
    };
    
    for (size_t i = 0; i < p2Colors.size(); ++i) {
        bool hasToggle = i >= 3;
        p2Column->addChild(createLabelColorRow(
            p2Colors[i].second,
            p2Colors[i].first,
            70.0f,
            hasToggle ? (toggleSettings[i - 3] + "-p2") : "",
            0.32f
        ));
    }
    
    p2Column->updateLayout();
    playersRow->addChild(p2Column);
    
    playersRow->updateLayout();
    m_contentContainer->addChild(playersRow);
    
    m_contentContainer->updateLayout();
}

void ModernSettingsPopup::buildParticlesContent() {
    auto size = m_contentArea->getContentSize();
    
    m_contentContainer = CCNode::create();
    m_contentContainer->setContentSize({size.width - 30.0f, size.height - 50.0f});
    m_contentContainer->setPosition({15.0f, 10.0f});
    m_contentContainer->setAnchorPoint({0.0f, 0.0f});
    m_contentContainer->setLayout(
        RowLayout::create()
            ->setGap(25.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(false)
            ->setGrowCrossAxis(true)
    );
    m_contentArea->addChild(m_contentContainer);
    
    // === PLAYER 1 COLUMN ===
    auto p1Column = CCNode::create();
    p1Column->setContentSize({0, 0});
    p1Column->setLayout(
        ColumnLayout::create()
            ->setGap(6.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(true)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    auto p1Header = CCLabelBMFont::create("Player 1", "modernBold.fnt"_spr);
    p1Header->setScale(0.42f);
    p1Header->setOpacity(220);
    p1Column->addChild(p1Header);
    
    // main particles
    p1Column->addChild(createSectionTitle("Main Particles"));
    p1Column->addChild(createParticleToggleRow("Enable", "tint-mainparticles-p1", 140.0f));
    p1Column->addChild(createColorPairRow("p1-main-particles-start", "p1-main-particles-end", 140.0f));
    
    auto spacer1 = CCNode::create();
    spacer1->setContentSize({1, 10});
    p1Column->addChild(spacer1);
    
    // UFO click
    p1Column->addChild(createSectionTitle("UFO Click"));
    p1Column->addChild(createParticleToggleRow("Enable", "tint-ufo-click-particles-p1", 140.0f));
    p1Column->addChild(createColorPairRow("p1-ufo-click-particles-start", "p1-ufo-click-particles-end", 140.0f));
    
    p1Column->updateLayout();
    m_contentContainer->addChild(p1Column);
    
    // === PLAYER 2 COLUMN ===
    auto p2Column = CCNode::create();
    p2Column->setContentSize({0, 0});
    p2Column->setLayout(
        ColumnLayout::create()
            ->setGap(6.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(true)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    auto p2Header = CCLabelBMFont::create("Player 2", "modernBold.fnt"_spr);
    p2Header->setScale(0.42f);
    p2Header->setOpacity(220);
    p2Column->addChild(p2Header);
    
    // main particles
    p2Column->addChild(createSectionTitle("Main Particles"));
    p2Column->addChild(createParticleToggleRow("Enable", "tint-mainparticles-p2", 140.0f));
    p2Column->addChild(createColorPairRow("p2-main-particles-start", "p2-main-particles-end", 140.0f));
    
    auto spacer2 = CCNode::create();
    spacer2->setContentSize({1, 10});
    p2Column->addChild(spacer2);
    
    // UFO click
    p2Column->addChild(createSectionTitle("UFO Click"));
    p2Column->addChild(createParticleToggleRow("Enable", "tint-ufo-click-particles-p2", 140.0f));
    p2Column->addChild(createColorPairRow("p2-ufo-click-particles-start", "p2-ufo-click-particles-end", 140.0f));
    
    p2Column->updateLayout();
    m_contentContainer->addChild(p2Column);
    
    m_contentContainer->updateLayout();
}

void ModernSettingsPopup::buildLegacyContent() {
    auto size = m_contentArea->getContentSize();
    
    m_contentContainer = CCNode::create();
    m_contentContainer->setContentSize({size.width - 30.0f, size.height - 50.0f});
    m_contentContainer->setPosition({15.0f, 10.0f});
    m_contentContainer->setAnchorPoint({0.0f, 0.0f});
    m_contentContainer->setLayout(
        RowLayout::create()
            ->setGap(20.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(false)
            ->setGrowCrossAxis(true)
    );
    m_contentArea->addChild(m_contentContainer);
    
    // === LEFT COLUMN ===
    auto leftColumn = CCNode::create();
    leftColumn->setContentSize({0, 0});
    leftColumn->setLayout(
        ColumnLayout::create()
            ->setGap(6.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(true)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    leftColumn->addChild(createSectionTitle("Legacy Trail"));
    leftColumn->addChild(createLabelToggleRow("Enable", "use-legacytrail", 140.0f, 0.35f, 0.6f));
    leftColumn->addChild(createLabelSliderRow("Max Points", "max-points", 140.0f, 0.32f));
    leftColumn->addChild(createLabelSliderRow("Segment Tuner", "segment-tuner", 140.0f, 0.32f));
    leftColumn->addChild(createLabelSliderRow("Pos Skip Rate", "pos-skip-rate", 140.0f, 0.32f));
    
    leftColumn->updateLayout();
    m_contentContainer->addChild(leftColumn);
    
    // === RIGHT COLUMN ===
    auto rightColumn = CCNode::create();
    rightColumn->setContentSize({0, 0});
    rightColumn->setLayout(
        ColumnLayout::create()
            ->setGap(6.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(true)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    rightColumn->addChild(createSectionTitle("Legacy Pulse"));
    rightColumn->addChild(createLabelToggleRow("Enable", "legacy-pulse", 140.0f, 0.35f, 0.6f));
    rightColumn->addChild(createLabelSliderRow("Pulse FPS", "pulse-fps", 140.0f, 0.32f));
    
    auto spacer1 = CCNode::create();
    spacer1->setContentSize({1, 8});
    rightColumn->addChild(spacer1);
    
    rightColumn->addChild(createSectionTitle("Misc"));
    rightColumn->addChild(createLabelToggleRow("No Mirror", "nomirror", 140.0f, 0.32f, 0.55f));
    rightColumn->addChild(createLabelToggleRow("Ball Rotation Bug", "ballrot-bug", 140.0f, 0.32f, 0.55f));
    
    rightColumn->updateLayout();
    m_contentContainer->addChild(rightColumn);
    
    m_contentContainer->updateLayout();
}

CCNode* ModernSettingsPopup::createSectionTitle(const std::string& title) {
    auto titleLabel = CCLabelBMFont::create(title.c_str(), "modernBold.fnt"_spr);
    titleLabel->setScale(0.38f);
    titleLabel->setOpacity(200);
    titleLabel->setAnchorPoint({0, 0.5f});
    return titleLabel;
}

CCNode* ModernSettingsPopup::createSectionTitleWithToggle(const std::string& title, const std::string& settingId, float width) {
    auto container = CCNode::create();
    container->setContentSize({width, 16.0f});
    container->setLayout(
        RowLayout::create()
            ->setGap(5.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    auto titleLabel = CCLabelBMFont::create(title.c_str(), "modernBold.fnt"_spr);
    titleLabel->setScale(0.38f);
    titleLabel->setOpacity(200);
    titleLabel->setAnchorPoint({0, 0.5f});
    container->addChild(titleLabel);
    
    auto menu = CCMenu::create();
    menu->setContentSize({20, 16});
    menu->setLayout(AnchorLayout::create());
    
    auto toggler = createToggler(settingId, {0, 0});
    toggler->setScale(0.55f);
    toggler->setLayoutOptions(
        AnchorLayoutOptions::create()
            ->setAnchor(Anchor::Center)
    );
    menu->addChild(toggler);
    menu->updateLayout();
    
    container->addChild(menu);
    container->updateLayout();
    
    return container;
}

CCNode* ModernSettingsPopup::createSection(const std::string& title, float width) {
    auto section = CCNode::create();
    section->setContentSize({width, 0});
    section->setLayout(
        ColumnLayout::create()
            ->setGap(5.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(true)
            ->setCrossAxisAlignment(AxisAlignment::Start)
            ->setGrowCrossAxis(true)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    auto titleLabel = CCLabelBMFont::create(title.c_str(), "modernBold.fnt"_spr);
    titleLabel->setScale(0.4f);
    titleLabel->setOpacity(200);
    titleLabel->setAnchorPoint({0, 0.5f});
    section->addChild(titleLabel);
    
    return section;
}

CCNode* ModernSettingsPopup::createLabelToggleRow(const std::string& labelText, const std::string& settingId, float width, float labelScale, float toggleScale) {
    auto menu = CCMenu::create();
    menu->setContentSize({width, 16.0f});
    menu->setLayout(
        RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Between)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setAutoScale(false)
            ->setGrowCrossAxis(true)
    );
    
    auto label = CCLabelBMFont::create(labelText.c_str(), "modernMain.fnt"_spr);
    label->setScale(labelScale);
    label->setOpacity(190);
    label->setAnchorPoint({0, 0.5f});
    menu->addChild(label);
    
    auto toggler = createToggler(settingId, {0, 0});
    toggler->setScale(toggleScale);
    menu->addChild(toggler);
    
    menu->updateLayout();
    
    return menu;
}

CCNode* ModernSettingsPopup::createLabelSliderRow(const std::string& labelText, const std::string& settingId, float width, float labelScale) {
    auto row = CCNode::create();
    row->setContentSize({width, 32.0f});
    row->setAnchorPoint({0.5f, 0.5f});
    row->setLayout(
        RowLayout::create()
            ->setGap(10.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(false)
            ->setCrossAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    auto label = CCLabelBMFont::create(labelText.c_str(), "modernMain.fnt"_spr);
    label->setScale(labelScale);
    label->setOpacity(180);
    label->setAnchorPoint({0, 0.5f});
    row->addChild(label);
    
    auto menu = CCMenu::create();
    menu->setContentSize({width, 18.0f});
    menu->setPosition({0, 0});
    menu->setScale(0.55f);
    menu->setLayout(
        AnchorLayout::create()
    );
    
    auto slider = createSlider(settingId, {0, 0}, width - 5.0f);
    slider->setAnchorPoint({0.5f, 0.5f});
    slider->setLayoutOptions(
        AnchorLayoutOptions::create()
            ->setAnchor(Anchor::Center)
    );

    menu->addChild(slider);
    
    row->addChild(menu);
    menu->updateLayout();
    
    auto valueLabel = CCLabelBMFont::create("0.0", "modernMain.fnt"_spr);
    valueLabel->setScale(0.25f);
    valueLabel->setOpacity(130);
    valueLabel->setAnchorPoint({0, 0.5f});
    row->addChild(valueLabel);
    
    m_valueLabels[slider->getTag()] = valueLabel;
    
    // update initial values
    float value = 0;
    if (settingId == "random-delay") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        valueLabel->setString((std::to_string(value).substr(0, 4) + "s").c_str());
    } else if (settingId == "max-points") {
        int value = Mod::get()->getSettingValue<int64_t>(settingId);
        valueLabel->setString(std::to_string(value).c_str());
    } else if (settingId == "pos-skip-rate") {
        int value = Mod::get()->getSettingValue<int64_t>(settingId);
        valueLabel->setString(std::to_string(value).c_str());
    } else if (settingId == "pulse-fps") {
        int value = Mod::get()->getSettingValue<int64_t>(settingId);
        valueLabel->setString(std::to_string(value).c_str());
    } else {
        float value = Mod::get()->getSettingValue<double>(settingId);
        valueLabel->setString(std::to_string(value).substr(0, 4).c_str());
    }
    
    row->updateLayout();
    
    return row;
}

CCNode* ModernSettingsPopup::createParticleToggleRow(const std::string& labelText, const std::string& settingId, float width) {
    auto menu = CCMenu::create();
    menu->setContentSize({width * 0.6f, 16.0f});
    menu->setLayout(
        RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Even)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setAutoScale(false)
            ->setGrowCrossAxis(true)
    );
    
    auto label = CCLabelBMFont::create(labelText.c_str(), "modernMain.fnt"_spr);
    label->setScale(0.35f);
    label->setOpacity(190);
    label->setAnchorPoint({0, 0.5f});
    menu->addChild(label);
    
    auto toggler = createToggler(settingId, {0, 0});
    toggler->setScale(0.6f);
    menu->addChild(toggler);
    
    menu->updateLayout();
    
    return menu;
}

CCNode* ModernSettingsPopup::createLabelColorRow(const std::string& labelText, const std::string& colorSettingId, float width, const std::string& toggleSettingId, float labelScale) {
    auto menu = CCMenu::create();
    menu->setContentSize({width, 18.0f});
    menu->setLayout(
        RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Between)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setAutoScale(false)
            ->setGrowCrossAxis(true)
    );
    
    auto leftSide = CCNode::create();
    leftSide->setContentSize({0, 18.0f});
    leftSide->setLayout(
        RowLayout::create()
            ->setGap(4.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    auto label = CCLabelBMFont::create(labelText.c_str(), "modernMain.fnt"_spr);
    label->setScale(labelScale);
    label->setOpacity(170);
    label->setAnchorPoint({0, 0.5f});
    leftSide->addChild(label);
    
    auto colorBtn = createColorPickerButton(colorSettingId, {0, 0}, false);
    colorBtn->setScale(0.45f);
    leftSide->addChild(colorBtn);
    
    leftSide->updateLayout();
    menu->addChild(leftSide);
    
    if (!toggleSettingId.empty()) {
        auto toggler = createToggler(toggleSettingId, {0, 0});
        toggler->setScale(0.5f);
        menu->addChild(toggler);
    } else {
        auto spacer = CCNode::create();
        spacer->setContentSize({15, 15});
        menu->addChild(spacer);
    }
    
    menu->updateLayout();
    
    return menu;
}

CCNode* ModernSettingsPopup::createParticleColorPair(const std::string& toggleSettingId, const std::string& startColorId, const std::string& endColorId, float width) {
    auto container = CCNode::create();
    container->setContentSize({width, 45.0f});
    container->setLayout(
        ColumnLayout::create()
            ->setGap(4.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(true)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    auto toggleRow = createLabelToggleRow("Enable", toggleSettingId, width, 0.35f, 0.6f);
    container->addChild(toggleRow);
    
    auto colorRow = createColorPairRow(startColorId, endColorId, width);
    container->addChild(colorRow);
    
    container->updateLayout();
    
    return container;
}

CCNode* ModernSettingsPopup::createColorPairRow(const std::string& startColorId, const std::string& endColorId, float width) {
    auto menu = CCMenu::create();
    menu->setContentSize({width * 0.6f, 20.0f});
    menu->setLayout(
        RowLayout::create()
            ->setGap(10.0f)
            ->setAxisAlignment(AxisAlignment::Even)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setAutoScale(false)
            ->setGrowCrossAxis(true)
    );
    
    auto startContainer = CCNode::create();
    startContainer->setContentSize({0, 20});
    startContainer->setLayout(
        RowLayout::create()
            ->setGap(3.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    auto sLabel = CCLabelBMFont::create("S", "modernMain.fnt"_spr);
    sLabel->setScale(0.35f);
    sLabel->setOpacity(160);
    startContainer->addChild(sLabel);
    
    auto startBtn = createColorPickerButton(startColorId, {0, 0}, true);
    startBtn->setScale(0.48f);
    startContainer->addChild(startBtn);
    
    startContainer->updateLayout();
    menu->addChild(startContainer);
    
    auto endContainer = CCNode::create();
    endContainer->setContentSize({0, 20});
    endContainer->setLayout(
        RowLayout::create()
            ->setGap(3.0f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    
    auto eLabel = CCLabelBMFont::create("E", "modernMain.fnt"_spr);
    eLabel->setScale(0.35f);
    eLabel->setOpacity(160);
    endContainer->addChild(eLabel);
    
    auto endBtn = createColorPickerButton(endColorId, {0, 0}, true);
    endBtn->setScale(0.48f);
    endContainer->addChild(endBtn);
    
    endContainer->updateLayout();
    menu->addChild(endContainer);
    
    menu->updateLayout();
    
    return menu;
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

Slider* ModernSettingsPopup::createSlider(std::string settingId, CCPoint position, float width) {
    std::string grooveSprite = "modernSliderGroove.png"_spr;
    std::string barSprite = "modernSliderBar.png"_spr;
    std::string thumbSprite = "modernThumb.png"_spr;
    std::string thumbSelSprite = "modernThumbSel.png"_spr;

    auto okBroWhatever = CCSprite::create(barSprite.c_str());

    auto slider = Slider::create(
        this, 
        menu_selector(ModernSettingsPopup::onSlider), 
        barSprite.c_str(),
        grooveSprite.c_str(),
        thumbSprite.c_str(),
        thumbSelSprite.c_str(),
        0.75f
    );
    auto m_thumb = slider->getThumb();
    slider->setPosition(position);
    slider->m_sliderBar->setContentSize({width, slider->m_sliderBar->getContentSize().height});
    slider->m_sliderBar->setDisplayFrame(okBroWhatever->displayFrame());
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
    } else if (settingId == "max-points") {
        int value = Mod::get()->getSettingValue<int64_t>(settingId);
        slider->setValue((value - 2.0f) / (20.0f - 2.0f));
    } else if (settingId == "segment-tuner") {
        float value = Mod::get()->getSettingValue<double>(settingId);
        slider->setValue((value - 0.1f) / (10.0f - 0.1f));
    } else if (settingId == "pos-skip-rate") {
        int value = Mod::get()->getSettingValue<int64_t>(settingId);
        slider->setValue((value - 1.0f) / (50.0f - 1.0f));
    } else if (settingId == "pulse-fps") {
        int value = Mod::get()->getSettingValue<int64_t>(settingId);
        slider->setValue((value - 60.0f) / (1080.0f - 60.0f));
    }
    
    return slider;
}

CCMenu* ModernSettingsPopup::createColorPickerButton(const std::string& settingId, CCPoint position, bool is4B) {
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
    
    button->setPosition({0, 0});
    
    button->setUserObject("setting-id"_spr, CCString::create(settingId));
    button->setUserObject("is-4b"_spr, CCBool::create(is4B));
    
    auto menu = CCMenu::create();
    menu->setLayout(
        RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setAutoScale(false)
            ->setAutoGrowAxis(true)
    );
    menu->addChild(button);
    menu->updateLayout();
    
    return menu;
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

    log::info("ModernUI: toggled {} -> {}", settingIdStr, newValue ? "true" : "false");
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
    } else if (settingIdStr == "max-points") {
        int mapped = 2 + (int)(value * (20 - 2));
        Mod::get()->setSettingValue<int64_t>(settingIdStr, mapped);
        if (this->m_valueLabels.count(tag)) {
            this->m_valueLabels[tag]->setString(std::to_string(mapped).c_str());
        }
    } else if (settingIdStr == "segment-tuner") {
        float mapped = 0.1f + value * (10.0f - 0.1f);
        Mod::get()->setSettingValue<double>(settingIdStr, mapped);
        if (this->m_valueLabels.count(tag)) {
            std::string t = std::to_string(mapped).substr(0,4);
            this->m_valueLabels[tag]->setString(t.c_str());
        }
    } else if (settingIdStr == "pos-skip-rate") {
        int mapped = 1 + (int)(value * (50 - 1));
        Mod::get()->setSettingValue<int64_t>(settingIdStr, mapped);
        if (this->m_valueLabels.count(tag)) {
            this->m_valueLabels[tag]->setString(std::to_string(mapped).c_str());
        }
    } else if (settingIdStr == "pulse-fps") {
        int mapped = 60 + (int)(value * (1080 - 60));
        Mod::get()->setSettingValue<int64_t>(settingIdStr, mapped);
        if (this->m_valueLabels.count(tag)) {
            this->m_valueLabels[tag]->setString(std::to_string(mapped).c_str());
        }
    }
}

void ModernSettingsPopup::onColorPicker(CCObject* sender) {
    auto menuItem = static_cast<CCMenuItemSpriteExtra*>(sender);
    if (!menuItem) return;
    
    auto settingIdObj = typeinfo_cast<CCString*>(menuItem->getUserObject("setting-id"_spr));
    if (!settingIdObj) return;
    
    auto is4BObj = typeinfo_cast<CCBool*>(menuItem->getUserObject("is-4b"_spr));
    if (!is4BObj) return;
    
    m_currentColorSettingId = std::string(settingIdObj->getCString());
    m_isColor4B = is4BObj->getValue();
    m_currentButtonSprite = typeinfo_cast<CCSprite*>(menuItem->getNormalImage());
    
    log::info("Opening color picker for: {} (is4B: {})", m_currentColorSettingId, m_isColor4B);
    
    geode::ColorPickPopup* colorPopup = nullptr;
    
    if (m_isColor4B) {
        auto currentColor = Mod::get()->getSettingValue<cocos2d::ccColor4B>(m_currentColorSettingId);
        colorPopup = geode::ColorPickPopup::create(currentColor);
    } else {
        auto currentColor = Mod::get()->getSettingValue<cocos2d::ccColor3B>(m_currentColorSettingId);
        colorPopup = geode::ColorPickPopup::create(currentColor);
    }
    
    if (colorPopup) {
        colorPopup->setDelegate(this);
        colorPopup->show();
    }
}

void ModernSettingsPopup::updateColor(cocos2d::ccColor4B const& color) {
    if (m_currentColorSettingId.empty()) return;
    
    if (m_isColor4B) {
        Mod::get()->setSettingValue<cocos2d::ccColor4B>(m_currentColorSettingId, color);
        log::info("Saved 4B color {} to ({}, {}, {}, {})", 
            m_currentColorSettingId, color.r, color.g, color.b, color.a);
    } else {
        cocos2d::ccColor3B color3B = {color.r, color.g, color.b};
        Mod::get()->setSettingValue<cocos2d::ccColor3B>(m_currentColorSettingId, color3B);
        log::info("Saved 3B color {} to ({}, {}, {})", 
            m_currentColorSettingId, color.r, color.g, color.b);
    }
    
    if (m_currentButtonSprite) {
        m_currentButtonSprite->setColor({color.r, color.g, color.b});
    }
}

void ModernSettingsPopup::onModSettings(CCObject*) {
    geode::openSettingsPopup(Mod::get());
}