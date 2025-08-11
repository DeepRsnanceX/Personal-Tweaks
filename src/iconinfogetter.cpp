#include <Geode/modify/GJGarageLayer.hpp>
#include <hiimjustin000.more_icons/include/MoreIcons.hpp>
#include "Geode/utils/cocos.hpp"
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace geode::prelude;

int getIconId(IconType type) {
    auto gm = GameManager::sharedState();
    switch(type) {
        case IconType::Cube:
            return gm->getPlayerFrame();
        case IconType::Ship:
            return gm->getPlayerShip();
        case IconType::Ball:
            return gm->getPlayerBall();
        case IconType::Ufo:
            return gm->getPlayerBird();
        case IconType::Wave:
            return gm->getPlayerDart();
        case IconType::Robot:
            return gm->getPlayerRobot();
        case IconType::Spider:
            return gm->getPlayerSpider();
        case IconType::Swing:
            return gm->getPlayerSwing();
        case IconType::Jetpack:
            return gm->getPlayerJetpack();
        default:
            return 0;
    }
}

std::string getIconTypeName(IconType type) {
    switch(type) {
        case IconType::Cube:
            return "Cube";
        case IconType::Ship:
            return "Ship";
        case IconType::Ball:
            return "Ball";  
        case IconType::Ufo:
            return "Ufo";
        case IconType::Wave:
            return "Wave";
        case IconType::Robot:
            return "Robot";
        case IconType::Spider:
            return "Spider";
        case IconType::Swing:
            return "Swing";
        case IconType::Jetpack:
            return "Jetpack";
        default:
            return "Unknown";
    }
}

std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

CCImage* getIconImage(SimplePlayer* player) {
    auto gm = GameManager::sharedState();
    bool isGlowCurrentlyVisible = player->m_outlineSprite->isVisible();

    auto visitMe = player->m_firstLayer;
    if (gm->m_playerIconType == IconType::Robot) {
        visitMe = player->m_robotSprite;
    } else if (gm->m_playerIconType == IconType::Spider) {
        visitMe = player->m_spiderSprite;
    }

    auto iconSize = visitMe->getContentSize();
    const int margin = 6;
    const int ufoExtraHeight = (gm->m_playerIconType == IconType::Ufo) ? 12 : 0;
    const int robotExtraHeight = (gm->m_playerIconType == IconType::Robot) ? 16 : 0;
    const int texWidth = static_cast<int>(iconSize.width) + margin * 2;
    const int texHeight = static_cast<int>(iconSize.height) + margin * 2 + ufoExtraHeight + robotExtraHeight;

    auto renderTex = CCRenderTexture::create(texWidth, texHeight, kCCTexture2DPixelFormat_RGBA8888);
    if (!renderTex) {
        geode::log::debug("Failed to create render texture!");
        return nullptr;
    }

    auto origPos = visitMe->getPosition();

    // center icon and make glow visible if it isnt
    // compat w/ icon preview lol
    float posX = texWidth / 2.0f;
    float posY = texHeight / 2.0f;

    if (gm->m_playerIconType == IconType::Ufo) {
        posY -= (ufoExtraHeight / 1.25f);
    } else if (gm->m_playerIconType == IconType::Robot) {
        posY -= (robotExtraHeight / 2.f);
    }
    
    visitMe->setPosition({posX, posY});
    player->m_outlineSprite->setVisible(gm->getPlayerGlow());

    renderTex->beginWithClear(0, 0, 0, 0);
    visitMe->visit();
    renderTex->end();

    // restore
    visitMe->setPosition(origPos);
    player->m_outlineSprite->setVisible(isGlowCurrentlyVisible);

    CCImage* image = renderTex->newCCImage();

    return image;
}

