#pragma once

#include <GLFW/glfw3.h>

#include "cavelib/alg/vec3.h"

#include <chrono>

class GlCameraView;

typedef void (*fnRender) (GlCameraView*);
typedef void (*fnKeypress) (GlCameraView*, int key, int action);

class GlCameraView
{
public:
	GlCameraView(const char* name);
	~GlCameraView();

	void doEvents();
	bool shouldClose();

	void runKeyCallback(int key, int action);
	void runMouseCallback(int key, int acition);

private:
	Vec3 getForward();
	Vec3 getLeft();
	Vec3 getUp();

	void runRender();
	void handleInput();

public:
	GLFWwindow* window = NULL;
	fnRender renderFn = NULL;
	fnKeypress keyFn = NULL;

	double frameTime = 0;

	bool pressed[GLFW_KEY_LAST + 1] = {};

private:
	Vec3 pos = Vec3(200, 0, 0);

	double rotateX = 0;
	double rotateY = 90;

	bool mouseHold = false;
	double lastX = -1;
	double lastY = -1;

	std::chrono::steady_clock::time_point lastRenderTime = std::chrono::steady_clock::now();
};