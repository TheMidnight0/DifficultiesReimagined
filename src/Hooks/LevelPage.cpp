#include <Geode/Geode.hpp>
#include <Geode/modify/LevelPage.hpp>
#include "../Helpers/DRSprite.hpp"
#include "../Helpers/DifficultyMaps.hpp"

using namespace geode::prelude;

class $modify(LevelPageHook, LevelPage) {
	void updateDynamicPage(GJGameLevel * level) {
		LevelPage::updateDynamicPage(level);

		if (CCSprite* child = static_cast<CCSprite*>(this->getChildByIDRecursive("difficulty-sprite"))) {
			DRSprite* face = static_cast<DRSprite*>(child->getParent()->getChildByID("DR-difficulty-face"));

			// If level ID is out-of-bounds - returns everything back to normal.
			if ((level->m_levelID < 0 || level->m_levelID - 1 >= std::size(originalLevelsToStars))) {
				if (face) {
					face->removeFromParent();
					child->setOpacity(255);
				}
				return;
			}

			// If custom face is already created - changes it, otherwise creates a new one.
			if (face) {
				face->setDisplayFrame(DRSprite::createFromStars(originalLevelsToStars[level->m_levelID - 1], None)->displayFrame());
			}
			else {
				face = DRSprite::createFromStars(originalLevelsToStars[level->m_levelID - 1], None);
				face->setID("DR-difficulty-face");
				face->setZOrder(child->getZOrder());
				child->setOpacity(0);
				child->getParent()->addChild(face);
			}
			face->setPosition(child->getPosition());
		}
	}
};