class $modify(ImSoLazyGarageLayer, GJGarageLayer) {
    struct Fields {
        std::vector<std::string> m_iconList;
        // ola
        // when eres un separador
        // soy un separador
        CCMenu* m_batchMenu = nullptr;
        int m_batchIconCount = 0;
        const int m_maxIconsPerRow = 8;
        const int m_iconMargin = 6;
        std::vector<CCSprite*> m_batchSprites;
        std::vector<IconType> m_batchIconTypes;
    };

    void addIconToList(CCObject* sender) {
        auto addedSpr = CCSprite::create("addToList.png"_spr);
        auto fields = m_fields.self();

        IconInfo* selectedIcon = MoreIcons::getIcon(m_selectedIconType);
        std::string iconName = selectedIcon->shortName;

        fields->m_iconList.push_back(iconName);

        Notification::create("Icon " + iconName + " added!", addedSpr, 0.30f)->show();
    }

    void printListOfIcons(CCObject* sender) {
        auto fields = m_fields.self();
        auto errSpr = CCSprite::createWithSpriteFrameName("exMark_001.png");
        auto worked = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");

        auto configDir = Mod::get()->getConfigDir(true);
        auto filePath = configDir / "chosen_icons.txt";
        bool fileSaved = false;

        std::ofstream out(filePath);
        if (out.is_open()) {
            for (const auto& icon : fields->m_iconList) {
                out << icon << "\n";
            }
            out.close();
            fileSaved = true;
            geode::log::debug("Icon list saved to {}", filePath);
            Notification::create("Icon list saved to chosen_icons.txt!", worked, 0.5f)->show();
        }

        // Fallback: print to debug log if file failed
        if (!fileSaved) {
            std::string allIcons;
            for (const auto& icon : fields->m_iconList) {
                allIcons += icon + "\n";
            }
            geode::log::debug("Icon list:\n{}", allIcons);
            Notification::create("Failed to save file! List printed to debug.", errSpr, 0.5f)->show();
        }
    }

    void checkListPopup(CCObject* sender) {
        auto fields = m_fields.self();

        std::string allIcons;
        for (const auto& icon : fields->m_iconList) {
            allIcons += "- " + icon + "\n";
        }

        std::string popupContent = fmt::format("# Your Current Icon List: \n\n{}", allIcons);

        MDPopup::create(
            "Chosen icons list",
            popupContent,
            "ok",
            "thanks",
            static_cast<std::function<void(bool)>>(nullptr)
        )->show();
    }

    void removeLastIconFromList(CCObject* sender) {
        auto fields = m_fields.self();
        auto err = CCSprite::createWithSpriteFrameName("exMark_001.png");
        auto removedSpr = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");

        if (!fields->m_iconList.empty()) {
            std::string removed = fields->m_iconList.back();
            fields->m_iconList.pop_back();
            geode::log::debug("Removed icon: {}", removed);
            Notification::create("Removed last icon!", removedSpr, 0.3f)->show();
        } else {
            geode::log::debug("Icon list is already empty.");
            Notification::create(" Icon list is empty!", err, 0.3f)->show();
        }
    }

    void countMixIcons(CCObject* sender) {
        size_t cubeCount    = MoreIcons::getIcons(IconType::Cube).size();
        size_t shipCount    = MoreIcons::getIcons(IconType::Ship).size();
        size_t ballCount    = MoreIcons::getIcons(IconType::Ball).size();
        size_t ufoCount     = MoreIcons::getIcons(IconType::Ufo).size();
        size_t waveCount    = MoreIcons::getIcons(IconType::Wave).size();
        size_t robotCount   = MoreIcons::getIcons(IconType::Robot).size();
        size_t spiderCount  = MoreIcons::getIcons(IconType::Spider).size();
        size_t swingCount   = MoreIcons::getIcons(IconType::Swing).size();
        size_t jetpackCount = MoreIcons::getIcons(IconType::Jetpack).size();

        size_t total = cubeCount + shipCount + ballCount + ufoCount + waveCount + robotCount + spiderCount + swingCount + jetpackCount;

        std::string infoData = fmt::format(
            "<cg>Cube</c>: {}\n<cp>Ship</c>: {}\n<cr>Ball</c>: {}\n<co>Ufo</c>: {}\n<cf>Wave</c>: {}\nRobot: {}\n<ca>Spider</c>: {}\n<cy>Swing</c>: {}\n<cp>Jetpack</c>: {}\n\n<cd>Total</c>: {}",
            cubeCount, shipCount, ballCount, ufoCount, waveCount,
            robotCount, spiderCount, swingCount, jetpackCount, total
        );

        IconInfo* thing = MoreIcons::getIcon(m_selectedIconType);
        std::string packInfoString = fmt::format("{} Info", thing->packName);

        FLAlertLayer::create(packInfoString.c_str(), infoData.c_str(), "ok")->show();
    }

