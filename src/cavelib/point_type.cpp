#include "point_type.h"

#include <vector>
#include <map>
#include <algorithm>

#include "alg/vec3.h"
#include "data.h"

using namespace std;

struct VL {
	Vec3 pos;
	double l = 0;
};

bool simpleGlobal(CaveData& cd, Vec3& target, int ncount) {
	vector<VL> neighbors;

	for (const auto& kv : cd.pickets) {
		Picket picket = kv.second;

		for (auto& vec : picket.points) {
			Vec3 vpos = picket.pos + vec;

			double l = (target - vpos).length2d();

			VL vl;
			vl.pos = vpos;
			vl.l = l;

			neighbors.push_back(vl);
		}
	}

	sort(neighbors.begin(), neighbors.end(), [](const VL& a, const VL& b) -> bool {
		return b.l > a.l;
	});

	double maxV = -INFINITY;
	double minV = INFINITY;

	for (int i = 0; i < min((int)neighbors.size(), ncount); i++) {
		Vec3 vec = neighbors[i].pos;

		double h = target.z - vec.z;
		maxV = max(maxV, h);
		minV = min(minV, h);
	}

	return abs(maxV) < abs(minV);
}