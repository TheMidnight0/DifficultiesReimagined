#include <Geode/Geode.hpp>
#include "SearchSystem.hpp"
#include <deque>

namespace SearchSystem {
   std::vector<int> filters = {};
   Anchor back = { {0, 0}, {0, 0} };
   Anchor front = { {0, 0}, {0, 0} };
   Anchor current = { {0, 0}, {0, 0} };
   int currentPage = 0;
   CCArray* heldLevels = nullptr;
   std::string awaitedKey = "";
   std::deque<SearchRequest> queue = {};
   const char* state = "idle";
   int totalLevelCount = 0;
   int lastPage = 0;
   bool isRunning = false;
   bool activated = false;

   std::string anchorToStr(Anchor anch) {
       return fmt::format("{{ b: {{{}, {}}}, f: {{{}, {}}} }}", 
           anch.back.page, anch.back.offset, anch.front.page, anch.front.offset);
   }

   // Filters array of levels by difficulty filters.
   void filterLevels(CCArray* levels) {
       int i = 0;
       while (i < levels->count()) {
           GJGameLevel* level = static_cast<GJGameLevel*>(levels->objectAtIndex(i));

           if (std::find(filters.begin(), filters.end(), static_cast<int>(level->m_stars)) == filters.end()) {
               levels->removeObjectAtIndex(i);
           }
           else i++;
       }
   }

   // Sends request to get the targetPage and sets browser to the originalPage.
   void sendRequest(LevelBrowserLayer* manager, SearchRequest request, int page) {
       request.searchObj->retain();
       request.searchObj->m_page = page;

       std::thread([manager, request]() {
           GameLevelManager* glm = GameLevelManager::sharedState();
           glm->m_levelManagerDelegate = static_cast<LevelManagerDelegate*>(manager);
           CCArray* levels = glm->getStoredOnlineLevels(request.searchObj->getKey());

           if (levels && levels->count() == 10) {
               log::debug("Got {} stored levels!", levels->count());

               Loader::get()->queueInMainThread([manager, levels, request]() {
                   manager->loadLevelsFinished(levels, request.searchObj->getKey(), request.type);
               });
           }
           else {
               log::debug("Levels are not stored. Making a request...");

               std::this_thread::sleep_for(std::chrono::milliseconds(500));
               Loader::get()->queueInMainThread([manager, glm, request]() {
                   awaitedKey = request.searchObj->getKey();
                   glm->getOnlineLevels(request.searchObj);
               });
           }
       }).detach();
   }

   // Positive offset trims first objects of an array, negative offset - last objects.
   void offsetLevels(CCArray* levels, int offset) {
       while (offset != 0 && levels->count() != 0) {
           if (offset > 0) {
               levels->removeFirstObject();
               offset--;
           }
           else {
               levels->removeLastObject();
               offset++;
           }
       }
   }

   void manageUI(LevelBrowserLayer* manager, bool endOfRequest = false) {
       log::debug("Refresh btn: {}", manager->m_refreshBtn);
       if (!endOfRequest) {
           manager->m_countText->setVisible(false);
           manager->m_pageBtn->setEnabled(false);
           manager->m_pageBtn->setVisible(false);
           manager->m_leftArrow->setEnabled(false);
           manager->m_leftArrow->setVisible(false);
           manager->m_rightArrow->setEnabled(false);
           manager->m_rightArrow->setVisible(false);
           manager->m_refreshBtn->setVisible(false);
           manager->m_refreshBtn->setEnabled(false);
       }
       else {
           int startPage = back.front.page + 1;
           int endPage = front.back.page + 1;

           if (startPage != endPage) manager->m_pageText->setString(fmt::format("{}-{}", startPage, endPage).c_str());
           else manager->m_pageText->setString(std::to_string(startPage).c_str());
           manager->m_pageBtn->setEnabled(true);
           manager->m_pageBtn->setVisible(true);
           manager->m_pageBtn->setContentWidth(std::max(40.0f, manager->m_pageText->getContentWidth() / 2));

           manager->m_countText->setString(fmt::format("{} to {} of {}", (startPage - 1) * 10 + 1, endPage * 10, totalLevelCount).c_str());
           manager->m_countText->setVisible(true);

           manager->m_pageBtn->setEnabled(true);
           manager->m_pageBtn->setVisible(true);
           manager->m_refreshBtn->setVisible(true);
           manager->m_refreshBtn->setEnabled(true);

           if (startPage != 1) {
               manager->m_leftArrow->setEnabled(true);
               manager->m_leftArrow->setVisible(true);
           }
           
           if (endPage != lastPage) {
               manager->m_rightArrow->setEnabled(true);
               manager->m_rightArrow->setVisible(true);
           }
       }
   }