    // ---------------------------------------
    // ICON RENDERER STUFF
    // ---------------------------------------

    void renderIconImage(CCObject* sender) {
        auto gm = GameManager::sharedState();
        bool isGlowCurrentlyVisible = m_playerObject->m_outlineSprite->isVisible();

        IconInfo* iconToRender = MoreIcons::getIcon(m_selectedIconType);

        int icID = getIconId(m_selectedIconType);
        std::string icTypeName = getIconTypeName(m_selectedIconType);
        std::string filenameInfo = "unknown";

        if (iconToRender) {
            filenameInfo = fmt::format("{}_{}", icTypeName, iconToRender->shortName);
        } else {
            filenameInfo = fmt::format("{}_{}", icTypeName, icID);
        }

        std::string glowInf = "unknown";

        if (gm->getPlayerGlow()) {
            glowInf = fmt::format("glow{}", gm->getPlayerGlowColor());
        } else {
            glowInf = "noglow";
        }

        std::string resultingFileName = fmt::format("render-{}-clrs{}+{}_{}.png", filenameInfo, gm->getPlayerColor(), gm->getPlayerColor2(), glowInf);

        auto configDir = Mod::get()->getConfigDir(true);
        auto rendersDir = configDir / "renders";
        std::filesystem::create_directories(rendersDir);

        CCImage* image = getIconImage(m_playerObject);
        auto filePath = rendersDir / resultingFileName;
        bool saved = image->saveToFile(filePath.string().c_str(), false);
        delete image;

        auto notifSprite = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
        Notification::create("Icon Rendered!", notifSprite, 0.5f)->show();
    }

