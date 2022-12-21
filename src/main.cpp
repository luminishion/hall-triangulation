#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3_mt.lib ")

#include "windows.h"

#include "cavelib/data.h"
#include "camera_view.h"
#include "cave_renderer.h"

#include <map>
#include <thread>
#include <iostream>
#include <sstream>
#include <mutex>
#include <filesystem>

#include <GLFW/glfw3.h>

using namespace std;

mutex g_MtxGuiCmd;
CaveRenderer g_CaveRend;


// CONSOLE HANDLERS

void resetHandler(istringstream& lss) {
	g_CaveRend.resetSettings();
	g_CaveRend.build();
}

void hardnessHandler(istringstream& lss) {
	double hardness;
	lss >> hardness;

	g_CaveRend.setHardness(hardness);
	g_CaveRend.build();
}

void ncountHandler(istringstream& lss) {
	int ncount;
	lss >> ncount;

	g_CaveRend.setNCount(ncount);
	g_CaveRend.build();
}

void alphaHandler(istringstream& lss) {
	double alpha;
	lss >> alpha;

	g_CaveRend.setAlpha(alpha);
}

map<string, void*> handler{
	{"reset", resetHandler},
	{"hardness", hardnessHandler},
	{"ncount", ncountHandler},
	{"alpha", alphaHandler},
};

void runCmdHandlerLoop() {
	while (1) {
		string line;
		getline(cin, line);

		istringstream lss(line);

		string cmd;
		lss >> cmd;

		if (handler.find(cmd) == handler.end()) {
			printf("Command not found\n");
			continue;
		}

		g_MtxGuiCmd.lock();
			((void (*)(istringstream&)) handler[cmd])(lss);
		g_MtxGuiCmd.unlock();
	}
}


// GUI

void keyFn(GlCameraView* w, int key, int action) {
	if (action != GLFW_PRESS) {
		return;
	}

	switch (key) {
	case GLFW_KEY_Z:
		g_CaveRend.nextRenderMode();
		break;
	case GLFW_KEY_X:
		g_CaveRend.prevRenderMode();
		break;
	case GLFW_KEY_C:
		g_CaveRend.setAlpha(g_CaveRend.getAlpha() == 1 ? 0.3 : 1);
		break;
	default:
		return;
	}
}

void renderFn(GlCameraView* v) {
	g_CaveRend.render();
}

void startWindow() {
	if (!glfwInit()) {
		printf("Could not init OpenGL\n");
		return;
	}

	GlCameraView w = GlCameraView("3D Trianglutaion");
	w.renderFn = renderFn;
	w.keyFn = keyFn;

	while (!w.shouldClose()) {
		g_MtxGuiCmd.lock();
			w.doEvents();
		g_MtxGuiCmd.unlock();

		Sleep(15);
	}

	glfwTerminate();
	exit(0);
}

void runGuiThread() {
	thread t(startWindow);
	t.detach();
}


// STARTUP

const char* HELP_TEXT = 
	"Console commands:\n"
	"reset - reset settings\n"
	"hardness <double> - triangulation filter hardness. recommended from 0.6 to 2\n"
	"ncount <integer> - count of points for testing the height\n"
	"alpha <double> - polygon's alpha, from 0 to 1\n\n"

	"Camera movement: \n"
	"W - forward\n"
	"S - back\n"
	"A - left\n"
	"D - right\n"
	"Space - up\n\n"
	"Camera rotation:\n"
	"Hold right mouse button\n"
	"Or use keyboard arrows\n\n"

	"Buttons:\n"
	"SHIFT - move faster\n"
	"CTRL - move slower\n"
	"Z - next rendermode\n"
	"X - previous rendermode\n"
	"C - fast change alpha\n";

int main(int argc, char* argv[]) {
	const char* path = argv[1];

	CaveData cd;

	if (path == NULL || !cd.buildFromFile(path)) {
		printf("Input file not found, drag and drop the file to application icon");
		Sleep(10000);
		return 0;
	}

	printf(HELP_TEXT);

	cd.center();

	g_CaveRend.setCaveData(cd);
	g_CaveRend.build();
	
	runGuiThread();
	runCmdHandlerLoop();

	return 0;
}