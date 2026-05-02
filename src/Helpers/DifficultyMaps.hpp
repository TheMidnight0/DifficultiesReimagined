#pragma once

constexpr inline std::array<int, 6> averageDiffToStars = {
	0, 2, 3, 5, 7, 9
};

constexpr inline std::array<int, 6> originalLevelsToStars = {
	2, 2, 3, 4, 4, 5
};

constexpr inline std::array<int, 10> starsToOriginalDiff = {
	0, -3, 1, 2, 3, 3, 4, 4, 5, 5
};

constexpr inline std::array<const char*, 13> originalDiffToCustom = {
	"DR_difficulty0"_spr,
	"DR_difficulty1"_spr,
	"DR_difficulty2"_spr,
	"DR_difficulty3"_spr,
	"DR_difficulty5"_spr,
	"DR_difficulty7"_spr,
	"DR_difficulty9"_spr,
	"DR_difficulty11"_spr,
	"DR_difficulty12"_spr,
	"DR_difficulty13"_spr,
	"DR_difficulty14"_spr,
	"DR_difficulty15"_spr
};