  #pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	BatchRenderer.h
// Author(s):	name
// 
//------------------------------------------------------------------------------
/*
Code adapted from https://learnopengl.com
Made by Joey de Vries https://twitter.com/JoeyDeVriez\
LearnOpenGl license: https://creativecommons.org/licenses/by/4.0/legalcode
*/
//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{
	class BatchRenderer
	{
	public:
		static const uint32_t m_MaxTextureSlots = 32;

		struct RendererStats
		{
			uint32_t DrawCalls = 0;
			uint32_t InstanceCount = 0;
			uint32_t TextureCount = 0;

			//uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			//uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		virtual ~BatchRenderer() {};

		virtual void BeginBatch() = 0;
		virtual void NextBatch() = 0;
		virtual void Flush() = 0;
		virtual void ImguiView() = 0;
		virtual void BindTextureBuffer() = 0;
		virtual void AddInstance(const ElementData& render) = 0;

		virtual void ResetStats() = 0;
	protected:
		uint32_t m_MaxInstances;
		uint32_t m_MaxVertices;
		uint32_t m_MaxIndices;


		uint32_t m_InstanceIndexCount = 0;
		uint32_t m_TextureSlotIndex = 0;

		//std::unordered_set<unsigned int> TexturesUsed;


		std::array<Texture*, m_MaxTextureSlots> m_TextureSlots;
	};


}