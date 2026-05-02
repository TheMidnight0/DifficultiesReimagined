#include <Geode/Geode.hpp>
#include <Geode/modify/LevelListCell.hpp>
#include "../Helpers/DRSprite.hpp"

using namespace geode::prelude;

class $modify(LevelListCellHook, LevelListCell) {
	void loadFromList(GJLevelList * list) {
		LevelListCell::loadFromList(list);

		if (CCSprite* child = static_cast<CCSprite*>(this->getChildByIDRecursive("difficulty-sprite"))) {
			DRSprite* face = DRSprite::createFromDifficulty(list->m_difficulty, Short);

			if (face) {
				child->setOpacity(0);
				face->setPosition(child->getPosition());
				face->setZOrder(child->getZOrder());
				child->getParent()->addChild(face);
			}
		}
	}
};