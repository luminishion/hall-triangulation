#pragma once

#include <vector>

#include "alg/vec3.h"

struct CPoint {
	Vec3 pos;
	bool isFloor = false;
};

enum {
	T_FLOOR,
	T_WALL,
	T_UP
};

struct CTriangle {
	int v1;
	int v2;
	int v3;
	int type;
};

std::vector<CTriangle> triangulateCavePoints(std::vector<CPoint>& points, double triangFilterHardness);