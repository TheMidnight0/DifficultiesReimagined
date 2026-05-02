#include <Geode/Geode.hpp>
#include <Geode/modify/MapPackCell.hpp>
#include "../Helpers/DRSprite.hpp"

using namespace geode::prelude;

class $modify(MapPackCellHook, MapPackCell) {
	void loadFromMapPack(GJMapPack * pack) {
		MapPackCell::loadFromMapPack(pack);

		if (CCSprite* child = this->getChildByType<CCSprite>(0)) {
			DRSprite* face = DRSprite::createFromStars(pack->m_stars, Short);

			if (face) {
				child->setOpacity(0);
				face->setPosition(child->getPosition());
				face->setZOrder(child->getZOrder());
				child->getParent()->addChild(face);
			}
		}
	}
};