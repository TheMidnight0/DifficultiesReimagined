#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DRSprite : public CCSprite {
protected:
    int m_stars = 0;
    bool m_isDemon = false;

public:
    static DRSprite* createFromLevel(GJGameLevel* level);

    static DRSprite* createFromStars(int stars);

protected:
    bool init() override;
};