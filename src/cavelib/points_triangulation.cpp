#include "points_triangulation.h"

#include "alg/vec3.h"
#include "alg/delone.h"
#include "alg/jarvis_ex.h"
#include "alg/triangles_filter.h"
#include "alg/hull_connector.h"

#include <vector>

using namespace std;

vector<int> castIndexes(vector<int>& trianglesRaw, vector<int>& map) {
	vector<int> n;

	for (unsigned int i = 0; i < trianglesRaw.size(); i++) {
		n.push_back(map[trianglesRaw[i]]);
	}

	return n;
}

void append(vector<CTriangle>& triangles, vector<int>& trianglesRaw, int type) {
	for (unsigned int i = 0; i < trianglesRaw.size(); i += 3) {
		CTriangle t;
		t.v1 = trianglesRaw[i];
		t.v2 = trianglesRaw[i + 1];
		t.v3 = trianglesRaw[i + 2];
		t.type = type;

		triangles.push_back(t);
	}
}

vector<CTriangle> triangulateCavePoints(vector<CPoint>& points, double triangFilterHardness) {
	vector<int> upMap;
	vector<int> floorMap;

	vector<Vec3> pts;

	vector<Vec3> up;
	vector<Vec3> floor;

	for (unsigned int i = 0; i < points.size(); ++i) {
		CPoint& v = points[i];

		if (v.isFloor) {
			floor.push_back(v.pos);
			floorMap.push_back(i);
		}
		else {
			up.push_back(v.pos);
			upMap.push_back(i);
		}

		pts.push_back(v.pos);
	}

	vector<int> triFloorRaw = triangulate(floor);
	vector<int> triUpRaw = triangulate(up);

	vector<int> triFloor = castIndexes(triFloorRaw, floorMap);
	vector<int> triUp = castIndexes(triUpRaw, upMap);

	filterTriangles(triFloor, pts, triangFilterHardness);
	filterTriangles(triUp, pts, triangFilterHardness);

	vector<int> floorHull = jarvisEx(triFloor, pts);
	vector<int> upHull = jarvisEx(triUp, pts);

	vector<int> triWl = connectHulls(floorHull, upHull, pts);

	vector<CTriangle> triangles;

	append(triangles, triFloor, T_FLOOR);
	append(triangles, triWl, T_WALL);
	append(triangles, triUp, T_UP);

	return triangles;
}