    void addIconToBatchRender(CCObject* sender) {
        auto fields = m_fields.self();
        auto gm = GameManager::sharedState();
        
        auto visitMe = m_playerObject->m_firstLayer;
        if (gm->m_playerIconType == IconType::Robot) {
            visitMe = m_playerObject->m_robotSprite;
        } else if (gm->m_playerIconType == IconType::Spider) {
            visitMe = m_playerObject->m_spiderSprite;
        }

        auto iconSize = visitMe->getContentSize();
        const int margin = fields->m_iconMargin;
        const int ufoExtraHeight = (gm->m_playerIconType == IconType::Ufo) ? 12 : 0;
        const int robotExtraHeight = (gm->m_playerIconType == IconType::Robot) ? 16 : 0;
        const int texWidth = static_cast<int>(iconSize.width) + margin * 2;
        const int texHeight = static_cast<int>(iconSize.height) + margin * 2 + ufoExtraHeight + robotExtraHeight;

        auto renderTex = CCRenderTexture::create(texWidth, texHeight, kCCTexture2DPixelFormat_RGBA8888);
        if (!renderTex) {
            geode::log::debug("Failed to create render texture for icon!");
            return;
        }

        bool isGlowCurrentlyVisible = m_playerObject->m_outlineSprite->isVisible();
        auto origPos = visitMe->getPosition();

        float posX = texWidth / 2.0f;
        float posY = texHeight / 2.0f;

        if (gm->m_playerIconType == IconType::Ufo) {
            posY -= (ufoExtraHeight / 1.25f);
        } else if (gm->m_playerIconType == IconType::Robot) {
            posY -= (robotExtraHeight / 2.f);
        }

        visitMe->setPosition({posX, posY});
        m_playerObject->m_outlineSprite->setVisible(gm->getPlayerGlow());

        renderTex->beginWithClear(0, 0, 0, 0);
        visitMe->visit();
        renderTex->end();

        visitMe->setPosition(origPos);
        m_playerObject->m_outlineSprite->setVisible(isGlowCurrentlyVisible);

        auto iconSprite = CCSprite::createWithTexture(renderTex->getSprite()->getTexture());
        iconSprite->setFlipY(true);
        iconSprite->retain();
        
        fields->m_batchSprites.push_back(iconSprite);
        fields->m_batchIconTypes.push_back(gm->m_playerIconType);
        
        if (!fields->m_batchMenu) {
            fields->m_batchMenu = CCMenu::create();
            fields->m_batchMenu->retain();
            
            auto rowLayout = RowLayout::create()
                ->setGap(0.f)
                ->setAxisAlignment(AxisAlignment::Start)
                ->setAutoScale(false)
                ->setCrossAxisOverflow(false);
            
            fields->m_batchMenu->setLayout(rowLayout);
            fields->m_batchMenu->setContentSize({iconSize.width + margin*2, iconSize.height + margin*2});
            
            geode::log::debug("Created batch menu with initial size {}x{}", texWidth, texHeight);
        } else {
            auto currentSize = fields->m_batchMenu->getContentSize();
            int iconsInCurrentRow = (fields->m_batchIconCount % fields->m_maxIconsPerRow) + 1;
            int currentRow = fields->m_batchIconCount / fields->m_maxIconsPerRow;
            
            float newWidth, newHeight;
            
            if (iconsInCurrentRow == 1) {
                newWidth = std::max(currentSize.width, static_cast<float>(texWidth));
                newHeight = currentSize.height + texHeight;
            } else {
                newWidth = currentSize.width + texWidth;
                newHeight = currentSize.height;
                
                if (texHeight > (currentSize.height / (currentRow + 1))) {
                    float rowHeight = currentSize.height / (currentRow + 1);
                    newHeight = (currentRow * rowHeight) + texHeight;
                }
            }
            
            fields->m_batchMenu->setContentSize({newWidth, newHeight});
            geode::log::debug("Updated menu size to {}x{} (icon {} in row {}, icon {} of row)", 
                             newWidth, newHeight, fields->m_batchIconCount + 1, currentRow, iconsInCurrentRow);
        }
        
        auto menuItem = CCMenuItemSpriteExtra::create(iconSprite, nullptr, nullptr);
        fields->m_batchMenu->addChild(menuItem);
        
        fields->m_batchIconCount++;
        
        if (fields->m_batchIconCount % fields->m_maxIconsPerRow == 0) {
            auto columnLayout = ColumnLayout::create()
                ->setGap(0.f)
                ->setAxisAlignment(AxisAlignment::Start)
                ->setAutoScale(false)
                ->setCrossAxisOverflow(false);
            
            auto newRowMenu = CCMenu::create();
            auto rowLayout = RowLayout::create()
                ->setGap(0.f)
                ->setAxisAlignment(AxisAlignment::Start)
                ->setAutoScale(false)
                ->setCrossAxisOverflow(false);
            
            geode::log::debug("Completed row {}, preparing for next row", fields->m_batchIconCount / fields->m_maxIconsPerRow);
        }
        
        fields->m_batchMenu->updateLayout();
        
        auto addedSpr = CCSprite::create("addToRenderBatch.png"_spr);
        Notification::create(fmt::format("Icon {} added to batch!", fields->m_batchIconCount), addedSpr, 0.30f)->show();
        
        geode::log::debug("Added icon {} to batch menu", fields->m_batchIconCount);
    }

