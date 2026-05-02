#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include "../../Helpers/DRSprite.hpp"
#include "../../Helpers/DifficultyMaps.hpp"
#include "SearchSystem.hpp"

using namespace geode::prelude;
using namespace SearchSystem;

class $modify(LevelSearchLayerHook, LevelSearchLayer) {
    struct Fields {
        std::vector<CCMenuItemToggler*> customToggles = {};
        bool initedAlready = false;
    };

    bool init(int type) {
        if (!LevelSearchLayer::init(type)) return false;

        if (!m_fields->initedAlready) {
            CCMenu* menu = static_cast<CCMenu*>(this->getChildByID("difficulty-filter-menu"));

            for (int i = 0; i < menu->getChildrenCount(); i++) {
                CCMenuItemSpriteExtra* child = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByIndex(i));
                if (child) {
                    child->setEnabled(false);
                    child->setVisible(false);
                }
            }

            for (int i : {0, 2, 3, 4, 5, 6, 7, 8, 9, 1}) {
                DRSprite* on = DRSprite::createFromStars(i, Short);
                DRSprite* off = DRSprite::createFromStars(i, Short);
                off->setColor({ 128, 128, 128 });

                auto toggler = CCMenuItemToggler::create(
                    off, on, this, menu_selector(LevelSearchLayerHook::onCustomFilterClicked)
                );

                toggler->setTag(i);
                toggler->setScale(0.8f);
                toggler->toggle(std::find(filters.begin(), filters.end(), i) != filters.end());
                menu->addChild(toggler);
                m_fields->customToggles.push_back(toggler);
            }

            RowLayout* layout = RowLayout::create();
            layout->setGap(0.5f);
            layout->setAutoScale(false);
            menu->setPositionY(83);
            menu->setLayout(layout);
            menu->updateLayout();

            m_fields->initedAlready = true;
        }

        return true;
    }

    void onCustomFilterClicked(CCObject * sender) {
        auto clickedToggler = static_cast<CCMenuItemToggler*>(sender);
        int stars = clickedToggler->getTag();

        if (!clickedToggler->isToggled()) {
            if (stars == 0 || stars == 1 || stars == 10) {
                SearchSystem::filters.clear();
                for (auto toggler : m_fields->customToggles) {
                    if (toggler != clickedToggler) {
                        toggler->toggle(false);
                        toggler->m_toggled = false;
                        toggler->updateSprite();
                    }
                }
            }
            SearchSystem::filters.push_back(stars);
        }
        else {
            auto target = std::find(SearchSystem::filters.begin(), SearchSystem::filters.end(), stars);
            if (target != SearchSystem::filters.end()) SearchSystem::filters.erase(target);
        }
    }

    GJSearchObject* getSearchObject(SearchType searchType, gd::string searchQuery) {
        auto searchObj = LevelSearchLayer::getSearchObject(searchType, searchQuery);

        if (searchObj && filters.size() > 0) {
            std::string searchString = "";
            for (int i = 0; i < filters.size(); i++) {
                searchString += std::to_string(starsToOriginalDiff[filters[i]]);
                if (i + 1 < filters.size()) searchString += ",";
            }
            searchObj->m_difficulty = searchString;
            searchObj->m_starFilter = true;
        }

        return searchObj;
    }
};