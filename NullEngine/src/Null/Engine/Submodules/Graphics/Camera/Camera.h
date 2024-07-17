#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Camera.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

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
    class Camera {
    public:
        virtual ~Camera() = default;

        virtual void Update(float dt) = 0;

        virtual void Init() = 0;

        virtual void Shudown() = 0;

        virtual const glm::mat4& GetViewMatrix() const = 0;

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

    protected:
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

        bool m_IsDirty;
    };

	template <typename T>
	class NLE_API CameraHelper : public Camera
	{
	public:
		static std::string TypeName() {
			std::string fullName = typeid(T).name();
			// Manual parsing: adjust based on compiler output
			auto pos = fullName.find_last_of(':');
			return fullName.substr(pos + 1);
		}
	private:
	};

}