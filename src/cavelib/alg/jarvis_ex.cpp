#include "jarvis_ex.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <map>

#include "vec3.h"

using namespace std;

double getAng(Vec3 v) {
	double x = v.x;
	double y = v.y;

	double x2 = x * x;
	double y2 = y * y;

	double dot1 = x / sqrt(x2 + y2);
	double dot2 = y / sqrt(y2 + x2);

	if (dot2 < 0) {
		return 2 * M_PI - acos(dot1);
	}
	else {
		return acos(dot1);
	}
}

void add(map<int, vector<int>>& points, int v1, int v2) {
	if (points.find(v1) == points.end()) {
		vector<int> v;
		points[v1] = v;
	}

	points[v1].push_back(v2);
}

vector<int> jarvisEx(vector<int>& triangles, vector<Vec3>& pts) {
	vector<int> hull;

	if (triangles.size() == 0) {
		return hull;
	}

	map<int, vector<int>> points;

	for (unsigned int i = 0; i < triangles.size(); i += 3) {
		int v1 = triangles[i];
		int v2 = triangles[i + 1];
		int v3 = triangles[i + 2];

		add(points, v1, v2);
		add(points, v1, v3);

		add(points, v2, v1);
		add(points, v2, v3);

		add(points, v3, v1);
		add(points, v3, v2);
	}

	int leftIndex = -1;
	for (const auto& kv : points) {
		int k = kv.first;

		if (leftIndex == -1 || pts[k].x < pts[leftIndex].x) {
			leftIndex = k;
		}
	}

	int backupLeft = leftIndex;
	double pAngle = getAng(Vec3(-1, 0, 0));
	int id = -1;

	do {
		int q = -1;
		double qa = 7;

		vector<int> r = points[backupLeft];
		for (unsigned int i = 0; i < r.size(); i++) {
			int v = r[i];

			if (id == v) {
				continue;
			}

			double ang = getAng(pts[v] - pts[backupLeft]);
			ang = (ang - pAngle);

			while (ang < 0.0) {
				ang += 2 * M_PI;
			}
			while (ang >= 2 * M_PI) {
				ang -= 2 * M_PI;
			}


			if (ang < qa) {
				qa = ang;
				q = v;
			}
		}

		hull.push_back(backupLeft);
		id = backupLeft;
		backupLeft = q;
		pAngle = getAng(pts[id] - pts[backupLeft]);

		if (q == leftIndex)
			break;
	} while (true);

	return hull;
}