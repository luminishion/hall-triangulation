#pragma once

class Vec3
{
public:
	static Vec3 fromSpherical(double len, double pitch, double yaw);

	Vec3();
	Vec3(double x, double y, double z);

	Vec3 operator+(const Vec3& vec);
	Vec3& operator+=(const Vec3& vec);

	Vec3 operator-(const Vec3& vec);
	Vec3& operator-=(const Vec3& vec);

	Vec3 operator-() const;

	Vec3 operator*(double val);
	Vec3& operator/=(double val);

	double length2d();
	Vec3 normalized();

public:
	double x = 0;
	double y = 0;
	double z = 0;
};

