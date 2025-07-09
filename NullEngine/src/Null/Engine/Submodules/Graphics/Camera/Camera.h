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
#include "PostProcess/PostProcess.h"

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
    
    class NLE_API Camera
    {
    public:
        enum CameraType
        {
            INVALID = -1,
            ORTHOGRAPHIC,
            PERSPECTIVE,
            PROJECTIONTYPE
        };

        struct Plane
        {
            glm::vec3 point;   // Any point on the plane
            glm::vec3 normal;  // Normal pointing inward
        };

        struct Frustum
        {
            Plane planes[6]; // 0: near, 1: far, 2: right, 3: left, 4: top, 5: bottom

            static Frustum FromCamera(glm::vec3 position, glm::vec3 up, glm::vec3 right, glm::vec3 front,
                float aspect, float fovY, float zNear, float zFar);
        };

        virtual ~Camera() = default;

        virtual void Update(float dt) = 0;

        virtual void Init() = 0;

        virtual void Shudown() = 0;

        virtual void View();

        virtual void Write(JSON& json);

        virtual bool IsWithinFrustum(const glm::vec3& center, const glm::vec3& halfExtents) const  = 0;

        virtual const glm::mat4 GetViewMatrix() const = 0;

        void SetShakeOffset(const glm::vec3& offset);
        const glm::vec3& GetShakeOffset() const { return m_ShakeOffset; }

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const CameraType& GetCameraType() const { return m_CameraType; }
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }
        const PostProcess& GetPPSettings() { return m_PPSettings; }
        PostProcess& PPSettings() { return m_PPSettings; }
    protected:
        glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
        glm::vec3 m_ShakeOffset = glm::vec3(0.0f);

        CameraType m_CameraType = CameraType::INVALID;
        std::string m_Name = "";
        bool m_IsDirty = true;

        PostProcess m_PPSettings;
        std::string m_PPTemplateFilename;
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