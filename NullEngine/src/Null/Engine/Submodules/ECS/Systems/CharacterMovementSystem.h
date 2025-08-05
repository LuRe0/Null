#pragma once

//------------------------------------------------------------------------------
//
// File Name:	CharacterMovementSystem.h
// Author(s):	YourName
// 
//------------------------------------------------------------------------------

#include "Null/Core.h"
#include "Null/Engine/Submodules/ECS/Systems/System.h"

namespace NULLENGINE
{
	struct CharacterMovementComponent;

	class NLE_API CharacterMovementSystem: public ISystem < CharacterMovementSystem >
	{
	public:

		CharacterMovementSystem();

		void Load() override;
		void Init() override;
		void Update(float dt) override;
		virtual void RuntimeUpdate(float dt) override;

		void Render() override;

		void Unload() override;
		void Shutdown() override;

		void RegisterToScripAPI(sol::state& lua) override;

	private:

		static void CreateCharacterMovementComponent(void* component, const nlohmann::json& json);
		static void AddCharacterMovementComponent(void* component, NRegistry* registry, EntityID id);
		static JSON WriteCharacterMovementComponent(const void* component);
		static JSON DiffCharacterMovementComponent(const void* base, const void* modified);

		void ViewCharacterMovementComponent(Entity& entity);
	};
}
