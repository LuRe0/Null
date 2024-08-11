#pragma once
#include "IComponent.h"

namespace NULLENGINE
{
	class Camera;

	struct CameraComponent : public BaseComponent
	{
		std::string m_Name;
		std::string m_Type;
		Camera* m_Camera;
		bool m_IsMainCamera; 

		CameraComponent(const std::string& name = "", const std::string& type = "", Camera* cam = nullptr, bool maincam = false)
			: m_Name(name), m_Type(type), m_Camera(cam), m_IsMainCamera(maincam)
		{}

		const std::string Name() const { return Component<CameraComponent>::TypeName(); };
		const uint32_t ID() const { return Component<CameraComponent>::GetID(); };

	};

}