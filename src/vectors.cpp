#include "vectors.h"
#include <cmath>

bool operator==(const vec3& v1, const vec3& v2)
{
	vec3 v = v1 - v2;
	if (abs(v.x) >= EPSILON)
		return false;
	if (abs(v.y) >= EPSILON)
		return false;
	if (abs(v.z) >= EPSILON)
		return false;
	return true;
}

bool operator!=(const vec3& v1, const vec3& v2)
{
	vec3 v = v1 - v2;
	if (abs(v.x) >= EPSILON)
		return true;
	if (abs(v.y) >= EPSILON)
		return true;
	if (abs(v.z) >= EPSILON)
		return true;
	return false;
}

vec3 operator-(vec3 v1, const vec3& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	return v1;
}

vec3 operator+(vec3 v1, const vec3& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}

vec3 operator*(vec3 v1, const vec3& v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
	return v1;
}

vec3 operator/(vec3 v1, const vec3& v2)
{
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;
	return v1;
}

vec3 operator-(vec3 v, float f)
{
	v.x -= f;
	v.y -= f;
	v.z -= f;
	return v;
}

vec3 operator+(vec3 v, float f)
{
	v.x += f;
	v.y += f;
	v.z += f;
	return v;
}

vec3 operator*(vec3 v, float f)
{
	v.x *= f;
	v.y *= f;
	v.z *= f;
	return v;
}

vec3 operator/(vec3 v, float f)
{
	v.x /= f;
	v.y /= f;
	v.z /= f;
	return v;
}

void vec3::operator-=(const vec3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

void vec3::operator+=(const vec3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}

void vec3::operator*=(const vec3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
}

void vec3::operator/=(const vec3& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
}

void vec3::operator-=(float f)
{
	x -= f;
	y -= f;
	z -= f;
}

void vec3::operator+=(float f)
{
	x += f;
	y += f;
	z += f;
}

void vec3::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;
}

void vec3::operator/=(float f)
{
	x /= f;
	y /= f;
	z /= f;
}

float vec3::length()
{
	return sqrt((x * x) + (y * y) + (z * z));
}

bool vec3::IsZero()
{
	return (abs(x) + abs(y) + abs(z)) < EPSILON;
}

std::string vec3::toString()
{
	return std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);
}

std::string vec3::toKeyvalueString(bool truncate, const std::string& suffix_x, const std::string& suffix_y, const std::string& suffix_z)
{
	std::string parts[3] = {std::to_string(x) ,std::to_string(y), std::to_string(z)};

	// remove trailing zeros to save some space
	for (int i = 0; i < 3; i++)
	{
		if (truncate)
		{
			parts[i] = parts[i].substr(0, parts[i].find('.') + 3);
		}

		parts[i].erase(parts[i].find_last_not_of('0') + 1, std::string::npos);

		// strip dot if there's no fractional part
		if (parts[i][parts[i].size() - 1] == '.')
		{
			parts[i] = parts[i].substr(0, parts[i].size() - 1);
		}
	}

	return parts[0] + suffix_x + parts[1] + suffix_y + parts[2] + suffix_z;
}
