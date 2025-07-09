
//------------------------------------------------------------------------------
//
// File Name:	ScriptSystem.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "ScriptSystem.h"
#include "imgui.h"
#include "Null/Engine/Submodules/Events/IEvents.h"
#include "Null/Tools/ScriptHelper.h"
#include <sol/sol.hpp>
#include <misc/cpp/imgui_stdlib.h>
#include <shellapi.h>
#include "../Entities/Entity.h"
#include "../../Scene.h"

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

namespace fs = std::filesystem;

using LuaValue = std::variant<float, int, std::string, bool, sol::table, sol::function, std::monostate>;

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//
namespace NULLENGINE
{





	ScriptSystem::ScriptSystem()
	{
		Require<ScriptComponent>();

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		componentFactory->Register<ScriptComponent>(CreateScriptComponent,
			[this](Entity& id) { this->ViewScriptComponent(id); }, WriteScriptComponent);

	}


	void ScriptSystem::Load()
	{
		NScriptingInterface* scriptingInterface = m_Parent->Get< NScriptingInterface>();
		try
		{
			m_LuaState.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io, sol::lib::table, sol::lib::math);

			ScriptHelper::SetLuaState(m_LuaState);

			scriptingInterface->RegisterEngineFunctions(m_LuaState);
		}
		catch (const std::exception&)
		{
			NLE_CORE_ERROR("Could not initialize Lua State");
		}
	}

	void ScriptSystem::Init()
	{
		ISystem::Init();


		//for (const auto entityId : GetSystemEntities())
		//{
		//	InitializeScripts(entityId);
		//}

		NEventManager* eventManager =   NEventManager::Instance();

		SUBSCRIBE_EVENT(ScriptCreatedEvent, &ScriptSystem::OnScriptAdded, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(ScriptModifiedEvent, &ScriptSystem::OnScriptModified, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(ScriptRemovedEvent, &ScriptSystem::OnScriptRemoved, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(SceneSwitchEvent, &ScriptSystem::OnSceneSwitched, eventManager, EventPriority::Low);
		SUBSCRIBE_EVENT(CollisionEnterEvent, &ScriptSystem::OnCollisionEnter, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(CollisionExitEvent, &ScriptSystem::OnCollisionExit, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(EntityCreatedEvent, &ScriptSystem::OnEntityCreated, eventManager, EventPriority::Low);




		NRegistry* registry = NRegistry::Instance();

	}

	void ScriptSystem::Update(float dt)
	{

	}

	void ScriptSystem::RuntimeUpdate(float dt)
	{
		NRegistry* registry = NRegistry::Instance();


		for (const auto entityId : GetSystemEntities())
		{
			ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(entityId);

			for (auto& script : scriptComponent.m_Scripts) {
				if (script.valid())
				{
					sol::function update_func = script["Update"];
					if (update_func.valid())
					{
						try
						{
							// Call the Lua Update function with the entity and delta time
							update_func(script, dt);
						}
						catch (const sol::error& e)
						{
							NLE_CORE_ERROR("Lua Error: {0}", e.what());
						}
					}
				}
			}
		}
	}


	void ScriptSystem::Render()
	{
	}

	void ScriptSystem::Unload()
	{
	}

	void ScriptSystem::Shutdown()
	{
		NRegistry* registry = NRegistry::Instance();

		for (const auto entityId : GetSystemEntities())
		{

			ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(entityId);

			for (auto& script : scriptComponent.m_Scripts) {
				if (script.valid()) {
					sol::function exit_func = script["Exit"];
					if (exit_func.valid()) {
					
						try
						{
							exit_func(script);
						}
						catch (const sol::error& e)
						{
							NLE_CORE_ERROR("Lua Error: {0}", e.what());
						}
					}
				}
			}
		}
	}

	void ScriptSystem::RegisterToScripAPI(sol::state& lua)
	{
	}


	void ScriptSystem::CreateScriptComponent(void* component, const nlohmann::json& json, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<ScriptComponent*>(component);
		JsonReader jsonWrapper(json);

		//std::vector<std::string> script_names;

		if (!jsonWrapper.Empty())
		{

			if (json.contains("scripts"))
			{
				for (const auto& [scriptName, Defaults] : json["scripts"].items())
				{
					for (const auto& [varName, value] : Defaults.items())
					{
						comp->m_ScriptDefaults[scriptName][varName] = ScriptHelper::JsonToLuaValue(value);
					}

					comp->m_Script_Names.push_back(scriptName);
				}
			}
			else
			{
				NLE_CORE_ERROR("'scripts' key is missing or not an array.");
			}
		}

		componentFactory->AddOrUpdate<ScriptComponent>(id, comp, registry, comp->m_Script_Names, comp->m_ScriptDefaults);
	}



	JSON ScriptSystem::WriteScriptComponent(BaseComponent* component)
	{
		nlohmann::json json;

		auto& script = *static_cast<ScriptComponent*>(component);

		//json["Script"]["scripts"] = script.m_Script_Names;


		JSON defaultsJSON;

		for (size_t i = 0; i < script.m_Script_Names.size(); ++i) {
			const std::string& scriptName = script.m_Script_Names[i];
			const auto& defaults = script.m_ScriptDefaults.at(scriptName);

			if (!script.m_Scripts.empty())
			{
				const sol::table& scriptTable = script.m_Scripts[i]["data"];

				nlohmann::json scriptDiffs = ScriptHelper::GenerateScriptDifferences(scriptTable, defaults.toMap());
				defaultsJSON["scripts"][scriptName] = scriptDiffs;
			}
		}


		json["Script"] = defaultsJSON;


		//for (size_t i = 0; i < script.m_Script_Names.size(); i++)
		//{
		//	sol::table scr = script.m_Scripts[i];

		//	for (auto& pair : scr)
		//	{
		//		sol::object key = pair.first;
		//		sol::object value = pair.second;

		//		std::string keyStr = key.as<std::string>();
		//		LuaValue luaValue = GetValue(value);
		//		ImGuiDisplayAndModifyLuaValue(scr, keyStr, luaValue);
		//	}
		//}

		return json;
	}

	void ScriptSystem::ViewScriptComponent(Entity& entity)
	{
		NScriptingInterface* scriptingInterface = m_Parent->Get< NScriptingInterface>();
		NEventManager* eventManager =   NEventManager::Instance();

		ScriptComponent& script = entity.Get<ScriptComponent>();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
		flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 contentRegion = ImGui::GetContentRegionAvail();

		if (script.m_Script_Names.empty())
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: No Scripts Attached");
		}

		for (size_t i = 0; i < script.m_Script_Names.size(); i++)
		{
			//ImGui::PushFont
			bool opened = ImGui::TreeNodeEx(script.m_Script_Names[i].c_str(), flags);

			ImGui::SameLine(contentRegion.x - lineHeight * .25f);
			bool removed = false;

			// Push the style color for the button
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));


			if (ImGui::SmallButton("X"))
			{
				removed = true;
			}

			ImGui::PopStyleColor(3);




			if (opened)
			{
	
				sol::table data = script.m_Scripts[i]["data"];
				for (auto& pair : data)
				{
					sol::object key = pair.first;
					sol::table props = pair.second;

					sol::object value = props["value"];
					bool serialize = props["serialize"];

					std::string keyStr = key.as<std::string>();
					LuaValue luaValue = ScriptHelper::GetValue(value);

					if (serialize)
					{
						ScriptHelper::ImGuiDisplayAndModifyLuaValue(script.m_Scripts[i], data, keyStr, luaValue);

						if (ImGui::IsItemDeactivatedAfterEdit()) 
						{
							sol::function _func = script.m_Scripts[i]["OnValueChangedInEditor"];
							if (_func.valid()) {

								try
								{
									_func(script.m_Scripts[i], keyStr);
								}
								catch (const sol::error& e)
								{
									NLE_CORE_ERROR("Lua Error: {0}", e.what());
								}
							}
						}
					}

				}



				if (scriptingInterface->GetScriptStatus(script.m_Script_Names[i]))
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Script out of date: Recompilation required");

				ImGui::TreePop();
			}

			if (removed)
			{
				eventManager->QueueEvent(std::make_unique<ScriptRemovedEvent>(entity.m_ID, script.m_Script_Names[i]));
			}



			// Get the size of the button
			ImVec2 buttonSize = ImVec2(100, 30); // Set your desired button size here

			// Get the current window size
			ImVec2 windowSize = ImGui::GetWindowSize();

			// Calculate the position to center the button horizontally
			float buttonX = (windowSize.x - buttonSize.x) * 0.5f; // Centering on the x-axis
			float buttonY = ImGui::GetCursorPosY(); // Keep the current cursor position on the y-axis

			// Set the cursor position to the calculated position
			ImGui::SetCursorPos(ImVec2(buttonX, buttonY));

			// Create the button
			if (ImGui::Button("Open Script", buttonSize))
			{
				std::filesystem::path relativePath(script.m_Script_Paths[i]);
				std::filesystem::path absolutePath = std::filesystem::absolute(relativePath);
				std::string fileP = absolutePath.string();

				// Check if the file exists
				if (std::filesystem::exists(absolutePath))
				{
					// Attempt to open the file
					if (ShellExecuteA(NULL, "open", fileP.c_str(), NULL, NULL, SW_SHOWNORMAL) <= (HINSTANCE)32)
					{
						NLE_CORE_ERROR("Failed to open script: {}", fileP);
					}
				}
				else
				{
					NLE_CORE_ERROR("File does not exist: {}", fileP);
				}
			}
			ImGui::Separator();
		}



		const auto& names = scriptingInterface->GetScriptNames();
		if (ImGui::BeginMenu("Add Script"))
		{
			static ImGuiTextFilter filterBeh;


			filterBeh.Draw("##searchbarBehAdd");

			ImGui::Separator();


			for (auto name : names)
			{
				if (std::find(script.m_Script_Names.begin(), script.m_Script_Names.end(), name) == script.m_Script_Names.end())
					if (filterBeh.PassFilter(name.c_str()))
					{
						if (ImGui::MenuItem(name.c_str()))
						{
							eventManager->QueueEvent(std::make_unique<ScriptCreatedEvent>(entity.m_ID, name));
						}
					}
			}


			if (ImGui::MenuItem("New Script"))
			{
				m_ShowCreationMenu = true;
			}
			ImGui::EndMenu();

		}



		if (m_ShowCreationMenu)
		{
			ImGui::OpenPopup("New Script Name");
		}


		if (ImGui::BeginPopupModal("New Script Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Enter the script name:");
			ImGui::InputText("##scriptname", &m_ScriptName);

			if (ImGui::Button("Create", ImVec2(120, 0)))
			{
				m_ShowCreationMenu = false; // Close the input box
				ImGui::CloseCurrentPopup();

				// Use the script_name here
				// For example: create a new script with the name entered
				scriptingInterface->CreateScript(m_ScriptName);
				eventManager->QueueEvent(std::make_unique<ScriptCreatedEvent>(entity.m_ID, m_ScriptName));

				/*		std::string fileP = std::filesystem::absolute(p.path()).string();

						ShellExecuteA(0, 0, fileP.c_str(), 0, 0, SW_SHOW);*/

				m_ScriptName = "New Script";
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				m_ShowCreationMenu = false; // Close the input box
				m_ScriptName = "New Script";
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

	}
	void ScriptSystem::InitializeScripts(EntityID id)
	{

		NRegistry* registry = NRegistry::Instance();
		ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(id);


		for (const auto& name : scriptComponent.m_Script_Names)
		{
			AddScript(id, name);
		}

	}

	const std::string ScriptSystem::GetScriptPaths(const std::string& scriptDirectory, const std::string& scriptName)
	{


		for (const auto& entry : fs::recursive_directory_iterator(scriptDirectory))
		{
			if (entry.is_regular_file()) {
				// Compare the filename without extension
				if (entry.path().filename().stem() == scriptName)
				{
					auto path = entry.path();
					auto pathStr = entry.path().string();
					return pathStr; // Return the full path if filename matches
				}
			}
		}


		return std::string();
	}


	void ScriptSystem::AddScript(EntityID id, const std::string& scriptname)
	{

		NScriptingInterface* scriptingInterface = m_Parent->Get< NScriptingInterface>();

		NRegistry* registry = NRegistry::Instance();
		NSceneManager* scene = NSceneManager::Instance();

		ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(id);

		std::string scriptDirectory = "../Assets/Scripts/";  // Update this path to your script directory



		try
		{
			std::string fullPath = GetScriptPaths(scriptDirectory, scriptname);

			// Load the script using require_file
			scriptComponent.m_Script_Paths.push_back(fullPath);

			scriptComponent.m_Environment = sol::environment(m_LuaState, sol::create, m_LuaState.globals());
			sol::table script = m_LuaState.script_file(fullPath, scriptComponent.m_Environment);
			//scriptComponent.m_Script_Names.push_back(scriptname);

			auto& pEntity = scene->GetCurrentScene()->GetEntity(id);
			scriptComponent.m_Environment["pEntity"] = pEntity;




			sol::table data = script["data"];
			for (auto& pair : data)
			{
				sol::object key = pair.first;
				sol::table props = pair.second;

				sol::object value = props["value"];
				bool serialize = props["serialize"];

				std::string keyStr = key.as<std::string>();
				LuaValue luaValue = ScriptHelper::GetValue(value);

				if (value.is<sol::function>() || !serialize)
					continue;


				if (scriptComponent.m_ScriptDefaults.empty())
				{
					scriptComponent.m_ScriptDefaults[scriptname][keyStr] = luaValue;
					continue;
				}

				// Get the script default for the given name, creating it if it doesn't exist
				auto& defaultsForName = scriptComponent.m_ScriptDefaults[scriptname];

				// Check if the key exists in the defaults for the name
				if (defaultsForName.contains(keyStr))
				{
					// If the value has changed, set it; otherwise, update the default
					if (defaultsForName.at(keyStr) != luaValue)
					{
						ScriptHelper::SetValue(script, keyStr, defaultsForName.at(keyStr));
					}
					else
					{
						defaultsForName[keyStr] = luaValue;
					}
				}
				else
				{
					defaultsForName[keyStr] = luaValue;
				}


			}

			scriptingInterface->AddScriptWatcher(scriptComponent.m_Script_Paths.back(), scriptname);
			//scriptingInterface->AddScriptWatcher(scriptComponent.m_Script_Paths.back(), scriptname);
			//scriptingInterface->AddScriptWatcher(scriptComponent.m_Script_Paths.back(), scriptname);

			scriptComponent.m_Scripts.emplace_back(script);

		}
		catch (const std::exception& e)
		{
			NLE_CORE_ERROR("Error loading script {0}: {1}", scriptname, e.what());
		}


		auto& newScr = scriptComponent.m_Scripts.back();
		if (newScr.valid())
		{
			sol::function start_func = newScr["Start"];
			if (start_func.valid()) 
			{
				try
				{
					start_func(newScr);
				}
				catch (const sol::error& e)
				{
					NLE_CORE_ERROR("Lua Error: {0}", e.what());
				}
			}
		}
	}

	void ScriptSystem::RemoveScript(EntityID id, const std::string& script)
	{
		NScriptingInterface* scriptingInterface = m_Parent->Get< NScriptingInterface>();
		NRegistry* registry = NRegistry::Instance();
		NSceneManager* scene = NSceneManager::Instance();

		ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(id);


		auto itr = std::find(scriptComponent.m_Script_Names.begin(), scriptComponent.m_Script_Names.end(), script);

		if (itr != scriptComponent.m_Script_Names.end())
		{
			//	m_Lua.script_file(scriptPath);
			size_t index = itr - scriptComponent.m_Script_Names.begin();
			scriptComponent.m_Script_Names.erase(itr);
			scriptComponent.m_ScriptDefaults.erase(script);
			scriptComponent.m_Script_Paths.erase(scriptComponent.m_Script_Paths.begin() + index);
			scriptComponent.m_Scripts.erase(scriptComponent.m_Scripts.begin() + index);
			m_LuaState[script.c_str()] = sol::nil;
		}
	}

	bool ScriptSystem::OnEntityCreated(const EntityCreatedEvent& e)
	{
		const auto& entityList = GetSystemEntities();

		if (std::find(entityList.begin(), entityList.end(), e.GetID()) != entityList.end())
			InitializeScripts(e.GetID());

		return true;
	}

	bool ScriptSystem::OnScriptAdded(const ScriptCreatedEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();
		ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(e.GetEntityID());
		scriptComponent.m_Script_Names.push_back(e.GetScriptName());
		AddScript(e.GetEntityID(), e.GetScriptName());

		return true;

	}


	bool ScriptSystem::OnScriptModified(const ScriptModifiedEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();
		NScriptingInterface* scriptingInterface = m_Parent->Get< NScriptingInterface>();

		for (const auto entityId : GetSystemEntities())
		{

			ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(entityId);

			auto itr = std::find(scriptComponent.m_Script_Names.begin(), scriptComponent.m_Script_Names.end(), e.GetScriptName());

			if (itr != scriptComponent.m_Script_Names.end())
			{
				//	m_Lua.script_file(scriptPath);
				size_t index = itr - scriptComponent.m_Script_Names.begin();

				try
				{
					//scriptComponent.m_Lua_state["package"]["loaded"][e.GetScriptName()] = sol::nil;

					sol::table oldData = scriptComponent.m_Scripts[index]["data"];

					sol::table table = m_LuaState.script_file(scriptComponent.m_Script_Paths[index]);

					sol::table newData = table["data"];

					for (auto& pair : oldData)
					{
						sol::object key = pair.first;
						sol::table props = pair.second;

						sol::object oldValue = props["value"];
						bool serialize = props["serialize"];

						std::string keyStr = key.as<std::string>();

						if (newData[keyStr].valid())
						{
							sol::table newProps = newData[keyStr];
							bool serialize = newProps["serialize"];
							if (serialize)
							{
								sol::object newValue = newProps["value"];
								if (oldValue.get_type() == newValue.get_type())
								{
									newProps["value"] = oldValue;
								}
								else
								{
									newProps["value"] = newValue;
								}
							}
						}
					}

					scriptComponent.m_Scripts[index] = table;

					auto& newScr = scriptComponent.m_Scripts[index];
					if (newScr.valid())
					{

						sol::function _func = newScr["OnReload"];
						if (_func.valid())
						{
							try
							{
								_func(newScr);
							}
							catch (const sol::error& e)
							{
								NLE_CORE_ERROR("Lua Error: {0}", e.what());
							}
						}
					}

					scriptingInterface->AddScripts(e.GetScriptName());
				}
				catch (const std::exception& ex)
				{
					NLE_CORE_ERROR("Error loading Script: {0}", ex.what());
				}

			}
		}
		return true;
	}

	bool ScriptSystem::OnScriptRemoved(const ScriptRemovedEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();
		ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(e.GetEntityID());

		RemoveScript(e.GetEntityID(), e.GetScriptName());
		return true;
	}

	bool ScriptSystem::OnSceneSwitched(const SceneSwitchEvent& e)
	{
		for (const auto entityId : GetSystemEntities())
		{
			InitializeScripts(entityId);
		}

		return true;
	}

	bool ScriptSystem::OnCollisionEnter(const CollisionEnterEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();
		auto* sceneManager = NSceneManager::Instance();
		auto* scene = sceneManager->GetCurrentScene();
		//auto& entityA = scene->GetEntity(e.GetEntityA()); // Get the Entity directly

		if (scene->HasEntity(e.GetEntityB()))
		{
			auto& entityB = scene->GetEntity(e.GetEntityB()); // Get the Entity directly

			if (registry->HasComponent<ScriptComponent>(e.GetEntityA()))
			{
				ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(e.GetEntityA());

				for (size_t i = 0; i < scriptComponent.m_Scripts.size(); i++)
				{
					auto& script = scriptComponent.m_Scripts[i];

					if (script.valid())
					{
						sol::function collision_enter_func = script["OnCollisionEnter"];

						if (collision_enter_func.valid())
						{
							collision_enter_func(script, entityB); // Pass the actual Entity
						}
					}
				}
			}
		}

		return true;
	}

	bool ScriptSystem::OnCollisionExit(const CollisionExitEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();
		auto* sceneManager = NSceneManager::Instance();
		auto* scene = sceneManager->GetCurrentScene();
		if (scene->HasEntity(e.GetEntityB()))
		{
			auto& entityB = scene->GetEntity(e.GetEntityB()); // Get the Entity directly

			ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(e.GetEntityA());

			for (size_t i = 0; i < scriptComponent.m_Scripts.size(); i++)
			{
				auto& script = scriptComponent.m_Scripts[i];

				if (script.valid())
				{
					sol::function collision_exit_func = script["OnCollisionExit"];
					if (collision_exit_func.valid())
					{
						collision_exit_func(script, entityB);
					}
				}
			}

		}
		return true;
	}

}