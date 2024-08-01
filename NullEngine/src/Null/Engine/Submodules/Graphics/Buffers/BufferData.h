#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	BufferData.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
//#include "glm/glm.hpp"
#include <nlohmann/json.hpp>


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//

struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 textCoords;
};

//// Custom to_json function
//void to_json(nlohmann::json& j, const Vertex& v) {
//	j = nlohmann::json{ {"position", {v.position.x, v.position.y, v.position.z}},
//					   {"color", {v.color.r, v.color.g, v.color.b, v.color.a}},
//					   {"uv", {v.uv.x, v.uv.y}} };
//}
//
//// Custom from_json function
//void from_json(const nlohmann::json& j, Vertex& v) {
//	v.position = glm::vec3(j.at("position")[0], j.at("position")[1], j.at("position")[2]);
//	v.color = glm::vec4(j.at("color")[0], j.at("color")[1], j.at("color")[2], j.at("color")[3]);
//	v.uv = glm::vec2(j.at("uv")[0], j.at("uv")[1]);
//}

struct Instance
{
	glm::vec3 vertexposition;

	glm::vec4 vertexColor;

	glm::vec2 vertexCoords;

	glm::mat4 model;

	glm::vec4 color;

	glm::vec2 textCoords;

	glm::vec2 textSize;

	unsigned int textIndex;

	uint32_t entityID;
};



struct Layout
{
	unsigned int count;
	unsigned int glType;
	unsigned int size;
};
