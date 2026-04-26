#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "../Helpers/DRSprite.hpp"

using namespace geode::prelude;

class $modify(LevelInfoLayerHook, LevelInfoLayer) {
	bool init(GJGameLevel * level, bool challenge) {
		if (!LevelInfoLayer::init(level, challenge)) return false;

		if (CCSprite* child = static_cast<CCSprite*>(this->getChildByIDRecursive("difficulty-sprite"))) {
			DRSprite* face = DRSprite::createFromLevel(level);

			if (face) {
				child->setOpacity(0);
				face->setPosition(child->getPosition());
				face->setZOrder(child->getZOrder());
				child->getParent()->addChild(face);
			}
		}

		return true;
	}
};