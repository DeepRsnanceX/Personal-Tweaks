#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

auto mod = Mod::get();

bool doCol1 = mod->getSettingValue<bool>("rgb-col1");
bool doCol2 = mod->getSettingValue<bool>("rgb-col2");
float sat = mod->getSettingValue<double>("rgb-saturation");

$on_mod(Loaded){
    listenForSettingChanges("rgb-col1", [](bool value) {
        doCol1 = value;
    });
    listenForSettingChanges("rgb-col2", [](bool value) {
        doCol2 = value;
    });
    listenForSettingChanges("rgb-saturation", [](double value) {
        sat = value;
    });
}

class $modify(RGBPlayerObject, PlayerObject) {
    struct Fields {
        bool usingCol1 = doCol1;
        bool usingCol2 = doCol2;
    };
    
    std::tuple<int, int, int> hsvToRgbRaw(float h, float s, float v) {
        float r, g, b;
        int i = int(h * 6);
        float f = h * 6 - i;
        float p = v * (1 - s);
        float q = v * (1 - f * s);
        float t = v * (1 - (1 - f) * s);

        switch (i % 6) {
            case 0: r = v, g = t, b = p; break;
            case 1: r = q, g = v, b = p; break;
            case 2: r = p, g = v, b = t; break;
            case 3: r = p, g = q, b = v; break;
            case 4: r = t, g = p, b = v; break;
            case 5: r = v, g = p, b = q; break;
        }
        return {
            static_cast<int>(r * 255),
            static_cast<int>(g * 255),
            static_cast<int>(b * 255)
        };
    }

    void update(float p0) {
        PlayerObject::update(p0);

        auto fld = m_fields.self();
        auto playLayer = PlayLayer::get();
        if (!playLayer) return;

        float counter = counter + 0.15f;
        float hue = fmod(counter * 0.01f, 1.0f); // <- multiply speed here

        auto [r, g, b] = hsvToRgbRaw(hue, sat, 1.0f);

        cocos2d::ccColor4F colorForParticle(
            r / 255.0f,
            g / 255.0f,
            b / 255.0f,
            1.0f
        );

        if (fld->usingCol1) {
            m_iconSprite->setColor({static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});
            m_vehicleSprite->setColor({static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});
            if (m_isRobot) {
                m_robotSprite->m_color = ccColor3B(static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b));
                m_robotSprite->
            } else if (m_isSpider) {
                m_spiderSprite->m_color = ccColor3B(static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b));
            }
            
            if (m_ghostTrail) {
                m_ghostTrail->m_color = ccColor3B(static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b));
            }

            //particles bc other mods don't do them lol
            m_playerGroundParticles->m_tStartColor = colorForParticle;
            m_playerGroundParticles->m_tEndColor = colorForParticle;
            m_vehicleGroundParticles->m_tStartColor = colorForParticle;
            m_vehicleGroundParticles->m_tEndColor = colorForParticle;
            m_landParticles0->m_tStartColor = colorForParticle;
            m_landParticles0->m_tEndColor = colorForParticle;
            m_landParticles1->m_tStartColor = colorForParticle;
            m_landParticles1->m_tEndColor = colorForParticle;
        }
        
        if (fld->usingCol2) {
            log::debug("* You enabled RGB for color 2.");
            log::debug("* ... Nothing happened.");
        }
        
        if (m_isRobot) {
            if (fld->usingCol1 || fld->usingCol2) {
                m_robotSprite->updateColors();
            }
        } else if (m_isSpider) {
            if (fld->usingCol1 || fld->usingCol2) {
                m_spiderSprite->updateColors();
            }
        }
    }
};
