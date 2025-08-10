
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
#include "../../../../Tools/ImGuiH.h"
#include "NIncludes.h"

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
			[this](Entity& id) { this->ViewScriptComponent(id); }, WriteScriptComponent,
			AddScriptComponent, DiffScriptComponent, AssignNameToComponent);

		try
		{
			m_LuaState.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io, sol::lib::table, sol::lib::math);
			m_LuaState["math"]["atan2"] = [](double y, double x) {
				return std::atan2(y, x);
				};


			m_LuaState["math"]["deg"] = [](double radians) {
				return radians * (180.0 / M_PI);
				};

			m_LuaState["math"]["rad"] = [](double degrees) {
				return degrees * (M_PI / 180.0);
				};

			m_LuaState["math"]["abs"] = [](double val) {
				return std::abs(val);
				};

			ScriptHelper::SetLuaState(m_LuaState);

		}
		catch (const std::exception&)
		{
			NLE_CORE_ERROR("Could not initialize Lua State");
		}

	}


	void ScriptSystem::Load()
	{
		NScriptingInterface* scriptingInterface = m_Parent->Get< NScriptingInterface>();

		scriptingInterface->RegisterEngineFunctions(m_LuaState);
	}

	void ScriptSystem::Init()
	{
		ISystem::Init();


		//for (const auto entityId : GetSystemEntities())
		//{
		//	InitializeScripts(entityId);
		//}

		NEventManager* eventManager = NEventManager::Instance();

		SUBSCRIBE_EVENT(ScriptCreatedEvent, &ScriptSystem::OnScriptAdded, eventManager, EventPriority::Low);
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

		for (EntityID entityId : GetSystemEntities())
		{
			auto scripts = registry->GetNamedComponents<ScriptComponent>(entityId);

			for (ScriptComponent& scriptComponent : scripts)
			{
				if (!IsValidRuntimeIndex(scriptComponent.overrideID))
					continue;

				ScriptInstance& instance = GetInstance(scriptComponent.tableID);
				sol::table& table = instance.scriptTable;

				if (!table.valid())
					continue;

				sol::function update_func = table["Update"];
				if (update_func.valid())
				{
					try
					{
						update_func(table, dt); // call as method: Update(self, dt)
					}
					catch (const sol::error& e)
					{
						NLE_CORE_ERROR("Lua Error in Update(): {0}", e.what());
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

			auto scripts = registry->GetNamedComponents<ScriptComponent>(entityId);
			auto* sys = ScriptSystem::Instance();

			for (ScriptComponent& script : scripts)
			{
				auto& instance = sys->GetInstance(script.tableID);

				if (instance.scriptTable.valid()) {
					sol::function exit_func = instance.scriptTable["Exit"];
					if (exit_func.valid()) {
						try {
							exit_func(instance.scriptTable);
						}
						catch (const sol::error& e) {
							NLE_CORE_ERROR("Lua Error in Exit(): {0}", e.what());
						}
					}
				}
			}
		}
	}

	void ScriptSystem::RegisterToScripAPI(sol::state& lua)
	{
	}


	void ScriptSystem::CreateScriptComponent(void* component, const nlohmann::json& json)
	{

		// this will remember where to find data as well as assigning instance ids to scripts

		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<ScriptComponent*>(component);
		JsonReader jsonWrapper(json);

		//std::vector<std::string> script_names;

		comp->nameID = STRID(jsonWrapper.GetString("name", ""));
		ComponentFlagSet flags;
		flags.Set(ComponentFlags_Enabled);
		flags.Set(ComponentFlags_Serialized);
		comp->componentFlags.m_Flags = jsonWrapper.GetUInt8("ComponentFlags", flags.m_Flags);

		auto* sys = ScriptSystem::Instance();
		if (!jsonWrapper.Empty())
		{
			if (json.contains("overrides") && json["overrides"].is_object())
			{
				const auto& defaults = json["overrides"][STRFROM(comp->nameID)];
				ScriptOverrides def;

				for (const auto& [varName, value] : defaults.items())
				{
					def[STRID(varName)] = ScriptHelper::JsonToLuaValue(value);
				}

				comp->overrideID = sys->AddScriptOverride(def);
			}


	
		}

	}


	void ScriptSystem::AddScriptComponent(void* component, NRegistry* registry, EntityID id)
	{
		NComponentFactory* componentFactory = NComponentFactory::Instance();

		auto* comp = static_cast<ScriptComponent*>(component);
		componentFactory->AddOrUpdateNamed<ScriptComponent>(id, comp->nameID, comp, registry, comp->nameID, comp->pathID, comp->overrideID, comp->tableID, comp->componentFlags);
	}

	void ScriptSystem::AssignNameToComponent(void* component, const uint32_t& nameID)
	{
		auto* comp = static_cast<ScriptComponent*>(component);

		comp->nameID = nameID;
	}



	nlohmann::json ScriptSystem::WriteScriptComponent(const void* component)
	{
		nlohmann::json json;
		auto* sys = ScriptSystem::Instance();

		const auto& script = *static_cast<const ScriptComponent*>(component);

		if (!script.componentFlags.IsSet(ComponentFlags_Serialized))
			return json;

		nlohmann::json overridesJSON;

		const std::string scriptName = STRFROM(script.nameID);
		const auto& instance = sys->GetInstance(script.tableID);
		// Convert the overrides to a map<string, LuaValue> if needed
		const auto& overrides = sys->GetOverrides(script.overrideID); 


		// Get the script table from the instance:
		const sol::table& scriptTable = instance.scriptTable;


		// Convert uint32_t keys to strings for the defaults map to pass to GenerateScriptDifferences
		std::unordered_map<std::string, LuaValue> defaultsMap;
		for (const auto& [fieldID, val] : overrides)
		{
			defaultsMap[STRFROM(fieldID)] = val;
		}

		// Generate only differences for serialization
		nlohmann::json scriptDiffs = ScriptHelper::GenerateScriptDifferences(scriptTable["data"], defaultsMap);

		overridesJSON[scriptName] = scriptDiffs;

		const std::string outName = "Script:" + scriptName;

		json[outName]["name"] = scriptName;
		json[outName]["overrides"] = overridesJSON; // singular "override" to match your input format
		json[outName]["componentFlags"] = script.componentFlags.m_Flags;

		return json;
	}


	JSON ScriptSystem::DiffScriptComponent(const void* baseComp, const void* modifiedComp)
	{
		JSON diff;

		//auto& baseScript = *static_cast<const ScriptComponent*>(baseComp);
		//auto& modScript = *static_cast<const ScriptComponent*>(modifiedComp);

		//const auto& baseNames = baseScript.m_Script_Names;
		//const auto& modNames = modScript.m_Script_Names;

		//if (baseNames != modNames) {
		//	diff["scripts"] = modNames; // Save the new full list
		//}

		return diff;
	}


	void ScriptSystem::ViewScriptComponent(Entity& entity)
	{
		NScriptingInterface* scriptingInterface = m_Parent->Get< NScriptingInterface>();
		NEventManager* eventManager = NEventManager::Instance();

		auto scripts = entity.GetAll<ScriptComponent>();

		for (ScriptComponent& scriptComponent : scripts)
		{

			uint8_t& flags = scriptComponent.componentFlags.m_Flags;
			// Show collapsible header with enable checkbox and remove button, tied to the Enabled flag
			auto [open, enabled, remove] = ImGuiH::CollapsingHeaderWithFlagCheckboxAndRemove(STRFROM(scriptComponent.nameID) + (" (Script)"), flags, ComponentFlags_Enabled);

			if (remove)
			{
				NEventManager::Instance()->QueueEvent(std::make_unique<EntityRemoveNamedComponentEvent>(entity.GetID(), 
						Component<ScriptComponent>::GetID(), scriptComponent.nameID));
			}


			if (!open)
				return;

			if (!enabled)
				ImGui::BeginDisabled();


			if (IsValidRuntimeIndex(scriptComponent.tableID))
			{
				ScriptInstance& scriptInstance = GetInstance(scriptComponent.tableID);
				sol::table& table = scriptInstance.scriptTable;

				if (!table.valid())
					return;

				std::string scriptName = STRFROM(scriptComponent.nameID);
				std::string scriptPath = STRFROM(scriptComponent.pathID);


					sol::table data = table["data"];
					if (data.valid())
					{
						for (auto& pair : data)
						{
							sol::object key = pair.first;
							sol::table props = pair.second;
							std::string keyStr = pair.first.as<std::string>();
							bool serialize = props["serialize"];
							if (!serialize) continue;

							sol::object value = props["value"];

							LuaValue luaValue = ScriptHelper::GetValue(value);

							ScriptHelper::ImGuiDisplayAndModifyLuaValue(table, data, keyStr, luaValue);

							if (ImGui::IsItemDeactivatedAfterEdit())
							{
								sol::function onChanged = table["OnValueChangedInEditor"];
								if (onChanged.valid())
								{
									try
									{
										onChanged(table, keyStr);
									}
									catch (const sol::error& e)
									{
										NLE_CORE_ERROR("Lua Error: {0}", e.what());
									}
								}
							}
						}
					}

					// Recompile warning
					if (scriptingInterface->GetScriptStatus(scriptName))
						ImGui::TextColored(ImVec4(1, 0, 0, 1), "Script out of date: Recompilation required");

					// Open Script Button
					ImVec2 buttonSize(100, 30);
					ImVec2 windowSize = ImGui::GetWindowSize();
					float buttonX = (windowSize.x - buttonSize.x) * 0.5f;

					ImGui::SetCursorPosX(buttonX);

					if (ImGui::Button("Open Script", buttonSize))
					{
						std::filesystem::path absPath = std::filesystem::absolute(scriptPath);
						std::string fileP = absPath.string();

						if (std::filesystem::exists(absPath))
						{
							if ((intptr_t)ShellExecuteA(nullptr, "open", fileP.c_str(), nullptr, nullptr, SW_SHOWNORMAL) <= 32)
							{
								NLE_CORE_ERROR("Failed to open script: {}", fileP);
							}
						}
						else
						{
							NLE_CORE_ERROR("File does not exist: {}", fileP);
						}
					}


			}

			if (!enabled)
				ImGui::EndDisabled();




			ImGui::TreePop();
		}

		//ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
		//flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
		//flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		//float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		//ImVec2 contentRegion = ImGui::GetContentRegionAvail();

		//if (script.m_Script_Names.empty())
		//{
		//	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Warning: No Scripts Attached");
		//}

		//for (size_t i = 0; i < script.m_Script_Names.size(); i++)
		//{
		//	//ImGui::PushFont
		//	bool opened = ImGui::TreeNodeEx(script.m_Script_Names[i].c_str(), flags);

		//	ImGui::SameLine(contentRegion.x - lineHeight * .25f);
		//	bool removed = false;

		//	// Push the style color for the button
		//	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		//	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
		//	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));


		//	if (ImGui::SmallButton("X"))
		//	{
		//		removed = true;
		//	}

		//	ImGui::PopStyleColor(3);




		//	if (opened)
		//	{

		//		sol::table data = script.m_Scripts[i]["data"];
		//		for (auto& pair : data)
		//		{
		//			sol::object key = pair.first;
		//			sol::table props = pair.second;

		//			sol::object value = props["value"];
		//			bool serialize = props["serialize"];

		//			std::string keyStr = key.as<std::string>();
		//			LuaValue luaValue = ScriptHelper::GetValue(value);

		//			if (serialize)
		//			{
		//				ScriptHelper::ImGuiDisplayAndModifyLuaValue(script.m_Scripts[i], data, keyStr, luaValue);

		//				if (ImGui::IsItemDeactivatedAfterEdit())
		//				{
		//					sol::function _func = script.m_Scripts[i]["OnValueChangedInEditor"];
		//					if (_func.valid()) {

		//						try
		//						{
		//							_func(script.m_Scripts[i], keyStr);
		//						}
		//						catch (const sol::error& e)
		//						{
		//							NLE_CORE_ERROR("Lua Error: {0}", e.what());
		//						}
		//					}
		//				}
		//			}

		//		}



		//		if (scriptingInterface->GetScriptStatus(script.m_Script_Names[i]))
		//			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Script out of date: Recompilation required");

		//		ImGui::TreePop();
		//	}

		//	if (removed)
		//	{
		//		eventManager->QueueEvent(std::make_unique<ScriptRemovedEvent>(entity.m_ID, script.m_Script_Names[i]));
		//	}



		//	// Get the size of the button
		//	ImVec2 buttonSize = ImVec2(100, 30); // Set your desired button size here

		//	// Get the current window size
		//	ImVec2 windowSize = ImGui::GetWindowSize();

		//	// Calculate the position to center the button horizontally
		//	float buttonX = (windowSize.x - buttonSize.x) * 0.5f; // Centering on the x-axis
		//	float buttonY = ImGui::GetCursorPosY(); // Keep the current cursor position on the y-axis

		//	// Set the cursor position to the calculated position
		//	ImGui::SetCursorPos(ImVec2(buttonX, buttonY));

		//	// Create the button
		//	if (ImGui::Button("Open Script", buttonSize))
		//	{
		//		std::filesystem::path relativePath(script.m_Script_Paths[i]);
		//		std::filesystem::path absolutePath = std::filesystem::absolute(relativePath);
		//		std::string fileP = absolutePath.string();

		//		// Check if the file exists
		//		if (std::filesystem::exists(absolutePath))
		//		{
		//			// Attempt to open the file
		//			if (ShellExecuteA(NULL, "open", fileP.c_str(), NULL, NULL, SW_SHOWNORMAL) <= (HINSTANCE)32)
		//			{
		//				NLE_CORE_ERROR("Failed to open script: {}", fileP);
		//			}
		//		}
		//		else
		//		{
		//			NLE_CORE_ERROR("File does not exist: {}", fileP);
		//		}
		//	}
		//	ImGui::Separator();
		//}



		//const auto& names = scriptingInterface->GetScriptNames();
		//if (ImGui::BeginMenu("Add Script"))
		//{


		//	m_ScriptFilter.Draw("##searchbarBehAdd");

		//	ImGui::Separator();


		//	for (auto name : names)
		//	{
		//		if (std::find(script.m_Script_Names.begin(), script.m_Script_Names.end(), name) == script.m_Script_Names.end())
		//			if (m_ScriptFilter.PassFilter(name.c_str()))
		//			{
		//				if (ImGui::MenuItem(name.c_str()))
		//				{
		//					eventManager->QueueEvent(std::make_unique<ScriptCreatedEvent>(entity.m_ID, name));
		//				}
		//			}
		//	}


		//	if (ImGui::MenuItem("New Script"))
		//	{
		//		m_ShowCreationMenu = true;
		//	}
		//	ImGui::EndMenu();

		//}



		//if (m_ShowCreationMenu)
		//{
		//	ImGui::OpenPopup("New Script Name");
		//}


		//if (ImGui::BeginPopupModal("New Script Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		//{
		//	ImGui::Text("Enter the script name:");
		//	ImGui::InputText("##scriptname", &m_ScriptName);

		//	if (ImGui::Button("Create", ImVec2(120, 0)))
		//	{
		//		m_ShowCreationMenu = false; // Close the input box
		//		ImGui::CloseCurrentPopup();

		//		// Use the script_name here
		//		// For example: create a new script with the name entered
		//		scriptingInterface->CreateScript(m_ScriptName);
		//		eventManager->QueueEvent(std::make_unique<ScriptCreatedEvent>(entity.m_ID, m_ScriptName));

		//		/*		std::string fileP = std::filesystem::absolute(p.path()).string();

		//				ShellExecuteA(0, 0, fileP.c_str(), 0, 0, SW_SHOW);*/

		//		m_ScriptName = "New Script";
		//	}
		//	ImGui::SameLine();
		//	if (ImGui::Button("Cancel", ImVec2(120, 0)))
		//	{
		//		m_ShowCreationMenu = false; // Close the input box
		//		m_ScriptName = "New Script";
		//		ImGui::CloseCurrentPopup();
		//	}
		//	ImGui::EndPopup();
		//}

	}
	void ScriptSystem::InitializeScripts(EntityID id)
	{

		NRegistry* registry = NRegistry::Instance();
		auto scripts = registry->GetNamedComponents<ScriptComponent>(id);

		for (const ScriptComponent& script : scripts)
		{
			AddScript(id, script.nameID);
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


	void ScriptSystem::AddScript(EntityID id, uint32_t nameID)
	{

		NScriptingInterface* scriptingInterface = m_Parent->Get< NScriptingInterface>();

		NRegistry* registry = NRegistry::Instance();
		NSceneManager* scene = NSceneManager::Instance();

		ScriptComponent& scriptComponent = registry->GetNamedComponent<ScriptComponent>(id, nameID);

		std::string scriptDirectory = "../Assets/Scripts/";  // Update this path to your script directory



		try
		{
			ScriptSystem* sys = ScriptSystem::Instance();

			const std::string name = STRFROM(scriptComponent.nameID);
			std::string fullPath = GetScriptPaths(scriptDirectory, name);
			scriptComponent.pathID = STRID(fullPath);

			// Create a new Lua environment for this script
			sol::environment env(m_LuaState, sol::create, m_LuaState.globals());
			sol::table script = m_LuaState.script_file(fullPath, env);
			
			// Attach the owning entity to the Lua environment
			auto& entity = scene->GetCurrentScene()->GetEntity(id);
			env["pEntity"] = entity;

			// Prepare script instance and extract defaults
			ScriptInstance instance;
			instance.scriptTable = script;

			//if an override ID is set, we will use it to load the serialized data
			if (IsValidRuntimeIndex(scriptComponent.overrideID))
			{
				ScriptOverrides& overrides = GetOverrides(scriptComponent.overrideID);

				sol::table data = script["data"];
				for (auto& pair : data)
				{
					sol::object key = pair.first;
					sol::table props = pair.second;

					sol::object value = props["value"];
					bool serialize = props["serialize"];

					if (!key.is<std::string>() || value.is<sol::function>() || !serialize)
						continue;

			
					std::string keyStr = key.as<std::string>();

					if (overrides.contains(STRID(keyStr)))
					{
						ScriptHelper::SetValue(script, keyStr, overrides[STRID(keyStr)]);
					}
				}
			}
			else
			{
				ScriptOverrides overrides;

				sol::table data = script["data"];
				for (auto& pair : data)
				{
					sol::object key = pair.first;
					sol::table props = pair.second;

					sol::object value = props["value"];
					bool serialize = props["serialize"];

					if (!key.is<std::string>() || value.is<sol::function>() || !serialize)
						continue;

					std::string keyStr = key.as<std::string>();
					LuaValue luaVal = ScriptHelper::GetValue(value);

					overrides[STRID(keyStr)] = luaVal; // store original value
				}

				// Register the overrides for this script
				scriptComponent.overrideID = sys->AddScriptOverride(overrides);
			}
			


			// Save environment to instance if needed (optional)
			AddEnvironment(id, { env });

			// Register instance and store the ID
			scriptComponent.tableID = sys->AddScriptInstance(std::move(instance));

			// Done — optional: add script watcher
			scriptingInterface->AddScriptWatcher(fullPath, name);

		
			if (script.valid())
			{
				sol::function start_func = script["Start"];
				if (start_func.valid())
				{
					try
					{
						start_func(script);
					}
					catch (const sol::error& e)
					{
						NLE_CORE_ERROR("Lua Error: {0}", e.what());
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			NLE_CORE_ERROR("Error loading script {0}: {1}", STRFROM(scriptComponent.nameID), e.what());
		}
	}

	//void ScriptSystem::RemoveScript(EntityID id, const std::string& script)
	//{
	//	NScriptingInterface* scriptingInterface = m_Parent->Get< NScriptingInterface>();
	//	NRegistry* registry = NRegistry::Instance();
	//	NSceneManager* scene = NSceneManager::Instance();

	//	ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(id);


	//	auto itr = std::find(scriptComponent.m_Script_Names.begin(), scriptComponent.m_Script_Names.end(), script);

	//	if (itr != scriptComponent.m_Script_Names.end())
	//	{
	//		//	m_Lua.script_file(scriptPath);
	//		size_t index = itr - scriptComponent.m_Script_Names.begin();
	//		scriptComponent.m_Script_Names.erase(itr);
	//		scriptComponent.m_ScriptDefaults.erase(script);
	//		scriptComponent.m_Script_Paths.erase(scriptComponent.m_Script_Paths.begin() + index);
	//		scriptComponent.m_Scripts.erase(scriptComponent.m_Scripts.begin() + index);
	//		m_LuaState[script.c_str()] = sol::nil;
	//	}
	//}

	void ScriptSystem::RemoveScript(EntityID id, const uint32_t& scriptName)
	{
		NScriptingInterface* scriptingInterface = m_Parent->Get<NScriptingInterface>();
		NRegistry* registry = NRegistry::Instance();
		ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(id);

		// Ensure the scriptComponent actually matches the script we're trying to remove
		if (scriptName != scriptComponent.nameID)
			return;

		// Remove the script instance (by ID)
		if (IsValidRuntimeIndex(scriptComponent.tableID))
		{
			ScriptInstance& instance = GetInstance(scriptComponent.tableID);
			instance.scriptTable = sol::nil;

			RemoveScriptInstance(scriptComponent.tableID);
		}

		// Remove the shared environment for this entity
		RemoveEnvironment(id);

		// Clear script component state
		scriptComponent.nameID = 0;
		scriptComponent.pathID = 0;
		scriptComponent.tableID = -1;

		// Optionally nil out global Lua table ref (not always needed)
		m_LuaState[STRFROM(scriptName).c_str()] = sol::nil;

		//// Optionally remove any script file watchers
		//scriptingInterface->RemoveScriptWatcher(scriptName);
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
		AddScript(e.GetEntityID(), STRID(e.GetScriptName()));

		return true;

	}


	bool ScriptSystem::OnScriptModified(const ScriptModifiedEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();
		NScriptingInterface* scriptingInterface = m_Parent->Get<NScriptingInterface>();
		uint32_t scriptNameID = STRID(e.GetScriptName());


		for (const auto entityId : GetSystemEntities())
		{
			auto scripts = registry->GetNamedComponents<ScriptComponent>(entityId);
	
			for (const ScriptComponent& scriptComponent : scripts)
			{
				if (scriptComponent.nameID != scriptNameID)
					continue;


				ScriptInstance& instance = GetInstance(scriptComponent.tableID);

				try
				{
					// Save old data values
					sol::table oldData = instance.scriptTable["data"];

					// Reload script
					std::string fullPath = STRFROM(scriptComponent.pathID); // or map scriptNameID → pathID → path
					sol::table newTable = m_LuaState.script_file(fullPath, GetEnvironment(entityId)->env);

					// Carry over matching serialized values
					sol::table newData = newTable["data"];
					for (auto& pair : oldData)
					{
						sol::object key = pair.first;
						sol::table props = pair.second;

						std::string keyStr = key.as<std::string>();
						sol::object oldValue = props["value"];
						bool serialize = props["serialize"];

						if (!serialize || !newData[keyStr].valid())
							continue;

						sol::table newProps = newData[keyStr];
						bool newSerialize = newProps["serialize"];
						if (!newSerialize) continue;

						sol::object newValue = newProps["value"];
						if (oldValue.get_type() == newValue.get_type())
							newProps["value"] = oldValue;
					}

					//Replace the script instance table
					instance.scriptTable = newTable;

					//Call OnReload if defined
					if (newTable.valid())
					{
						sol::function onReload = newTable["OnReload"];
						if (onReload.valid())
						{
							try { onReload(newTable); }
							catch (const sol::error& e)
							{
								NLE_CORE_ERROR("Lua Error in OnReload(): {0}", e.what());
							}
						}
					}

					// Re-register for watching if needed
					scriptingInterface->AddScripts(e.GetScriptName());
				}
				catch (const std::exception& ex)
				{
					NLE_CORE_ERROR("Error reloading script: {0}", ex.what());
				}
			}
		}

		return true;
	}

	bool ScriptSystem::OnScriptRemoved(const ScriptRemovedEvent& e)
	{
		NRegistry* registry = NRegistry::Instance();
		ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(e.GetEntityID());

		RemoveScript(e.GetEntityID(), STRID(e.GetScriptName()));
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
		//NRegistry* registry = NRegistry::Instance();
		//auto* sceneManager = NSceneManager::Instance();
		//auto* scene = sceneManager->GetCurrentScene();
		////auto& entityA = scene->GetEntity(e.GetEntityA()); // Get the Entity directly

		//if (scene->HasEntity(e.GetEntityB()))
		//{
		//	auto& entityB = scene->GetEntity(e.GetEntityB()); // Get the Entity directly

		//	if (registry->HasComponent<ScriptComponent>(e.GetEntityA()))
		//	{
		//		ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(e.GetEntityA());

		//		for (size_t i = 0; i < scriptComponent.m_Scripts.size(); i++)
		//		{
		//			auto& script = scriptComponent.m_Scripts[i];

		//			if (script.valid())
		//			{
		//				sol::function collision_enter_func = script["OnCollisionEnter"];

		//				if (collision_enter_func.valid())
		//				{
		//					collision_enter_func(script, entityB); // Pass the actual Entity
		//				}
		//			}
		//		}
		//	}
		//}

		return true;
	}

	bool ScriptSystem::OnCollisionExit(const CollisionExitEvent& e)
	{
		//NRegistry* registry = NRegistry::Instance();
		//auto* sceneManager = NSceneManager::Instance();
		//auto* scene = sceneManager->GetCurrentScene();
		//if (scene->HasEntity(e.GetEntityB()))
		//{
		//	auto& entityB = scene->GetEntity(e.GetEntityB()); // Get the Entity directly

		//	ScriptComponent& scriptComponent = registry->GetComponent<ScriptComponent>(e.GetEntityA());

		//	for (size_t i = 0; i < scriptComponent.m_Scripts.size(); i++)
		//	{
		//		auto& script = scriptComponent.m_Scripts[i];

		//		if (script.valid())
		//		{
		//			sol::function collision_exit_func = script["OnCollisionExit"];
		//			if (collision_exit_func.valid())
		//			{
		//				collision_exit_func(script, entityB);
		//			}
		//		}
		//	}

		//}
		return true;
	}



	uint32_t ScriptSystem::AddScriptOverride(const ScriptOverrides& overrides)
	{
		if (!m_FreeOverrideIDs.empty()) {
			uint32_t id = m_FreeOverrideIDs.back();
			m_FreeOverrideIDs.pop_back();
			m_ScriptOverrides[id] = std::move(overrides);
			return id;
		}

		m_ScriptOverrides.emplace_back(std::move(overrides));
		return static_cast<uint32_t>(m_ScriptOverrides.size() - 1);
	}

	void ScriptSystem::RemoveScriptOverride(uint32_t tableID) 
	{
		if (tableID >= m_ScriptOverrides.size()) return;

		// Clear Lua reference
		m_ScriptOverrides[tableID] = {};
		m_FreeOverrideIDs.push_back(tableID);

		// Also remove any overrides
	/*	m_ScriptOverrides.erase(tableID);*/
	}

	uint32_t ScriptSystem::AddScriptInstance(ScriptSystem::ScriptInstance instance) 
	{
		if (!m_FreeTableIDs.empty()) {
			uint32_t id = m_FreeTableIDs.back();
			m_FreeTableIDs.pop_back();
			m_ScriptInstances[id] = std::move(instance);
			return id;
		}

		m_ScriptInstances.emplace_back(std::move(instance));
		return static_cast<uint32_t>(m_ScriptInstances.size() - 1);
	}

	void ScriptSystem::RemoveScriptInstance(uint32_t tableID) {
		if (tableID >= m_ScriptInstances.size()) return;

		// Clear Lua reference
		m_ScriptInstances[tableID] = ScriptInstance{};
		m_FreeTableIDs.push_back(tableID);

		// Also remove any overrides
	/*	m_ScriptOverrides.erase(tableID);*/
	}

	ScriptSystem::ScriptInstance& ScriptSystem::GetInstance(uint32_t tableID) {
		NLE_CORE_ASSERT(tableID < m_ScriptInstances.size(), "INVALID SCRIPT INSTANCE ID");
		return m_ScriptInstances[tableID];
	}

	ScriptOverrides& ScriptSystem::GetOverrides(uint32_t overrideID)
	{
		NLE_CORE_ASSERT(overrideID < m_ScriptOverrides.size(), "INVALID SCRIPT Override ID");
		return m_ScriptOverrides[overrideID];
	}

	void ScriptSystem::AddEnvironment(EntityID entityID, EntityScriptEnv env) {
		m_ScriptEnvironments[entityID] = std::move(env);
	}

	void ScriptSystem::RemoveEnvironment(EntityID entityID) {
		m_ScriptEnvironments.erase(entityID);
	}

	ScriptSystem::EntityScriptEnv* ScriptSystem::GetEnvironment(EntityID entityID) {
		auto it = m_ScriptEnvironments.find(entityID);
		return it != m_ScriptEnvironments.end() ? &it->second : nullptr;
	}


}