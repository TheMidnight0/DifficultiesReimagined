//#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <string>
#include "SearchSystem.hpp"

using namespace SearchSystem;
using namespace geode::prelude;

class $modify(LevelBrowserLayerHook, LevelBrowserLayer) {
    struct Fields {
        TaskHolder<web::WebResponse> m_listener;
    };

    bool init(GJSearchObject * object) {
       if (object->isLevelSearchObject() &&
           std::find(allowedSearchTypes.begin(), allowedSearchTypes.end(), object->m_searchType) != allowedSearchTypes.end()) {
           log::debug("Init invoked: data cleared");
           queue.clear();
           awaitedKey = "";
           back = { {-1, 0}, {0, 0} };
           SearchSystem::isRunning = false;
           currentPage = 0;
           state = "idle";
           totalLevelCount = 0;
           lastPage = 9999;

           activated = true;
       }
       else activated = false;

        if (!LevelBrowserLayer::init(object)) return false;


        return true;
    }

    void setupPageInfo(gd::string info, char const* key) {
       if (activated) {
           if (totalLevelCount == 0) {
               size_t pos = info.find(':');
               if (pos != gd::string::npos) {
                   info = info.substr(0, pos);
                   totalLevelCount = std::stoi(info);
                   lastPage = static_cast<int>(std::ceil(totalLevelCount / 10.0));
               }
           }
           return;
       }
       else LevelBrowserLayer::setupPageInfo(info, key);
    }

    void loadLevelsFinished(CCArray* levels, const char* key, int type) {
        if (activated) {
           GJSearchObject* object = GJSearchObject::createFromKey(key);
           object->retain();

           CCArray* newArr = CCArray::create();
           newArr->retain();
           newArr->addObjectsFromArray(levels);

           if (awaitedKey == key) {
               awaitedKey = "";
               queue.push_front({ newArr, object, type, true });
               searchManager(this);
           }
           else {
               queue.push_back({ newArr, object, type, true });
               if (!SearchSystem::isRunning && awaitedKey == "") {
                   searchManager(this);
               }
           }
        }
        else {
           LevelBrowserLayer::loadLevelsFinished(levels, key, type);
        }
    }

    void loadLevelsFailed(char const* key, int type) {
       if (activated) {
           GJSearchObject* object = GJSearchObject::createFromKey(key);
           object->retain();

           if (awaitedKey == key) {
               awaitedKey = "";
               queue.push_front({ nullptr, object, type, false });
               searchManager(this);
           }
           else {
               queue.push_back({ nullptr, object, type, false });
               if (!SearchSystem::isRunning && awaitedKey == "") {
                   searchManager(this);
               }
           }
       }
       else {
           LevelBrowserLayer::loadLevelsFailed(key, type);
       }
    }
};