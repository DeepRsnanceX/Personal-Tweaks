#include <Geode/modify/GJGarageLayer.hpp>
#include <hiimjustin000.more_icons/include/MoreIcons.hpp>
#include "Geode/utils/cocos.hpp"
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/FLAlertLayer.hpp>

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

CCImage* getIconImage(SimplePlayer* player) {
    auto gm = GameManager::sharedState();
    bool isGlowCurrentlyVisible = player->m_outlineSprite->isVisible();

    auto visitMe = player->m_firstLayer;
    if (gm->m_playerIconType == IconType::Robot) {
        visitMe = player->m_robotSprite;
    } else if (gm->m_playerIconType == IconType::Spider) {
        visitMe = player->m_spiderSprite;
    }

    // adds margin so img isnt cut off
    auto iconSize = visitMe->getContentSize();
    const int margin = 8;
    const int texWidth = static_cast<int>(iconSize.width) + margin * 2;
    const int texHeight = static_cast<int>(iconSize.height) + margin * 2;

    auto renderTex = CCRenderTexture::create(texWidth, texHeight, kCCTexture2DPixelFormat_RGBA8888);
    if (!renderTex) {
        geode::log::debug("Failed to create render texture!");
        return nullptr;
    }

    auto origPos = visitMe->getPosition();

    // center icon and make glow visible if it isnt
    // compat w/ icon preview lol
    visitMe->setPosition({texWidth / 2.0f, texHeight / 2.0f});
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
        std::vector<CCImage*> m_batchIcons; // Store icon images
        std::vector<CCSize> m_batchIconSizes; // Store icon sizes for layout
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
        std::string dir = "C:\\Users\\PC\\Desktop\\MIX";
        std::string filePath = dir + "\\chosen_icons.txt";
        bool fileSaved = false;

        // Try to create directory if it doesn't exist (C++17)
        try {
            std::filesystem::create_directories(dir);
        } catch (...) {
            geode::log::debug("Failed to create directory: {}", dir);
        }

        // Try to write to file
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

        CCImage* image = getIconImage(m_playerObject);
        std::string filePath = fmt::format("C:\\Users\\PC\\Desktop\\renders\\{}", resultingFileName);
        bool saved = image->saveToFile(filePath.c_str(), false);
        delete image;

        auto notifSprite = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
        Notification::create("Icon Rendered!", notifSprite, 0.5f)->show();
    }

    void addIconToBatchRender(CCObject* sender) {
        
    }

    void renderBatchIcons(CCObject* sender) {

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

        rendererButtonsMenu->setContentSize({40.f, 100.f});
        rendererButtonsMenu->setPosition({20.f, 90.f});
        rendererButtonsMenu->setID("pt-renderer-buttons-menu"_spr);
        this->addChild(rendererButtonsMenu, 2);

        rendererButtonsMenu->addChild(renderIconButton);
        rendererButtonsMenu->addChild(addToBatchButton);
        rendererButtonsMenu->addChild(renderBatchButton);
        rendererButtonsMenu->updateLayout();

        return true;
    }
};