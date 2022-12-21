#include "borders.h"

#include <vector>

#include "alg/delone.h"
#include "alg/triangles_filter.h"
#include "alg/jarvis_ex.h"
#include "alg/vec3.h"

using namespace std;

vector<Vec3> getCaveBorders(vector<Vec3> pts, double hardness) {
	vector<int> triangles = triangulate(pts);
	filterTriangles(triangles, pts, hardness);
	vector<int> jarvis = jarvisEx(triangles, pts);

	vector<Vec3> ret;

	for (unsigned int i = 0; i < jarvis.size(); i++) {
		Vec3 v = pts[jarvis[i]];
		ret.push_back(Vec3(v.x, v.y, 0));
	}

	return ret;
}