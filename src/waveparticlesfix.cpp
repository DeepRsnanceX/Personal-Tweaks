#include <Geode/modify/PlayerObject.hpp>
using namespace geode::prelude;

class $modify(PlayerObject){

	void update(float dt){
		PlayerObject::update(dt);

		if(m_isDart){
			m_trailingParticles->setVisible(false);
		} else {
			m_trailingParticles->setVisible(true);
		}
	}
};