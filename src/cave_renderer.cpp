#include "cave_renderer.h"

#include "cavelib/data.h"
#include "cavelib/point_type.h"
#include "cavelib/points_triangulation.h"
#include "cavelib/borders.h"

#include <GLFW/glfw3.h>

using namespace std;

void CaveRenderer::drawTriangles(double alpha) {
	glBegin(GL_TRIANGLES);

	for (unsigned int i = 0; i < triangles.size(); i++) {
		CTriangle v = triangles[i];

		CPoint p1 = cpts[v.v1];
		CPoint p2 = cpts[v.v2];
		CPoint p3 = cpts[v.v3];

		Vec3 v1 = p1.pos;
		Vec3 v2 = p2.pos;
		Vec3 v3 = p3.pos;

		if (alpha != 0) {
			switch (v.type) {
			case T_UP:
				glColor4d(0.3, 0.2, 1, alpha);
				break;
			case T_FLOOR:
				glColor4d(1, 0.2, 0.2, alpha);
				break;
			case T_WALL:
				glColor4d(0, 1, 0, alpha);
				break;
			}
		}

		glVertex3d(v1.x, v1.y, v1.z);
		glVertex3d(v2.x, v2.y, v2.z);
		glVertex3d(v3.x, v3.y, v3.z);
	}

	glEnd();
}

void CaveRenderer::drawDepthLines(double alpha) {
	//fix z-filtering artifact
	glPolygonOffset(-1, -1);
	glEnable(GL_POLYGON_OFFSET_LINE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawTriangles(alpha);

	glDisable(GL_POLYGON_OFFSET_LINE);
}

void CaveRenderer::renderMode0() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawTriangles(alpha);

	glColor4d(0, 0, 0, 1);
	drawDepthLines(0);
}

void CaveRenderer::renderMode1() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor4d(0.12, 0.12, 0.12, alpha);
	drawTriangles(0);

	drawDepthLines(1);
}

void CaveRenderer::renderMode2() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor4d(0.12, 0.12, 0.12, alpha);
	drawTriangles(0);

	glColor4d(1, 1, 1, 1);
	drawDepthLines(0);
}

void CaveRenderer::renderMode3() {
	glColor4d(0, 0, 0, alpha);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawTriangles(0);
}

void CaveRenderer::renderMode4() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawTriangles(1);
}

void CaveRenderer::renderMode5() {
	glLineWidth(2);

	glBegin(GL_LINES);

	for (const auto& kv : cd.pickets) {
		Picket picket = kv.second;

		Vec3 v2 = picket.pos;

		for (auto& v3 : picket.nextPos) {
			v3 += v2;
			glColor3d(1, 0, 0);
			glVertex3d(v2.x, v2.y, v2.z);
			glVertex3d(v3.x, v3.y, v3.z);
		}

		for (auto& vec : picket.points) {
			Vec3 v1 = v2 + vec;

			glColor3d(0, 0, 0);
			glVertex3d(v1.x, v1.y, v1.z);
			glVertex3d(v2.x, v2.y, v2.z);
		}
	}

	glEnd();
}

void CaveRenderer::renderMode6() {
	glPointSize(5);

	glBegin(GL_POINTS);

	for (unsigned int i = 0; i < cpts.size(); i++) {
		Vec3 v = cpts[i].pos;

		cpts[i].isFloor ? glColor3d(1, 0.2, 0) : glColor3d(0, 0.2, 1);
		glVertex3d(v.x, v.y, v.z);
	}

	glEnd();
}

void CaveRenderer::renderMode7() {
	glLineWidth(2);
	glColor3d(0, 0, 0);

	glBegin(GL_LINES);

	for (unsigned int i = 0; i < bord.size(); i++) {
		Vec3 v1 = bord[i];
		Vec3 v2 = bord[(i + 1) % bord.size()];

		glVertex3d(v1.x, v1.y, v1.z);
		glVertex3d(v2.x, v2.y, v2.z);
	}

	glEnd();
}

typedef void (CaveRenderer::* method_function)();

method_function modes[] = {
	&CaveRenderer::renderMode0,
	&CaveRenderer::renderMode1,
	&CaveRenderer::renderMode2,
	&CaveRenderer::renderMode3,
	&CaveRenderer::renderMode4,
	&CaveRenderer::renderMode5,
	&CaveRenderer::renderMode6,
	&CaveRenderer::renderMode7,
};

void CaveRenderer::render() {
	(this->*modes[currentRender])();
}

void CaveRenderer::nextRenderMode() {
	currentRender = (currentRender + 1) % (sizeof(modes) / sizeof(void*));
}

void CaveRenderer::prevRenderMode() {
	if (--currentRender == -1) {
		currentRender = sizeof(modes) / sizeof(void*) - 1;
	}
}


void CaveRenderer::resetSettings() {
	hardness = 1;
	alpha = 1.0;
	ncount = 6;
}

void CaveRenderer::setHardness(double hardness) {
	this->hardness = hardness;
}

double CaveRenderer::getHardness() {
	return hardness;
};

void CaveRenderer::setNCount(int n) {
	this->ncount = n;
}

int CaveRenderer::getNCount() {
	return ncount;
};

void CaveRenderer::setAlpha(double alpha) {
	this->alpha = alpha;
}

double CaveRenderer::getAlpha() {
	return alpha;
};


void CaveRenderer::setCaveData(CaveData cd) {
	this->cd = cd;
}

void CaveRenderer::build() {
	vector<Vec3> pts = cd.getPoints();

	bord = getCaveBorders(pts, hardness);

	cpts.clear();

	for (unsigned int i = 0; i < pts.size(); i++) {
		CPoint v;
		v.pos = pts[i];
		v.isFloor = simpleGlobal(cd, v.pos, ncount);
		cpts.push_back(v);
	}

	triangles = triangulateCavePoints(cpts, hardness);
}