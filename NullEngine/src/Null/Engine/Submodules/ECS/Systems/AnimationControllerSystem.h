#pragma once

//------------------------------------------------------------------------------
//
// File Name:	AnimationControllerSystem.h
// Author(s):	YourName
// 
//------------------------------------------------------------------------------

#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"

namespace NULLENGINE
{
	struct AnimationControllerComponent;

	class NLE_API AnimationControllerSystem: public ISystem < AnimationControllerSystem >
	{
	public:

		AnimationControllerSystem();

		void Load() override;
		void Init() override;
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void Render() override;

		void Unload() override;
		void Shutdown() override;

		void RegisterToScripAPI(sol::state& lua) override;

	private:

		static void CreateAnimationControllerComponent(void* component, const nlohmann::json& json);
		static void AddAnimationControllerComponent(void* component, NRegistry* registry, EntityID id);
		static JSON WriteAnimationControllerComponent(const void* component);
		static JSON DiffAnimationControllerComponent(const void* base, const void* modified);

		void ViewAnimationControllerComponent(Entity& entity);
	};
}
