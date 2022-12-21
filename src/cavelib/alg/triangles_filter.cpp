#include "triangles_filter.h"

#include <vector>
#include <map>

#include "vec3.h"
#include "jarvis_ex.h"

using namespace std;

struct AvgInfo {
	double len;
	int count;
};

void appendLine(map<int, AvgInfo>& avgMap, vector<Vec3>& pts, int v1, int v2) {
	double len = (pts[v1] - pts[v2]).length2d();

	auto it = avgMap.find(v1);
	if (it != avgMap.end()) {
		AvgInfo& avg = it->second;
		avg.len += len;
		avg.count += 1;

		return;
	}

	AvgInfo avg;
	avg.len = len;
	avg.count = 1;

	avgMap[v1] = avg;
}

map<int, AvgInfo> buildDistances(vector<int>& triangles, vector<Vec3>& pts) {
	map<int, AvgInfo> d;

	for (int i = triangles.size() - 1; i > 0; i -= 3) {
		int v1 = triangles[i];
		int v2 = triangles[i - 1];
		int v3 = triangles[i - 2];

		appendLine(d, pts, v1, v2);
		appendLine(d, pts, v1, v3);

		appendLine(d, pts, v2, v1);
		appendLine(d, pts, v2, v3);

		appendLine(d, pts, v3, v2);
		appendLine(d, pts, v3, v1);
	}

	for (auto& kv : d) {
		AvgInfo& avg = kv.second;
		avg.len /= avg.count;
	}

	return d;
}

bool checkLine(map<int, AvgInfo>& avgMap, vector<Vec3>& pts, int v1, int v2, double hardness) {
	double len = (pts[v1] - pts[v2]).length2d();
	double dst = (avgMap[v1].len + avgMap[v2].len) * hardness;

	return dst < len;
}

void filterTriangles(vector<int>& triangles, vector<Vec3>& pts, double hardness) {
	if (triangles.size() == 0) {
		return;
	}

	bool runAgain;

	do {
		runAgain = false;

		vector<int> jarvis = jarvisEx(triangles, pts);
		map<int, int> borderMap;

		for (unsigned int i = 0; i < jarvis.size(); i++) {
			borderMap[jarvis[i]] = i;
		}

		map<int, AvgInfo> avgMap = buildDistances(triangles, pts);

		for (int i = triangles.size() - 1; i >= 0; i -= 3) {
			int v1 = triangles[i];
			int v2 = triangles[i - 1];
			int v3 = triangles[i - 2];

			int b1 = borderMap.find(v1) != borderMap.end();
			int b2 = borderMap.find(v2) != borderMap.end();
			int b3 = borderMap.find(v3) != borderMap.end();

			if (b1 + b2 + b3 != 2) {
				continue;
			}

			int u;
			int y;
			if (b1 && b2) {
				u = borderMap[v1];
				y = borderMap[v2];
			}
			else if (b2 && b3) {
				y = borderMap[v2];
				u = borderMap[v3];
			}
			else {
				u = borderMap[v1];
				y = borderMap[v3];
			}

			if (abs((y - u) % (int)jarvis.size()) != 1) {
				continue;
			}

			if (checkLine(avgMap, pts, v1, v2, hardness) || checkLine(avgMap, pts, v2, v3, hardness) || checkLine(avgMap, pts, v3, v1, hardness)) {
				triangles.erase(next(triangles.begin(), i));
				triangles.erase(next(triangles.begin(), i - 1));
				triangles.erase(next(triangles.begin(), i - 2));

				runAgain = true;
			}
		}
	} while (runAgain);
}