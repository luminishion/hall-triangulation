#include "data.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <set>

#include "alg/vec3.h"

using namespace std;

void CaveData::addPoint(string name, Vec3 pos) {
	Picket& p = pickets[name];
	p.points.push_back(pos);
}

void CaveData::registerPicket(string name) {
	if (pickets.find(name) == pickets.end()) {
		Picket p;
		pickets[name] = p;
	}
}

void CaveData::setConstraint(string current, string constrainted, Vec3 vec) {
	Picket& from = pickets[current];
	Picket& to = pickets[constrainted];

	from.nextPos.push_back(vec);
	from.nextId.push_back(constrainted);
	to.prevId.push_back(current);

	if (!to.constrainted) {
		to.pos = from.pos + vec;
	}

	from.constrainted = true;
	to.constrainted = true;
}


void equateHandler(CaveData* cd, string line) {
	istringstream ls(line);

	string a, b, c;
	if (!(ls >> a >> b >> c)) {
		return;
	}

	if (b[0] == '^') {
		b.erase(0, 1);
	}

	if (c[0] == '^') {
		c.erase(0, 1);
	}

	cd->eq.push_back(EQ{ b, c });
}

void corrAHandler(CaveData* cd, string line) {
	line.resize(line.find_first_of("]"));
	line = line.substr(line.find_first_of("[") + 1);

	double a;
	istringstream ls(line);
	ls >> a;

	cd->corrA = a;
}

void ecorrAHandler(CaveData* cd, string line) {
	cd->corrA = 0;
}

map<string, void*> cmdHandler{
	{"equate", equateHandler},
	{"corr_A", corrAHandler},
	{"end_corr_A", ecorrAHandler},
};

void CaveData::doCmd(string line) {
	string cmd = line;

	size_t found = cmd.find_first_of("[ \n\r\t\f\v");
	if (found != string::npos) {
		cmd.resize(found);
	}

	if (cmdHandler.find(cmd) != cmdHandler.end()) {
		((void (*)(CaveData*, string)) cmdHandler[cmd])(this, line);
	}
}


auto lcNeutral = _create_locale(LC_NUMERIC, "C");

void CaveData::doCoord(string line) {
	size_t found = line.find_first_of("#");
	if (found != string::npos) {
		line.resize(found);
	}

	string curPicket, constraint, yawStr;
	double len, pitch;

	istringstream ls(line);

	if (!(ls >> curPicket >> constraint >> len >> yawStr >> pitch)) {
		return;
	}

	double yaw = yawStr == "-" ? 0 : _strtod_l(yawStr.c_str(), NULL, lcNeutral);
	yaw += corrA;

	Vec3 v = Vec3::fromSpherical(len, pitch, yaw);

	registerPicket(curPicket);

	if (constraint[0] == '-') {
		addPoint(curPicket, v);
		return;
	}

	registerPicket(constraint);

	if (isCav) {
		string temp = constraint;
		constraint = curPicket;
		curPicket = temp;
		v = -v;
	}

	setConstraint(curPicket, constraint, v);
}


void CaveData::doLine(string line) {
	//remove comments
	size_t found = line.find_first_of(";%");
	if (found != string::npos) {
		line.resize(found);
	}

	//trim left
	size_t start = line.find_first_not_of(" \n\r\t\f\v");
	line = start == string::npos ? "" : line.substr(start);

	if (line.size() == 0) {
		return;
	}

	if (line[0] == '#') {
		doCmd(line.substr(1));
		return;
	}

	doCoord(line);
}

void find(set<string>& allNames, string& to, map<string, Picket>& pickets) {
	if (allNames.find(to) != allNames.end()) {
		return;
	}
	allNames.insert(to);

	Picket j = pickets[to];

	for (auto& v : j.nextId) {
		find(allNames, v, pickets);
	}

	for (auto& v : j.prevId) {
		find(allNames, v, pickets);
	}
}

bool CaveData::buildFromFile(string path) {
	pickets.clear();
	eq.clear();
	isCav = path.substr(path.find_last_of(".") + 1) == "cav";

	ifstream fs(path);

	string line;
	while (getline(fs, line))
	{
		doLine(line);
	}

	for (auto& k : eq) {
		string first = k.first;
		string second = k.second;

		if (pickets.find(first) == pickets.end() || pickets.find(second) == pickets.end()) {
			continue;
		}

		Picket& from = pickets[first];
		Picket& to = pickets[second];

		Vec3 o = from.pos - to.pos;

		set<string> allNames;
		find(allNames, second, pickets);

		for (auto& s : allNames) {
			pickets[s].pos += o;
		}
	}

	return !pickets.empty();
}


void CaveData::center() {
	Vec3 center = Vec3(0, 0, 0);
	int count = 0;

	for (const auto& kv : pickets) {
		Picket picket = kv.second;
		Vec3 pos = picket.pos;

		for (auto& vec : picket.points) {
			center += pos + vec;
			count++;
		}
	}

	if (count == 0) {
		return;
	}

	center /= count;

	for (auto& kv : pickets) {
		kv.second.pos -= center;
	}
}

vector<Vec3> CaveData::getPoints() {
	vector<Vec3> v;

	for (const auto& kv : pickets) {
		Picket picket = kv.second;
		Vec3 pos = picket.pos;

		for (auto& vec : picket.points) {
			v.push_back(pos + vec);
		}
	}

	return v;
}