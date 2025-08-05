#pragma once

//------------------------------------------------------------------------------
//
// File Name:	AnimatorSystem.h
// Author(s):	YourName
// 
//------------------------------------------------------------------------------

#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"

namespace NULLENGINE
{
	struct AnimatorComponent;
	struct AnimationComponent;
	struct AnimationControllerComponent;

	class NLE_API AnimatorSystem: public ISystem < AnimatorSystem >
	{
	public:

		AnimatorSystem();

		void Load() override;
		void Init() override;
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void Render() override;

		void Unload() override;
		void Shutdown() override;

		void RegisterToScripAPI(sol::state& lua) override;

	private:


		bool PlayClip(Entity entity, uint32_t clipID);

		void SwitchToClip(Entity entity, AnimatorComponent& animator, AnimationComponent& animation);

		void SwitchToClip(AnimatorComponent& animator, AnimationComponent& animation);

		bool CanTransition(const AnimationControllerComponent& controller, uint32_t fromClip, uint32_t toClip);

		static void CreateAnimatorComponent(void* component, const nlohmann::json& json);
		static void AddAnimatorComponent(void* component, NRegistry* registry, EntityID id);
		static JSON WriteAnimatorComponent(const void* component);
		static JSON DiffAnimatorComponent(const void* base, const void* modified);

		void ViewAnimatorComponent(Entity& entity);
	};
}
