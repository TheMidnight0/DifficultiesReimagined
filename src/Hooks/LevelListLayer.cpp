#include <Geode/Geode.hpp>
#include <Geode/modify/LevelListLayer.hpp>
#include "../Helpers/DRSprite.hpp"

using namespace geode::prelude;

class $modify(LevelListLayerHook, LevelListLayer) {
	bool init(GJLevelList * list) {
		if (!LevelListLayer::init(list)) return false;

		if (CCSprite* child = static_cast<CCSprite*>(this->getChildByIDRecursive("difficulty-sprite"))) {
			DRSprite* face = DRSprite::createFromDifficulty(list->m_difficulty, None);

			if (face) {
				child->setOpacity(0);
				face->setPosition(child->getPosition());
				face->setScale(child->getScale());
				face->setZOrder(child->getZOrder());
				child->getParent()->addChild(face);
			}
		}

		return true;
	}
};