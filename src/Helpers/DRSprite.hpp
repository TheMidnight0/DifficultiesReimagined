#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;


enum DifficultyName {
    None,
    Short,
    Long
};

class DRSprite : public CCSprite {
protected:
    int m_stars = 0;
    bool m_isDemon = false;

public:
    static DRSprite* createFromLevel(GJGameLevel* level, DifficultyName name);

    static DRSprite* createFromStars(int stars, DifficultyName name);

    static DRSprite* createFromDifficulty(int difficulty, DifficultyName name);

protected:
    bool init() override;
};