    void renderBatchIcons(CCObject* sender) {
        auto fields = m_fields.self();
        auto errSpr = CCSprite::createWithSpriteFrameName("exMark_001.png");
        auto successSpr = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
        
        if (!fields->m_batchMenu || fields->m_batchSprites.empty()) {
            Notification::create("No icons in batch to render!", errSpr, 0.5f)->show();
            return;
        }
        
        auto children = CCArray::create();
        auto menuChildren = fields->m_batchMenu->getChildren();
        for (int i = 0; i < menuChildren->count(); i++) {
            auto child = static_cast<CCMenuItemSpriteExtra*>(menuChildren->objectAtIndex(i));
            children->addObject(child);
        }
        children->retain();
        fields->m_batchMenu->removeAllChildren();
        
        std::vector<CCMenu*> rowMenus;
        int totalIcons = children->count();
        int rows = (totalIcons + fields->m_maxIconsPerRow - 1) / fields->m_maxIconsPerRow;
        
        float maxRowWidth = 0;
        float totalHeight = 0;
        
        for (int row = 0; row < rows; row++) {
            auto rowMenu = CCMenu::create();
            auto rowLayout = RowLayout::create()
                ->setGap(0.f)
                ->setAxisAlignment(AxisAlignment::Start)
                ->setAutoScale(false);
            rowMenu->setLayout(rowLayout);
            
            int startIcon = row * fields->m_maxIconsPerRow;
            int endIcon = std::min(startIcon + fields->m_maxIconsPerRow, totalIcons);
            
            float rowWidth = 0;
            float rowHeight = 0;
            
            for (int i = startIcon; i < endIcon; i++) {
                auto menuItem = static_cast<CCMenuItemSpriteExtra*>(children->objectAtIndex(i));
                rowMenu->addChild(menuItem);
                
                auto spriteSize = menuItem->getContentSize();
                rowWidth += spriteSize.width;
                if (spriteSize.height > rowHeight) {
                    rowHeight = spriteSize.height;
                }
            }
            
            rowMenu->setContentSize({rowWidth, rowHeight});
            rowMenu->updateLayout();
            
            if (rowWidth > maxRowWidth) {
                maxRowWidth = rowWidth;
            }
            totalHeight += rowHeight;
            
            rowMenus.push_back(rowMenu);
            
            geode::log::debug("Created row {} with {} icons, size: {}x{}", row, endIcon - startIcon, rowWidth, rowHeight);
        }
        
        auto masterMenu = CCMenu::create();
        auto columnLayout = ColumnLayout::create()
            ->setGap(0.f)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(false);
        masterMenu->setLayout(columnLayout);
        masterMenu->setContentSize({maxRowWidth, totalHeight});
        
        for (auto rowMenu : rowMenus) {
            masterMenu->addChild(rowMenu);
        }
        masterMenu->updateLayout();
        
        children->release();
        
        const int padding = 10;
        int finalWidth = static_cast<int>(maxRowWidth) + padding * 2;
        int finalHeight = static_cast<int>(totalHeight) + padding * 2;
        
        geode::log::debug("Creating final render texture: {}x{}", finalWidth, finalHeight);
        
        auto finalRenderTex = CCRenderTexture::create(finalWidth, finalHeight, kCCTexture2DPixelFormat_RGBA8888);
        if (!finalRenderTex) {
            Notification::create("Failed to create final render texture!", errSpr, 0.5f)->show();
            return;
        }
        
        masterMenu->setPosition({finalWidth / 2.0f, finalHeight / 2.0f});
        
        finalRenderTex->beginWithClear(0, 0, 0, 0);
        masterMenu->visit();
        finalRenderTex->end();

        std::set<std::string> uniqueGamemodes;
        for (const auto& iconType : fields->m_batchIconTypes) {
            uniqueGamemodes.insert(getIconTypeName(iconType));
        }
        std::string gamemodeList;
        bool first = true;
        for (const auto& gamemode : uniqueGamemodes) {
            if (!first) gamemodeList += "_";
            gamemodeList += gamemode;
            first = false;
        }

        // huge ass filename but it's so u don't overwrite renders
        // unless you REALLY want to :sob:
        std::string timeString = getCurrentTimeString();
        std::string resultingFileName = fmt::format("batch-render-{}-icons-{}-{}.png", fields->m_batchSprites.size(), gamemodeList, timeString);

        auto configDir = Mod::get()->getConfigDir(true);
        auto rendersDir = configDir / "renders";
        std::filesystem::create_directories(rendersDir);
        
        CCImage* image = finalRenderTex->newCCImage();
        auto filePath = rendersDir / resultingFileName;
        
        bool saved = image->saveToFile(filePath.string().c_str(), false);
        delete image;
        
        if (saved) {
            Notification::create(fmt::format("Batch of {} icons rendered!", fields->m_batchSprites.size()), successSpr, 0.5f)->show();
            geode::log::debug("Batch rendered to: {}", filePath);
        } else {
            Notification::create("Failed to save batch render!", errSpr, 0.5f)->show();
        }
        
        if (fields->m_batchMenu) {
            fields->m_batchMenu->removeAllChildren();
            fields->m_batchMenu->release();
            fields->m_batchMenu = nullptr;
        }
        
        for (auto sprite : fields->m_batchSprites) {
            sprite->release();
        }
        fields->m_batchSprites.clear();
        fields->m_batchIconCount = 0;
    }

