#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NCameraManager.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"
//#include "Null/Engine/Submodules/Graphics/Camera/Camera.h"



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
    class Camera;
	class Camera2D;
	class Camera3D;

    class sol::state;
    // Forward declarations for events
    class EngineEditStateEvent;
	class EnginePauseStateEvent;

	class NLE_API NCameraManager : public ModuleBase<NCameraManager>
	{
	public:
        NCameraManager() = default;
		void Load() override;
		//! Virtual Init function
		void Init() override;
		//! Virtual Update function
		void Update(float dt) override;
        void RuntimeUpdate(float dt) override;

		void Unload() override;
		//! Virtual Shutdown function
		void Shutdown() override;

        template <typename T, typename... Args>
        T* AddCamera(const uint32_t& name, Args&&... args) {
            static_assert(std::is_base_of<Camera, T>::value, "T must be derived from Camera");

            if constexpr (std::is_same<T, Camera2D>::value) {
                auto camera = std::make_unique<T>(std::forward<Args>(args)...);
                T* cameraPtr = camera.get();
                m_Cameras2D[(name)] = std::move(camera);
                return cameraPtr;
            }
            else if constexpr (std::is_same<T, Camera3D>::value) {
                auto camera = std::make_unique<T>(std::forward<Args>(args)...);
                T* cameraPtr = camera.get();
                m_Cameras3D[(name)] = std::move(camera);
                return cameraPtr;
            }

            return nullptr;
        }

        template <typename T>
        T* GetCamera(const uint32_t& name) {
            if constexpr (std::is_same<T, Camera2D>::value) {
                auto it = m_Cameras2D.find((name));
                if (it != m_Cameras2D.end()) {
                    return dynamic_cast<T*>(it->second.get());
                }
            }
            else if constexpr (std::is_same<T, Camera3D>::value) {
                auto it = m_Cameras3D.find((name));
                if (it != m_Cameras3D.end()) {
                    return dynamic_cast<T*>(it->second.get());
                }
            }

            return nullptr;
        }



        Camera* GetCamera(const std::string& name);
  
        Camera* GetCamera(const uint32_t nameID);





        void RegisterToScripAPI(sol::state& lua) override {};

        //const Camera* GetCurrentCamera() const;

        void SetCurrentCamera(const std::string& name);

        Camera* ReadCamera(const JSON& json);

        Camera* GetCurrentCamera();

        std::vector<std::string> Get2DCameraNames() const;

        std::vector<std::string> Get3DCameraNames() const;

        bool IsWithinFrustum(const glm::vec3& center, const glm::vec3& halfExtents)  const;

        void ResizeCameras(float width, float height);


        glm::vec3 ScreenToWorldPos(const glm::vec2& screenPos);
        glm::vec2 WorldToScreenPos(const glm::vec3& worldPos);

	private:
		std::unordered_map<uint32_t, std::unique_ptr<Camera2D>> m_Cameras2D;
		std::unordered_map<uint32_t, std::unique_ptr<Camera3D>> m_Cameras3D;

        Camera* m_CurrentCamera = nullptr;


        bool OnRuntimeStop(const EngineEditStateEvent& e);
        bool OnRuntimePause(const EnginePauseStateEvent& e);


        NCameraManager(NCameraManager const&);
        NCameraManager& operator=(NCameraManager const&);
	};


}