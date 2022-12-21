#include "vec3.h"

#define _USE_MATH_DEFINES
#include <math.h>

Vec3::Vec3() {
}

Vec3::Vec3(double _x, double _y, double _z) {
	x = _x;
	y = _y;
	z = _z;
}

Vec3 Vec3::operator+(const Vec3& vec)
{
	return Vec3(x + vec.x, y + vec.y, z + vec.z);
}

Vec3& Vec3::operator+=(const Vec3& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;

	return *this;
}

Vec3 Vec3::operator-(const Vec3& vec)
{
	return Vec3(x - vec.x, y - vec.y, z - vec.z);
}

Vec3& Vec3::operator-=(const Vec3& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;

	return *this;
}

Vec3 Vec3::operator-() const {
	return Vec3(-x, -y, -z);
}

Vec3 Vec3::operator*(double d)
{
	return Vec3(x * d, y * d, z * d);
}

Vec3& Vec3::operator/=(double val)
{
	x /= val;
	y /= val;
	z /= val;

	return *this;
}

double rad(double deg) {
	return deg / 180.0 * M_PI;
}

Vec3 Vec3::fromSpherical(double l, double pitch, double yaw) {
	pitch = rad(90.0 - pitch);
	yaw = rad(yaw);

	double x = l * sin(pitch) * cos(-yaw);
	double y = l * sin(pitch) * sin(-yaw);
	double z = l * cos(pitch);

	return Vec3(x, y, z);
}

Vec3 Vec3::normalized() {
	double m = sqrt(x * x + y * y + z * z);

	return Vec3(x / m, y / m, z / m);
}

double Vec3::length2d() {
	return sqrt(x * x + y * y);
}