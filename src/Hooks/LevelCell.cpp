#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include "../Helpers/DRSprite.hpp"

using namespace geode::prelude;

class $modify(LevelCellHook, LevelCell) {
	void loadFromLevel(GJGameLevel * level) {
		LevelCell::loadFromLevel(level);
		
		if (CCSprite* child = static_cast<CCSprite*>(this->getChildByIDRecursive("difficulty-sprite"))) {
			DRSprite* face = DRSprite::createFromLevel(level);

			if (face) {
				child->setOpacity(0);
				face->setPosition(child->getPosition());
				face->setZOrder(child->getZOrder());
				child->getParent()->addChild(face);
			}
		}
	}
};