
//------------------------------------------------------------------------------
//
// File Name:	Random.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "Random.h"
#include "time.h"

//------------------------------------------------------------------------------
// Private Constants:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private Structures:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Variables:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private Variables:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private Function Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Public Functions:
//------------------------------------------------------------------------------



void Random::Init()
{
	//random seed based on time
	srand((unsigned)time(0));
}


int Random::RangeInt(int rangeMin, int rangeMax)
{
	return (rand() % (rangeMax - rangeMin + 1) + rangeMin);
}


float Random::RangeFloat(float rangeMin, float rangeMax)
{
	// code from MS Documentation https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/rand
	return ((float)rand() / RAND_MAX) * (rangeMax - rangeMin) + rangeMin;
}

glm::vec2 Random::RangeVec2(const glm::vec2& min, const glm::vec2& max) {
	return glm::vec2(
		Random::RangeFloat(min.x, max.x),
		Random::RangeFloat(min.y, max.y)
	);
}


glm::vec3 Random::RangeVec3(const glm::vec3& min, const glm::vec3& max) {
	return glm::vec3(
		Random::RangeFloat(min.x, max.x),
		Random::RangeFloat(min.y, max.y),
		Random::RangeFloat(min.z, max.z)
	);
}

glm::vec4 Random::RangeVec4(const glm::vec4& min, const glm::vec4& max) {
	return glm::vec4(
		Random::RangeFloat(min.x, max.x),
		Random::RangeFloat(min.y, max.y),
		Random::RangeFloat(min.z, max.z),
		Random::RangeFloat(min.w, max.w)
	);
}


glm::vec2 Random::Vec2() {
	return glm::vec2(
		Random::RangeFloat(0.0f, 1.0f),
		Random::RangeFloat(0.0f, 1.0f)
	);
}


glm::vec3 Random::Vec3() {
	return glm::vec3(
		Random::RangeFloat(0.0f, 1.0f),
		Random::RangeFloat(0.0f, 1.0f),
		Random::RangeFloat(0.0f, 1.0f)
	);
}

glm::vec4 Random::Vec4() {
	return glm::vec4(
		Random::RangeFloat(0.0f, 1.0f),
		Random::RangeFloat(0.0f, 1.0f),
		Random::RangeFloat(0.0f, 1.0f),
		Random::RangeFloat(0.0f, 1.0f)
	);
}

glm::vec2 Random::UnitVec2() {
	float angle = Random::RangeFloat(0.0f, glm::two_pi<float>());
	return glm::vec2(std::cos(angle), std::sin(angle));
}

glm::vec3 Random::UnitVec3() {
	float theta = Random::RangeFloat(0.0f, glm::two_pi<float>()); // azimuth
	float phi = std::acos(Random::RangeFloat(-1.0f, 1.0f));       // inclination

	float x = std::sin(phi) * std::cos(theta);
	float y = std::sin(phi) * std::sin(theta);
	float z = std::cos(phi);

	return glm::vec3(x, y, z); // already normalized
}


glm::vec4 Random::UnitVec4() {
	// Simple way using spherical Gaussian sampling
	auto randNormal = []() {
		// Box-Muller transform
		float u1 = Random::RangeFloat(0.0001f, 1.0f);
		float u2 = Random::RangeFloat(0.0001f, 1.0f);
		return std::sqrt(-2.0f * std::log(u1)) * std::cos(2.0f * glm::pi<float>() * u2);
		};

	glm::vec4 v(
		randNormal(),
		randNormal(),
		randNormal(),
		randNormal()
	);

	return glm::normalize(v);
}


void Random::RegisterToScripAPI(sol::state& lua)
{
	lua.create_named_table("Random",
		"range_int", &Random::RangeInt,
		"range_float",&Random::RangeFloat,
		"range_vec2", &Random::RangeVec2,
		"range_vec3", &Random::RangeVec3,
		"range_vec4", &Random::RangeVec4,
		"vec2",		  &Random::Vec2,
		"vec3",		  &Random::Vec3,
		"vec4",		  &Random::Vec4,
		"unit_vec2",  &Random::UnitVec2,
		"unit_vec3",  &Random::UnitVec3,
		"unit_vec4",  &Random::UnitVec4
	);
}

//------------------------------------------------------------------------------
// Private Functions:
//------------------------------------------------------------------------------

