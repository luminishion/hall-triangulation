#include "hull_connector.h"

#include <vector>
#include <map>

#include "vec3.h"

using namespace std;

vector<Vec3> shift(vector<int>& hull, vector<Vec3>& pts, double dst) {
	vector<Vec3> p;

	for (unsigned int i = 0; i < hull.size(); i++) {
		int v1 = i != 0 ? hull[i - 1] : hull[hull.size() - 1];
		int v2 = hull[i];
		int v3 = i != (hull.size() - 1) ? hull[i + 1] : hull[0];

		Vec3 p3 = pts[v3];
		Vec3 p1 = pts[v1];
		Vec3 p2 = pts[v2];

		p1.z = 0;
		p2.z = 0;
		p3.z = 0;

		Vec3 t1 = (p2 - p1).normalized();
		Vec3 t2 = (p3 - p2).normalized();

		Vec3 d1 = Vec3(-t1.y, t1.x, 0);
		Vec3 d2 = Vec3(-t2.y, t2.x, 0);

		p.push_back(p2 + (d1 + d2).normalized() * dst);
	}

	return p;
}

int areIntersecting(
	double v1x1, double v1y1, double v1x2, double v1y2,
	double v2x1, double v2y1, double v2x2, double v2y2
) {
	double d1, d2;
	double a1, a2, b1, b2, c1, c2;

	a1 = v1y2 - v1y1;
	b1 = v1x1 - v1x2;
	c1 = (v1x2 * v1y1) - (v1x1 * v1y2);

	d1 = (a1 * v2x1) + (b1 * v2y1) + c1;
	d2 = (a1 * v2x2) + (b1 * v2y2) + c1;

	if (d1 > 0 && d2 > 0) return 0;
	if (d1 < 0 && d2 < 0) return 0;

	a2 = v2y2 - v2y1;
	b2 = v2x1 - v2x2;
	c2 = (v2x2 * v2y1) - (v2x1 * v2y2);

	d1 = (a2 * v1x1) + (b2 * v1y1) + c2;
	d2 = (a2 * v1x2) + (b2 * v1y2) + c2;

	if (d1 > 0 && d2 > 0) return 0;
	if (d1 < 0 && d2 < 0) return 0;

	if ((a1 * b2) - (a2 * b1) == 0.0f) return 2;

	return 1;
}

bool checkHull(vector<int>& hull, vector<Vec3>& pts, Vec3 p1, Vec3 p2) {
	vector<Vec3> shiftedPts = shift(hull, pts, 1);

	for (unsigned int i = 0; i < shiftedPts.size(); i++) {
		Vec3 v1 = shiftedPts[i];
		Vec3 v2 = i != (shiftedPts.size() - 1) ? shiftedPts[i + 1] : shiftedPts[0];

		if (areIntersecting(
			v1.x, v1.y, v2.x, v2.y,
			p1.x, p1.y, p2.x, p2.y
		) != 0) {
			return true;
		}
	}

	return false;
}

bool isConstOk(int fromId, int toId, vector<Vec3>& pts, vector<int>& fromHull, vector<int>& toHull) {
	Vec3 fromP = pts[fromHull[fromId]];
	Vec3 toP = pts[toHull[toId]];

	return !checkHull(fromHull, pts, fromP, toP) || !checkHull(toHull, pts, fromP, toP);
}

int nextId(unsigned int id, vector<int>& hull) {
	id += 1;

	return id >= hull.size() ? 0 : id;
}

void checkCond1(vector<int>& triangles, int& pId1, int pId2, vector<int>& hull1, vector<int>& hull2, vector<Vec3>& pts, int bannedPId, bool& changed) {
	if (pId1 == bannedPId) {
		return;
	}

	int n1 = nextId(pId1, hull1);

	if (!isConstOk(n1, pId2, pts, hull1, hull2)) {
		return;
	}

	changed = true;

	triangles.push_back(hull1[pId1]);
	triangles.push_back(hull2[pId2]);
	triangles.push_back(hull1[n1]);

	pId1 = n1;
}

void checkCond2(vector<int>& triangles, int pId1, int& pId2, vector<int>& hull1, vector<int>& hull2, vector<Vec3>& pts, int bannedPId, bool& changed) {
	if (pId2 == bannedPId) {
		return;
	}

	int curId = pId2;
	curId = nextId(curId, hull2);

	int backupId = curId;

	vector<int> queue;

	while (true) {
		if (curId == bannedPId) {
			break;
		}

		curId = nextId(curId, hull2);
		queue.push_back(curId);

		if (checkHull(hull2, pts, pts[hull2[pId2]], pts[hull2[curId]])) {
			break;
		}

		if (isConstOk(curId, pId1, pts, hull2, hull1)) {
			for (unsigned int i = 0; i < queue.size(); i++) {
				triangles.push_back(hull2[pId2]);
				triangles.push_back(hull2[backupId]);
				triangles.push_back(hull2[queue[i]]);
				backupId = queue[i];
			}


			triangles.push_back(hull2[pId2]);
			triangles.push_back(hull2[curId]);
			triangles.push_back(hull1[pId1]);
			pId2 = curId;
			changed = true;

			break;
		}
	}
}

vector<int> connectHulls(vector<int>& hull1, vector<int>& hull2, vector<Vec3>& pts) {
	vector<int> tri;

	if (hull1.size() < 3 || hull2.size() < 3) {
		return tri;
	}

	int p1 = 0;
	int p2 = 0;

	for (unsigned int i = 0; i < hull1.size(); i++) {
		if (isConstOk(p1, i, pts, hull1, hull2)) {
			p2 = i;
			break;
		}
	}

	int b1 = -1;
	int b2 = -1;

	int c1 = p1;
	int c2 = p2;

	while (true) {
		bool changed1 = false;
		bool changed2 = false;
		bool changed = false;

		checkCond1(tri, p1, p2, hull1, hull2, pts, b1, changed1);
		checkCond1(tri, p2, p1, hull2, hull1, pts, b2, changed2);

		if (!changed1 && !changed2) {
			checkCond2(tri, p1, p2, hull1, hull2, pts, b2, changed);
			checkCond2(tri, p2, p1, hull2, hull1, pts, b1, changed);
		}

		if (!changed && !changed1 && !changed2) {
			break;
		}

		if (changed1) {
			b1 = c1;
		}

		if (changed2) {
			b2 = c2;
		}
	}

	return tri;
}