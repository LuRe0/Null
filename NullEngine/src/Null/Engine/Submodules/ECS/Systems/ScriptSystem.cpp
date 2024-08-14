
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

		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

		componentFactory->Register<ScriptComponent>(CreateScriptComponent,
			[this](Entity& id) { this->ViewScriptComponent(id); }, WriteScriptComponent);

	}


	void ScriptSystem::Load()
	{
		NScriptingInterface* scriptingInterface = m_Parent->Get< NScriptingInterface>();
		try
		{
			m_LuaState.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io);
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


		for (const auto entityId : GetSystemEntities())
		{
			InitializeScripts(entityId);
		}

		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		SUBSCRIBE_EVENT(ScriptCreatedEvent, &ScriptSystem::OnScriptAdded, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(ScriptModifiedEvent, &ScriptSystem::OnScriptModified, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(ScriptRemovedEvent, &ScriptSystem::OnScriptRemoved, eventManager, EventPriority::High);
		SUBSCRIBE_EVENT(SceneSwitchEvent, &ScriptSystem::OnSceneSwitched, eventManager, EventPriority::Low);


	}

	void ScriptSystem::Update(float dt)
	{

	}

	void ScriptSystem::RuntimeUpdate(float dt)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();


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
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();

		for (const auto entityId : GetSystemEntities())
		{

			ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(entityId);

			for (auto& script : scriptComponent.m_Scripts) {
				if (script.valid()) {
					sol::function exit_func = script["Exit"];
					if (exit_func.valid()) {
						exit_func();
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
		NComponentFactory* componentFactory = NEngine::Instance().Get<NComponentFactory>();

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
			const sol::table& scriptTable = script.m_Scripts[i]["data"];

			nlohmann::json scriptDiffs = ScriptHelper::GenerateScriptDifferences(scriptTable, defaults);
			defaultsJSON["scripts"][scriptName] = scriptDiffs;
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
		NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();

		ScriptComponent& script = entity.Get<ScriptComponent>();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen  |  ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
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
				sol::table scr = script.m_Scripts[i];

				sol::table data = scr["data"];
				for (auto& pair : data)
				{
					sol::object key = pair.first;
					sol::table props = pair.second;

					sol::object value = props["value"];
					bool serialize = props["serialize"];

					std::string keyStr = key.as<std::string>();
					LuaValue luaValue = ScriptHelper::GetValue(value);

					if (serialize)
						ScriptHelper::ImGuiDisplayAndModifyLuaValue(scr, data, keyStr, luaValue);

				}



				if (scriptingInterface->GetScriptStatus(script.m_Script_Names[i]))
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Script out of date: Recompilation required");

				ImGui::TreePop();
			}

			if (removed)
			{
				eventManager->QueueEvent(std::make_unique<ScriptRemovedEvent>(entity.m_ID, script.m_Script_Names[i]));
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

		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
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

		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NSceneManager* scene = NEngine::Instance().Get<NSceneManager>();

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

			scriptComponent.m_Environment["pEntity"] = &scene->GetCurrentScene()->GetEntity(id);

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


		auto newScr = scriptComponent.m_Scripts.back();
		if (newScr.valid())
		{
			sol::function start_func = newScr["Start"];
			if (start_func.valid()) {
				start_func();
			}
		}
	}

	void ScriptSystem::RemoveScript(EntityID id, const std::string& script)
	{
		NScriptingInterface* scriptingInterface = m_Parent->Get< NScriptingInterface>();
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		NSceneManager* scene = NEngine::Instance().Get<NSceneManager>();

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

	void ScriptSystem::OnScriptAdded(const ScriptCreatedEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(e.GetEntityID());
		scriptComponent.m_Script_Names.push_back(e.GetScriptName());
		AddScript(e.GetEntityID(), e.GetScriptName());
	}


	void ScriptSystem::OnScriptModified(const ScriptModifiedEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
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
								newProps["value"] = oldValue;
						}
					}

					scriptComponent.m_Scripts[index] = table;

					scriptingInterface->AddScripts(e.GetScriptName());
				}
				catch (const std::exception& ex)
				{
					NLE_CORE_ERROR("Error loading Script: {0}", ex.what());
				}

			}
		}
	}

	void ScriptSystem::OnScriptRemoved(const ScriptRemovedEvent& e)
	{
		NRegistry* registry = NEngine::Instance().Get<NRegistry>();
		ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(e.GetEntityID());

		RemoveScript(e.GetEntityID(), e.GetScriptName());
	}

	void ScriptSystem::OnSceneSwitched(const SceneSwitchEvent& e)
	{
		for (const auto entityId : GetSystemEntities())
		{
			InitializeScripts(entityId);
		}
	}

}