   // Manages core logic of search system.
   void manageSuccesfulRequest(LevelBrowserLayer* manager, SearchRequest request) {
       if (state == "idle") manageUI(manager, false);

       if (filters.size() > 0) {
           if (state == "idle") {
               int requestPage = request.searchObj->m_page;
               if (heldLevels) heldLevels->release();
               heldLevels = CCArray::create();
               heldLevels->retain();
               log::debug("Search proccess started!");
               log::debug("request page: {}", requestPage);
               log::debug("current page: {}", currentPage);
               log::debug("manager page: {}", manager->m_searchObject->m_page);

               if (requestPage == currentPage) {
                   state = "repeat_search";
                   current = back;
                   log::debug("Type: repeat search");
               }
               else if (requestPage == currentPage + 1) {
                   state = "forward_search";
                   current = front;
                   log::debug("Type: forward search");
               }
               else if (requestPage == currentPage - 1) {
                   state = "backward_search";
                   current = back;
                   log::debug("Type: backward search");
                   
                   sendRequest(manager, request, current.back.page);
                   log::debug("Sended request to current.back.page: {}", current.back.page);
                   return;
               }
               else {
                   current = back = { { requestPage - 1, 0 }, { requestPage, 0 } };
                   state = "repeat_search";
                   log::debug("Type: reset search");
               }
               manager->m_searchObject->m_page = current.front.page;
               currentPage = current.front.page;
               log::debug("searchObj page: {}", manager->m_searchObject->m_page);
               log::debug("Current page: {}", currentPage);
               log::debug("Front anchor: {}", anchorToStr(front));
               log::debug("Back anchor: {}", anchorToStr(back));
               log::debug("Current anchor: {}", anchorToStr(current));
               
               sendRequest(manager, request, current.front.page);
               log::debug("Sended request to current.front.page: {}", current.front.page);
               return;
           }

           filterLevels(request.levels);
           log::debug("Array filtered, size: {}", request.levels->count());

           if (state == "backward_search") {
               if (current.back.offset > 0) {
                   int offset = -std::min(current.back.offset, (int)request.levels->count());
                   offsetLevels(request.levels, offset);
                   current.back.offset += offset;
                   log::debug("Array offsetted, offset: {}", offset);
                   log::debug("Current anchor: {}", anchorToStr(current));
               }

               int count = request.levels->count();
               int sharedCount = heldLevels->count() + count;
               if (sharedCount > 10) offsetLevels(request.levels, sharedCount - 10);
               log::debug("Array size: {}, shared size: {}", count, sharedCount);

               request.levels->addObjectsFromArray(heldLevels);
               heldLevels->release();
               heldLevels = request.levels;
               heldLevels->retain();

               if (sharedCount >= 10 || current.back.page == 0) {
                   front = back;
                   int leftover = sharedCount - 10;
                   back = {
                       { (leftover == 0 ? -1 : 0) + current.back.page, 
                       (leftover == 0 ? -count : 0) + count - leftover },
                       { (leftover == count ? 1 : 0) + current.back.page, 
                       (leftover == count ? -leftover : 0) + leftover }
                   };
                   log::debug("Front anchor: {}", anchorToStr(front));
                   log::debug("Back anchor: {}", anchorToStr(back));
                   manager->m_searchObject->m_page = back.front.page;
                   currentPage = back.front.page;
                   log::debug("searchObj page: {}", manager->m_searchObject->m_page);
                   log::debug("Current page: {}", currentPage);
                   
                   manageUI(manager, true);
                   manager->loadLevelsFinished(heldLevels, manager->m_searchObject->getKey(), request.type);
                   state = "idle";
               }
               else {
                   sendRequest(manager, request, --current.back.page);
                   log::debug("new request to current.back.page: {}", current.back.page);
               }
           }
           if (state == "forward_search" || state == "repeat_search") {
               if (current.front.offset > 0) {
                   int offset = std::min(current.front.offset, (int)request.levels->count());
                   offsetLevels(request.levels, offset);
                   current.front.offset -= offset;
                   log::debug("Array offsetted, offset: {}", offset);
                   log::debug("Current anchor: {}", anchorToStr(current));
               }

               int count = request.levels->count();
               int sharedCount = heldLevels->count() + count;
               if (sharedCount > 10) offsetLevels(request.levels, 10 - sharedCount);
               heldLevels->addObjectsFromArray(request.levels);
               log::debug("Array size: {}, shared size: {}", count, sharedCount);

               if (sharedCount >= 10 || current.front.page == lastPage) {
                   if (state == "forward_search") {
                       back = front;
                   }
                   int leftover = sharedCount - 10;
                   front = { 
                       { (leftover == count ? -1 : 0) + current.front.page, 
                       (leftover == count ? -leftover : 0) + leftover },
                       { (leftover == 0 ? 1 : 0) + current.front.page, 
                       (leftover == 0 ? -count : 0) + count - leftover }
                   };
                   log::debug("Front anchor: {}", anchorToStr(front));
                   log::debug("Back anchor: {}", anchorToStr(back));
                   manager->m_searchObject->m_page = back.front.page;
                   currentPage = back.front.page;
                   log::debug("searchObj page: {}", manager->m_searchObject->m_page);
                   log::debug("Current page: {}", currentPage);
                   
                   manageUI(manager, true);
                   manager->loadLevelsFinished(heldLevels, manager->m_searchObject->getKey(), request.type);
                   state = "idle";
               }
               else {
                   sendRequest(manager, request, ++current.front.page);
                   log::debug("new request to current.front.page: {}", current.front.page);
               }
           }
           return;
       }
       manageUI(manager, true);
       manager->loadLevelsFinished(request.levels, manager->m_searchObject->getKey(), request.type);
   }

   void manageFailedRequest(LevelBrowserLayer* manager, SearchRequest request) {
       manager->loadLevelsFailed(manager->m_searchObject->getKey(), request.type);
   }

   void queueRequest(LevelBrowserLayer* manager, SearchRequest request) {
       if (request.succesful) {
           manageSuccesfulRequest(manager, request);
       }
       else {
           manageFailedRequest(manager, request);
       }
   }

   void searchManager(LevelBrowserLayer* manager) {
       isRunning = true;
       while (queue.size() > 0 && awaitedKey == "") {
           SearchRequest request = queue.front();

           queueRequest(manager, request);
           if (request.searchObj) request.searchObj->release();
           if (request.levels) request.levels->release();

           queue.pop_front();
       }
       isRunning = false;
   }
}