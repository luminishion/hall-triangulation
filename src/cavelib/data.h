#pragma once

#include <vector>
#include <map>
#include <string>

#include "alg/vec3.h"

struct Picket
{
	std::vector<Vec3> points;

	Vec3 pos;
	std::vector<Vec3> nextPos;

	std::vector<std::string> nextId;
	std::vector<std::string> prevId;

	bool constrainted = false;
};

struct EQ {
	std::string first;
	std::string second;
};

class CaveData
{
public:
	bool buildFromFile(std::string path);
	void center();
	std::vector<Vec3> getPoints();

	void registerPicket(std::string name);
	void setConstraint(std::string current, std::string constraint, Vec3 vec);

	void addPoint(std::string currrent, Vec3 vec);
private:
	void doCmd(std::string cmd);
	void doCoord(std::string coord);
	void doLine(std::string line);

public:
	std::map<std::string, Picket> pickets;
	std::vector<EQ> eq;
	bool isCav = false;
	double corrA = 0;
};

