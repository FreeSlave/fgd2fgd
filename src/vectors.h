#pragma once

#include <string>

#define EPSILON 0.0001f // EPSILON from rad.h / 10

struct vec3
{
	float x, y, z;

	void Copy(const vec3& other) 
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}
	vec3& operator =(const vec3& other)
	{
		Copy(other); 
		return *this;
	}

	vec3(const vec3& other) 
	{
		Copy(other);
	}

	vec3() : x(+0.0f), y(+0.0f), z(+0.0f)
	{

	}

	vec3(float x, float y, float z) : x(x), y(y), z(z)
	{
		if (abs(x) < EPSILON)
		{
			x = +0.0f;
		}
		if (abs(y) < EPSILON)
		{
			y = +0.0f;
		}
		if (abs(z) < EPSILON)
		{
			z = +0.0f;
		}
	}
	float length();
	bool IsZero();
	std::string toKeyvalueString(bool truncate = false, const std::string& suffix_x = " ", const std::string& suffix_y = " ", const std::string& suffix_z = "");
	std::string toString();

	void operator-=(const vec3& v);
	void operator+=(const vec3& v);
	void operator*=(const vec3& v);
	void operator/=(const vec3& v);

	void operator-=(float f);
	void operator+=(float f);
	void operator*=(float f);
	void operator/=(float f);

	vec3 operator-()
	{
		x *= -1.f;
		y *= -1.f;
		z *= -1.f;
		return *this;
	}

	float operator [] (const int i) const
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
		return z;
	}

	float& operator [] (const int i)
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
		return z;
	}

};

vec3 operator-(vec3 v1, const vec3& v2);
vec3 operator+(vec3 v1, const vec3& v2);
vec3 operator*(vec3 v1, const vec3& v2);
vec3 operator/(vec3 v1, const vec3& v2);

vec3 operator+(vec3 v, float f);
vec3 operator-(vec3 v, float f);
vec3 operator*(vec3 v, float f);
vec3 operator/(vec3 v, float f);

bool operator==(const vec3& v1, const vec3& v2);
bool operator!=(const vec3& v1, const vec3& v2);
