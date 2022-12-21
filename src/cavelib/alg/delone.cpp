#include "delone.h"

#include "vec3.h"

#include <vector>
#include <algorithm>

using namespace std;

#define EPS 1e-7

struct CircTri {
	int a;
	int b;
	int c;
	double x;
	double y;
	double r;
};

void firstTriangle(vector<Vec3>& points) {
	double minx = 10000000;
	double maxx = -10000000;
	double miny = 10000000;
	double maxy = -10000000;

	for (unsigned int i = 0; i < points.size(); i++) {
		minx = min(minx, points[i].x);
		miny = min(miny, points[i].y);
		maxx = max(maxx, points[i].x);
		maxy = max(maxy, points[i].y);
	}

	double dx = maxx - minx;
	double dy = maxy - miny;

	double dxy = max(dx, dy);

	double midx = dx * 0.5 + minx;
	double midy = dy * 0.5 + miny;

	points.push_back(Vec3(midx - 10 * dxy, midy - 10 * dxy, 0));
	points.push_back(Vec3(midx, midy + 10 * dxy, 0));
	points.push_back(Vec3(midx + 10 * dxy, midy - 10 * dxy, 0));
}

CircTri deloneCircle(vector<Vec3>& points, int v1, int v2, int v3) {
	double x1 = points[v1].x;
	double y1 = points[v1].y;
	double x2 = points[v2].x;
	double y2 = points[v2].y;
	double x3 = points[v3].x;
	double y3 = points[v3].y;

	double dy12 = abs(y1 - y2);
	double dy23 = abs(y2 - y3);

	double xc, yc;

	if (dy12 < EPS) {
		double m2 = -((x3 - x2) / (y3 - y2));
		double mx2 = (x2 + x3) / 2;
		double my2 = (y2 + y3) / 2;

		xc = (x1 + x2) / 2;
		yc = m2 * (xc - mx2) + my2;
	}
	else if (dy23 < EPS) {
		double m1 = -((x2 - x1) / (y2 - y1));
		double mx1 = (x1 + x2) / 2;
		double my1 = (y1 + y2) / 2;

		xc = (x2 + x3) / 2;
		yc = m1 * (xc - mx1) + my1;
	}
	else {
		double m1 = -((x2 - x1) / (y2 - y1));
		double m2 = -((x3 - x2) / (y3 - y2));
		double mx1 = (x1 + x2) / 2;
		double my1 = (y1 + y2) / 2;
		double mx2 = (x2 + x3) / 2;
		double my2 = (y2 + y3) / 2;

		xc = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);

		if (dy12 > dy23)
			yc = m1 * (xc - mx1) + my1;
		else
			yc = m2 * (xc - mx2) + my2;
	}

	double dx = x2 - xc;
	double dy = y2 - yc;

	CircTri r;
	r.a = v1;
	r.b = v2;
	r.c = v3;
	r.x = xc;
	r.y = yc;
	r.r = dx * dx + dy * dy;

	return r;
}

int getEdge(vector<int>& edges, unsigned int i) {
	if (i >= edges.size()) {
		return -1;
	}

	return edges[i];
}

void fixEdges(vector<int>& edges) {
	for (int j = edges.size() - 1; j >= 0;) {
		int b = getEdge(edges, j); j--;
		int a = getEdge(edges, j); j--;
		int n, m;

		for (int i = j; i >= 0;) {
			n = getEdge(edges, i); i--;
			m = getEdge(edges, i); i--;

			if (a == m && b == n || a == n && b == m) {
				auto start1 = next(edges.begin(), j + 1);
				auto end1 = next(start1, 2);
				edges.erase(start1, end1);

				auto start = next(edges.begin(), i + 1);
				auto end = next(start, 2);
				edges.erase(start, end);

				break;
			}
		}
	}
}

vector<int> triangulate(vector<Vec3> points) {
	int n = points.size();
	if (n < 3) {
		vector<int> v;
		return v;
	}

	vector<int> ind;

	for (int i = 0; i < n; i++) {
		ind.push_back(i);
	}

	sort(ind.begin(), ind.end(), [&points](const int& a, const int& b) -> bool {
		return points[a].x > points[b].x;
	});

	firstTriangle(points);

	vector<CircTri> cur_points;
	cur_points.push_back(deloneCircle(points, n, n + 1, n + 2));

	vector<int> edges;
	vector<CircTri> ans;

	for (int i = ind.size() - 1; i >= 0; i--) {
		for (int j = cur_points.size() - 1; j >= 0; j--) {
			double dx = points[ind[i]].x - cur_points[j].x;
			if (dx > 0 && dx * dx > cur_points[j].r) {
				ans.push_back(cur_points[j]);
				cur_points.erase(next(cur_points.begin(), j));
				continue;
			}

			double dy = points[ind[i]].y - cur_points[j].y;
			if (dx * dx + dy * dy - cur_points[j].r > EPS) {
				continue;
			}

            edges.push_back(cur_points[j].a);
            edges.push_back(cur_points[j].b),
            edges.push_back(cur_points[j].b);
            edges.push_back(cur_points[j].c),
            edges.push_back(cur_points[j].c);
            edges.push_back(cur_points[j].a);

			cur_points.erase(next(cur_points.begin(), j));
		}

		fixEdges(edges);

		for (int j = edges.size() - 1; j >= 0;) {
			int b = edges[j]; j--;
			if (j < 0) break;
			int a = edges[j]; j--;
			cur_points.push_back(deloneCircle(points, a, b, ind[i]));
		}
		edges.clear();
	}

	for (int i = cur_points.size() - 1; i >= 0; i--) {
		ans.push_back(cur_points[i]);
	}

	vector<int> tr;
	for (unsigned int i = 0; i < ans.size(); i++) {
		if (ans[i].a < n && ans[i].b < n && ans[i].c < n) {
			tr.push_back(ans[i].a);
			tr.push_back(ans[i].b);
			tr.push_back(ans[i].c);
		}
	}

	return tr;
}