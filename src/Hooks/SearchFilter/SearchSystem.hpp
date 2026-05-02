#pragma once

#include <Geode/Geode.hpp>
#include <deque>
#include <Geode/cocos/cocoa/CCArray.h>

using namespace geode::prelude;

namespace SearchSystem {
   struct Anchor {
       struct Pointer {
           int page;
           int offset;
       };

       Pointer back;
       Pointer front;
   };

   struct SearchRequest {
       CCArray* levels;
       GJSearchObject* searchObj;
       int type;
       bool succesful;
   };

   // Selected difficulty filters.
   extern std::vector<int> filters;

   inline std::array<SearchType, 9> allowedSearchTypes = {
       SearchType::MostLiked, SearchType::Downloaded, SearchType::Sent, 
       SearchType::Trending, SearchType::Recent, SearchType::Magic, 
       SearchType::Awarded, SearchType::Followed, SearchType::Friends
   };

   // Back anchor. Used when going to the previous page.
   extern Anchor back;

   // Front anchor. Used when going to the next page.
   extern Anchor front;

   // Current anchor. Used when processing requests.
   extern Anchor current;


   extern int currentPage;
   extern int totalLevelCount;
   extern int lastPage;
   extern CCArray* heldLevels;

   extern std::string awaitedKey;
   extern std::deque<SearchRequest> queue;

   extern const char* state;
   extern bool isRunning;
   extern bool activated;

   void searchManager(LevelBrowserLayer* manager);
}