    void openConfigFolder(CCObject* sender) {
        auto configDir = Mod::get()->getConfigDir(true);
        auto rendersDir = configDir / "renders";

        if (!std::filesystem::exists(rendersDir)) {
            std::filesystem::create_directories(rendersDir);
        }

        geode::utils::file::openFolder(rendersDir);
    }

    bool init() {
        if (!GJGarageLayer::init()) return false;

        float extraOffset = 0.f;
        bool isSeparateLoaded = Loader::get()->isModLoaded("weebify.separate_dual_icons");

        if (isSeparateLoaded) {
            extraOffset = 90.f;
        }
        
        auto buttonsMenu = CCMenu::create();
        buttonsMenu->setLayout(
            RowLayout::create()
                ->setGap(4.f)
                ->setAxisAlignment(AxisAlignment::End)
                ->setAxisReverse(true)
                ->setCrossAxisOverflow(false)
        );
        buttonsMenu->setContentSize({100.f, 40.f});
        buttonsMenu->setPosition({m_playerObject->getPositionX() + 90.f + extraOffset, m_playerObject->getPositionY()});
        buttonsMenu->setID("pt-buttons-menu"_spr);
        this->addChild(buttonsMenu);

        auto addSpr = CircleButtonSprite::createWithSprite("addToList.png"_spr, 1.0f, CircleBaseColor::Pink, CircleBaseSize::Small);
        auto printSpr = CircleButtonSprite::createWithSprite("printList.png"_spr, 1.0f, CircleBaseColor::Cyan, CircleBaseSize::Small);
        auto removeLastSpr = CircleButtonSprite::createWithSprite("removeLast.png"_spr, 1.0f, CircleBaseColor::DarkAqua, CircleBaseSize::Small);
        auto checkListSpr = CircleButtonSprite::createWithSprite("elPutisimoOjoDelBetterEditHermanoJajaj.png"_spr, 1.0f, CircleBaseColor::Green, CircleBaseSize::Small);

        auto addButton = CCMenuItemSpriteExtra::create(
            addSpr,
            this,
            menu_selector(ImSoLazyGarageLayer::addIconToList)
        );
        auto printButton = CCMenuItemSpriteExtra::create(
            printSpr, 
            this, 
            menu_selector(ImSoLazyGarageLayer::printListOfIcons)
        );
        auto removeLastButton = CCMenuItemSpriteExtra::create(
            removeLastSpr,
            this,
            menu_selector(ImSoLazyGarageLayer::removeLastIconFromList)
        );
        auto checkListButton = CCMenuItemSpriteExtra::create(
            checkListSpr,
            this,
            menu_selector(ImSoLazyGarageLayer::checkListPopup)
        );

        buttonsMenu->addChild(addButton);
        buttonsMenu->addChild(printButton);
        buttonsMenu->addChild(removeLastButton);
        buttonsMenu->addChild(checkListButton);

        buttonsMenu->updateLayout();

        // icon counter
        auto infoMenu = CCMenu::create();
        infoMenu->setContentSize({20.f, 20.f});
        infoMenu->setID("mix-info"_spr);
        
        auto infoSpr = CCSprite::create("mix-info.png"_spr);
        auto infoButton = CCMenuItemSpriteExtra::create(infoSpr, this, menu_selector(ImSoLazyGarageLayer::countMixIcons));

        infoMenu->addChild(infoButton);
        infoButton->setPosition({10.f, 10.f});

        auto selBg = this->getChildByID("select-background");

        auto addX = selBg->getContentSize().width / 2;
        auto addY = selBg->getContentSize().height / 2;

        infoMenu->setPosition({selBg->getPositionX() + addX, selBg->getPositionY() + addY});
        infoMenu->setScale(0.7f);

        this->addChild(infoMenu, 2);

        // ----------------------------
        // icon renderer stuff
        // ----------------------------

        auto rendererButtonsMenu = CCMenu::create();
        rendererButtonsMenu->setLayout(
            ColumnLayout::create()
                ->setGap(2.f)
                ->setAxisAlignment(AxisAlignment::End)
                ->setAxisReverse(true)
                ->setCrossAxisOverflow(false)
        );

        auto renderIconSpr = CircleButtonSprite::createWithSprite("renderIcon.png"_spr, 1.0f, CircleBaseColor::Blue, CircleBaseSize::Small);
        auto addToBatchSpr = CircleButtonSprite::createWithSprite("addToRenderBatch.png"_spr, 1.0f, CircleBaseColor::Green, CircleBaseSize::Small);
        auto renderBatchSpr = CircleButtonSprite::createWithSprite("renderBatch.png"_spr, 1.0f, CircleBaseColor::Green, CircleBaseSize::Small);
        auto openFolderSpr = CCSprite::createWithSpriteFrameName("folderIcon_001.png");

        auto renderIconButton = CCMenuItemSpriteExtra::create(
            renderIconSpr,
            this,
            menu_selector(ImSoLazyGarageLayer::renderIconImage)
        );
        auto addToBatchButton = CCMenuItemSpriteExtra::create(
            addToBatchSpr,
            this,
            menu_selector(ImSoLazyGarageLayer::addIconToBatchRender)
        );
        auto renderBatchButton = CCMenuItemSpriteExtra::create(
            renderBatchSpr,
            this,
            menu_selector(ImSoLazyGarageLayer::renderBatchIcons)
        );
        auto openFolderButton = CCMenuItemSpriteExtra::create(
            openFolderSpr,
            this,
            menu_selector(ImSoLazyGarageLayer::openConfigFolder)
        );

        rendererButtonsMenu->setContentSize({40.f, 100.f});
        rendererButtonsMenu->setPosition({20.f, 90.f});
        rendererButtonsMenu->setID("pt-renderer-buttons-menu"_spr);
        this->addChild(rendererButtonsMenu, 2);

        rendererButtonsMenu->addChild(renderIconButton);
        rendererButtonsMenu->addChild(addToBatchButton);
        rendererButtonsMenu->addChild(renderBatchButton);
        rendererButtonsMenu->addChild(openFolderButton);
        rendererButtonsMenu->updateLayout();

        return true;
    }
};