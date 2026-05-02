#include "DRSprite.hpp"
#include "DifficultyMaps.hpp"

DRSprite* DRSprite::createFromLevel(GJGameLevel* level, DifficultyName name) {
    DRSprite* ret = new DRSprite();

    if (ret) {
        bool useReq = Mod::get()->getSettingValue<bool>("use-req-difficulty");
        bool alwaysUseReq = Mod::get()->getSettingValue<bool>("always-use-req-difficulty");

        int stars = level->m_stars;
        if (stars == 0) {
            if (useReq) stars = level->m_starsRequested;
            else stars = averageDiffToStars[level->getAverageDifficulty()];
        }
        else if (alwaysUseReq) stars = level->m_starsRequested;
        //if (stars == 10 && level->m_stars > 0); These should be demon switch in the future.

        if (ret->initWithSpriteFrameName(fmt::format("DR_difficulty{}_short.png"_spr, stars).c_str())) {
            ret->autorelease();
            return ret;
        }
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

DRSprite* DRSprite::createFromStars(int stars, DifficultyName name) {
    DRSprite* ret = new DRSprite();

    if (ret) {
        std::string spriteName = "DR_difficulty"_spr + std::to_string(stars);
        switch (name) {
        case None:
            spriteName += ".png";
            break;
        case Short:
            spriteName += "_short.png";
            break;
        case Long:
            spriteName += "_long.png";
            break;
        }
        if (ret->initWithSpriteFrameName(spriteName.c_str())) {
            ret->autorelease();
            return ret;
        }
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

DRSprite* DRSprite::createFromDifficulty(int difficulty, DifficultyName name) {
    DRSprite* ret = new DRSprite();

    if (ret) {
        std::string spriteName = originalDiffToCustom[difficulty + 1];
        switch (name) {
        case None:
            spriteName += ".png";
            break;
        case Short:
            spriteName += "_short.png";
            break;
        case Long:
            spriteName += "_long.png";
            break;
        }
        if (ret->initWithSpriteFrameName(spriteName.c_str())) {
            ret->autorelease();
            return ret;
        }
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool DRSprite::init() {
    return CCSprite::init();
}