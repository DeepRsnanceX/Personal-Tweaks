#include <Geode/modify/CCMotionStreak.hpp>

using namespace geode::prelude;

float trailMaxPts = Mod::get()->getSettingValue<int64_t>("max-points");
bool isLegacyTrailEnabled = Mod::get()->getSettingValue<bool>("use-legacytrail");

$on_mod(Loaded){
	listenForSettingChanges("max-points", [](int value) {
        trailMaxPts = value;
    });
	listenForSettingChanges("use-legacytrail", [](bool value) {
		isLegacyTrailEnabled = value;
	});
}

class $modify(LegacyTrailCCMSHook, cocos2d::CCMotionStreak) {

	bool initWithFade(float fade, float minSeg, float stroke, const cocos2d::ccColor3B& color, cocos2d::CCTexture2D* texture) {

		if (!isLegacyTrailEnabled) {
            return CCMotionStreak::initWithFade(fade, minSeg, stroke, color, texture);
        }

		this->setPosition({ .0f, .0f});
		this->setAnchorPoint({ .0f, .0f});
		this->ignoreAnchorPointForPosition(true);
		m_bStartingPositionInitialized = false;

		m_tPositionR = cocos2d::CCPoint{ .0f, .0f };
		m_bFastMode = true;
		m_fMinSeg = (minSeg == -1.f) ? stroke / 5.0f : minSeg;
		m_fMinSeg *= m_fMinSeg;

		m_fStroke = stroke;
		m_fFadeDelta = 1.0f / fade;

		//float maxPoints = 6;
		m_uMaxPoints = trailMaxPts;

		m_uNuPoints = 0;

		m_pPointState = std::allocator<float>().allocate(sizeof(float) * m_uMaxPoints);
		m_pPointVertexes = std::allocator<cocos2d::CCPoint>().allocate(sizeof(cocos2d::CCPoint) * m_uMaxPoints);

		m_pVertices = std::allocator<cocos2d::ccVertex2F>().allocate(sizeof(cocos2d::ccVertex2F) * m_uMaxPoints * 2);
		m_pTexCoords = std::allocator<cocos2d::ccTex2F>().allocate(sizeof(cocos2d::ccTex2F) * m_uMaxPoints * 2);
		m_pColorPointer = std::allocator<GLubyte>().allocate(sizeof(GLubyte) * m_uMaxPoints * 2 * 4);

		m_tBlendFunc.src = GL_SRC_ALPHA;
		m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;

		auto program = cocos2d::CCShaderCache::sharedShaderCache()->programForKey(
            kCCShader_PositionTextureColor
        );
		this->setShaderProgram(program);

		this->setTexture(texture);
		this->setColor(color);

		this->scheduleUpdate();

		return true;
	}
	
};