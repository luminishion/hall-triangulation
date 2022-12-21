#include "camera_view.h"

#include "cavelib/alg/vec3.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "wtypes.h"
#include <chrono>

#include <GLFW/glfw3.h>

#define rad(x) ((x) / 180.0 * M_PI)

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UNKNOWN) {
		return;
	}

	GlCameraView* win = (GlCameraView*)glfwGetWindowUserPointer(window);

	win->runKeyCallback(key, action);
}

void GlCameraView::runKeyCallback(int key, int action) {
	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}

	if (action == GLFW_PRESS) {
		pressed[key] = true;
	}
	else if (action == GLFW_RELEASE) {
		pressed[key] = false;
	}

	keyFn(this, key, action);
}


void mouseButtonCallback(GLFWwindow* window, int key, int action, int mods)
{
	GlCameraView* win = (GlCameraView*)glfwGetWindowUserPointer(window);

	win->runMouseCallback(key, action);
}

void GlCameraView::runMouseCallback(int key, int action) {
	if (key == GLFW_MOUSE_BUTTON_RIGHT)
	{
		mouseHold = GLFW_PRESS == action;
		lastX = -1;
	}
}


void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);

	horizontal = desktop.right;
	vertical = desktop.bottom;
}

GlCameraView::GlCameraView(const char* name)
{
	int sw, sh;
	GetDesktopResolution(sw, sh);

	int w = (int)(sw * 0.75);
	int h = (int)(sh * 0.75);

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(w, h, name, NULL, NULL);
	if (!window) {
		return;
	}

	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	glfwSetWindowMonitor(window, NULL, sw / 2 - w / 2, sh / 2 - h / 2, w, h, GLFW_DONT_CARE);
}

GlCameraView::~GlCameraView()
{
	if (!window) {
		return;
	}

	glfwDestroyWindow(window);
}


Vec3 GlCameraView::getForward() {
	double yaw = rad(-rotateY + 90);
	double p = rad(rotateX);
	double x = cos(p);

	return -Vec3(cos(yaw) * x, sin(yaw) * x, sin(p));
}

Vec3 GlCameraView::getLeft() {
	double yaw = rad(rotateY) - M_PI / 2;

	return Vec3(-sin(yaw), -cos(yaw), 0);
}

Vec3 GlCameraView::getUp() {
	double yaw = rad(-rotateY + 90);
	double p = rad(rotateX + 90);
	double x = cos(p);

	return -Vec3(cos(yaw) * x, sin(yaw) * x, sin(p));
}


void GlCameraView::handleInput() {
	double speed = frameTime * 23;

	if (pressed[GLFW_KEY_LEFT_SHIFT]) {
		speed *= 2;
	}

	if (pressed[GLFW_KEY_LEFT_CONTROL]) {
		speed /= 2;
	}

	if (mouseHold) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		if (lastX != -1) {

			rotateY += (xpos - lastX) * speed * 0.75;
			rotateX = max(min(rotateX - (ypos - lastY) * speed * 0.75, 90), -90);
		}

		lastX = xpos;
		lastY = ypos;
	}

	if (pressed[GLFW_KEY_UP]) {
		rotateX = max(min(rotateX + speed, 90), -90);
	}

	if (pressed[GLFW_KEY_DOWN]) {
		rotateX = max(min(rotateX - speed, 90), -90);
	}

	if (pressed[GLFW_KEY_RIGHT]) {
		rotateY += speed;
	}

	if (pressed[GLFW_KEY_LEFT]) {
		rotateY -= speed;
	}

	if (pressed[GLFW_KEY_W]) {
		pos += getForward() * speed;
	}

	if (pressed[GLFW_KEY_S]) {
		pos -= getForward() * speed;
	}

	if (pressed[GLFW_KEY_A]) {
		pos += getLeft() * speed;
	}

	if (pressed[GLFW_KEY_D]) {
		pos -= getLeft() * speed;
	}

	if (pressed[GLFW_KEY_SPACE]) {
		pos += getUp() * speed;
	}
}


void perspective(double fov, double aspect, double front, double back)
{
	fov = rad(fov);
	fov = 2.0 * atan(tan(fov * 0.5) / aspect);

	double tangent = tan(fov / 2.0);
	double height = front * tangent;
	double width = height * aspect;

	glFrustum(-width, width, -height, height, front, back);
}

void GlCameraView::runRender() {
	GLint w, h;
	glfwGetWindowSize(window, &w, &h);

	glViewport(0, 0, w, h);

	glfwMakeContextCurrent(window);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	perspective(70.0, (double)w / h, 0.1, 1000);

	glRotated(-rotateX - 90, 1.0, 0.0, 0.0);
	glRotated(rotateY, 0.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslated(pos.x, pos.y, pos.z);

	renderFn(this);

	glfwSwapBuffers(window);
	glfwPollEvents();
}


void GlCameraView::doEvents()
{
	if (!window || !renderFn)
		return;

	handleInput();
	runRender();

	//for the same speed at different FPS
	auto t2 = std::chrono::steady_clock::now();
	frameTime = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - lastRenderTime).count();
	lastRenderTime = t2;
}

bool GlCameraView::shouldClose()
{
	return !window || !renderFn || glfwWindowShouldClose(window);
}