#pragma once

#include "cavelib/data.h"
#include "cavelib/points_triangulation.h"

#include <vector>

class CaveRenderer
{
public:
	void setCaveData(CaveData cd);

	void setHardness(double hardness);
	double getHardness();

	void setNCount(int n);
	int getNCount();

	void setAlpha(double alpha);
	double getAlpha();

	void resetSettings();

	void nextRenderMode();
	void prevRenderMode();

	void renderMode0();
	void renderMode1();
	void renderMode2();
	void renderMode3();
	void renderMode4();
	void renderMode5();
	void renderMode6();
	void renderMode7();

	void build();
	void render();

private:
	void drawTriangles(double alpha);
	void drawDepthLines(double alpha);

private:
	double hardness = 1;
	int ncount = 6;
	double alpha = 1;

	CaveData cd;

	std::vector<CPoint> cpts;
	std::vector<CTriangle> triangles;
	std::vector<Vec3> bord;

	int currentRender = 0;
};

