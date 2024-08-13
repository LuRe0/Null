
//------------------------------------------------------------------------------
//
// File Name:	NScriptingInterface.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NScriptingInterface.h"
#include "Null/Tools/Trace.h"
#include <nlohmann/json.hpp>
#include "Null/Engine/Submodules/ECS/Entities/Entity.h"
#include "FileWatch.hpp"
using JSON = nlohmann::json;

//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//



namespace NULLENGINE
{
    static std::string TemplatedScript = R"(
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local Template_Script = {
    data = 
    {
        name = { value = "Template_Script", serialize = true }
    }
}

setmetatable(Template_Script, {
    __index = function(t, k)
        if Template_Script.data[k] then
            return Template_Script.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if Template_Script.data[k] then
            Template_Script.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function Template_Script:Start()
    -- print("Template_Script started")
    -- Initialization code here
end

function Template_Script:Update(dt)
    -- print("Template_Script started")
    -- Update code here
end

function Template_Script:Exit()
    -- print("Template_Script exited")
    -- Cleanup code here
end

return Template_Script
)";


    void replaceAll(std::string& subject, const std::string& search, const std::string& replace) 
    {
 
    }

	void NScriptingInterface::Load()
	{
        std::string scriptDirectory = "../Assets/Scripts/";  // Update this path to your script directory

        for (const auto& entry : std::filesystem::recursive_directory_iterator(scriptDirectory))
        {
            if (entry.is_regular_file()) {
                // Compare the filename without extension
                if (entry.path().filename().extension() == ".lua")
                {
                    AddScripts(entry.path().stem().string());
                }
            }
        }
	}
	void NScriptingInterface::Init()
	{
        //CreateScript("Player_Shoot");
   
	}

    void NScriptingInterface::Update(float dt)
    {

        switch (NEngine::Instance().GetEngineState())
        {
        case NULLENGINE::IEngine::RUN_MAXIMIZED: // Handle maximized state
        case NULLENGINE::IEngine::RUN_WINDOWED:  // Handle windowed state
        case NULLENGINE::IEngine::SIMULATE:
        {
            for (auto& scripts : m_ScriptList)
            {
                if (scripts.second)
                {
                    ReloadScript(scripts.first);
                }
            }
            break;
        }
        default:
            break;
        }
    }

    void NScriptingInterface::RuntimeUpdate(float dt)
    {
        Update(dt);
    }

	void NScriptingInterface::RegisterEngineFunctions(sol::state& lua)
	{
		const auto& modules = m_Parent->GetModules();
		Input::RegisterToScripAPI(lua);
		Trace::RegisterToScripAPI(lua);
		RegisterMathStructures(lua);
      
		for (const auto& mod : modules)
		{
			mod.first->RegisterToScripAPI(lua);
		}

	}
    void NScriptingInterface::CreateScript(const std::string& filename)
    {
        std::string filePath = std::string("../Assets/Scripts/") + filename + std::string(".lua");

        std::ofstream outFile(filePath);

        // Check if the file was successfully opened
        if (!outFile) {
            std::cerr << "Error opening file for writing: " << filename << std::endl;
            return; // Return an error code
        }

        std::string luaScript = TemplatedScript;

        //std::string text = "Quick brown fox";
        std::regex temp("Template_Script");

        // write the results to an output iterator
        luaScript =  std::regex_replace(luaScript, temp, filename);

        // Write the Lua script to the file
        outFile << luaScript;

        // Close the file
        outFile.close();


        //AddScriptWatcher(filePath, filename);
    }
    void NScriptingInterface::ReloadScript(const std::string& filename)
    {
        NEventManager* eventManager = NEngine::Instance().Get<NEventManager>();
        eventManager->QueueEvent(std::make_unique<ScriptModifiedEvent>(filename));
    }
    void NScriptingInterface::AddScriptWatcher(const std::string filepath, const std::string& filename)
    {
        AddScripts(filename);

        std::unique_ptr<filewatch::FileWatch<std::wstring>> watcher = std::make_unique<filewatch::FileWatch<std::wstring>>(
            std::filesystem::path(filepath).parent_path().wstring(),
            [this, filename](const std::wstring& path, const filewatch::Event change_type) {
                if (change_type == filewatch::Event::modified) 
                {
                    //std::wcout << L"Script file modified: " << path << std::endl;
                    UpdateScript(filename, true);
                }
            });

        m_ScriptReloaders.push_back(std::move(watcher));
    }
    void NScriptingInterface::AddScripts(const std::string& filename)
    {
        m_ScriptList[filename] = false;
    }

    void NScriptingInterface::UpdateScript(const std::string& filename, bool m)
    {
        m_ScriptList[filename] = m;
    }

    bool NScriptingInterface::GetScriptStatus(const std::string& filename)
    {
        return m_ScriptList[filename];
    }

    void NScriptingInterface::RegisterMathStructures(sol::state& lua)
    {
        RegisterVec2Structures(lua);
        RegisterVec3Structures(lua);
        RegisterVec4Structures(lua);



        //// Register glm::mat4
        //lua.new_usertype<glm::mat4>("mat4",
        //    sol::constructors<glm::mat4(), glm::mat4(float)>(),
        //    "operator*", sol::overload(
        //        [](const glm::mat4& lhs, const glm::mat4& rhs) { return lhs * rhs; },
        //        [](const glm::mat4& lhs, const glm::vec4& rhs) { return lhs * rhs; }
        //    ),
        //    "operator+", sol::overload(
        //        [](const glm::mat4& lhs, const glm::mat4& rhs) { return lhs + rhs; }
        //    )
        //    // Add more operators or methods if needed
        //);
	}

    void NScriptingInterface::RegisterVec2Structures(sol::state& lua)
    {

        // Register glm::vec2


        auto vec2_multiply_overloads = sol::overload(
            [](const glm::vec2& v1, const glm::vec2& v2) { return v1 * v2; },
            [](const glm::vec2& v1, float value) { return v1 * value; },
            [](float value, const glm::vec2& v1) { return v1 * value; }
        );

        auto vec2_divide_overloads = sol::overload(
            [](const glm::vec2& v1, const glm::vec2& v2) { return v1 / v2; },
            [](const glm::vec2& v1, float value) { return v1 / value; },
            [](float value, const glm::vec2& v1) { return v1 / value; }
        );

        auto vec2_addition_overloads = sol::overload(
            [](const glm::vec2& v1, const glm::vec2& v2) { return v1 + v2; },
            [](const glm::vec2& v1, float value) { return v1 + value; },
            [](float value, const glm::vec2& v1) { return v1 + value; }
        );

        auto vec2_substraction_overloads = sol::overload(
            [](const glm::vec2& v1, const glm::vec2& v2) { return v1 - v2; },
            [](const glm::vec2& v1, float value) { return v1 - value; },
            [](float value, const glm::vec2& v1) { return v1 - value; }
        );


        lua.new_usertype<glm::vec2>(
            "vec2",
            sol::call_constructor,
            sol::constructors<glm::vec2(float), glm::vec2(float, float)>(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y,
            sol::meta_function::multiplication, vec2_multiply_overloads,
            sol::meta_function::division, vec2_divide_overloads,
            sol::meta_function::addition, vec2_addition_overloads,
            sol::meta_function::subtraction, vec2_substraction_overloads
        );
    }

    void NScriptingInterface::RegisterVec3Structures(sol::state& lua)
    {
        // Register glm::vec3
        auto vec3_multiply_overloads = sol::overload(
            [](const glm::vec3& v1, const glm::vec3& v2) { return v1 * v2; },
            [](const glm::vec3& v1, float value) { return v1 * value; },
            [](float value, const glm::vec3& v1) { return v1 * value; }
        );

        auto vec3_divide_overloads = sol::overload(
            [](const glm::vec3& v1, const glm::vec3& v2) { return v1 / v2; },
            [](const glm::vec3& v1, float value) { return v1 / value; },
            [](float value, const glm::vec3& v1) { return v1 / value; }
        );

        auto vec3_addition_overloads = sol::overload(
            [](const glm::vec3& v1, const glm::vec3& v2) { return v1 + v2; },
            [](const glm::vec3& v1, float value) { return v1 + value; },
            [](float value, const glm::vec3& v1) { return v1 + value; }
        );

        auto vec3_substraction_overloads = sol::overload(
            [](const glm::vec3& v1, const glm::vec3& v2) { return v1 - v2; },
            [](const glm::vec3& v1, float value) { return v1 - value; },
            [](float value, const glm::vec3& v1) { return v1 - value; }
        );

        lua.new_usertype<glm::vec3>("vec3",
            sol::call_constructor,
            sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z,
            sol::meta_function::multiplication, vec3_multiply_overloads,
            sol::meta_function::division, vec3_divide_overloads,
            sol::meta_function::addition, vec3_addition_overloads,
            sol::meta_function::subtraction, vec3_substraction_overloads
        );
    }

    void NScriptingInterface::RegisterVec4Structures(sol::state& lua)
    {
        // Register glm::vec4
        auto vec4_multiply_overloads = sol::overload(
            [](const glm::vec4& v1, const glm::vec4& v2) { return v1 * v2; },
            [](const glm::vec4& v1, float value) { return v1 * value; },
            [](float value, const glm::vec4& v1) { return v1 * value; }
        );

        auto vec4_divide_overloads = sol::overload(
            [](const glm::vec4& v1, const glm::vec4& v2) { return v1 / v2; },
            [](const glm::vec4& v1, float value) { return v1 / value; },
            [](float value, const glm::vec4& v1) { return v1 / value; }
        );

        auto vec4_addition_overloads = sol::overload(
            [](const glm::vec4& v1, const glm::vec4& v2) { return v1 + v2; },
            [](const glm::vec4& v1, float value) { return v1 + value; },
            [](float value, const glm::vec4& v1) { return v1 + value; }
        );

        auto vec4_substraction_overloads = sol::overload(
            [](const glm::vec4& v1, const glm::vec4& v2) { return v1 - v2; },
            [](const glm::vec4& v1, float value) { return v1 - value; },
            [](float value, const glm::vec4& v1) { return v1 - value; }
        );

        lua.new_usertype<glm::vec4>("vec4",
            sol::call_constructor,
            sol::constructors<glm::vec4(float), glm::vec4(float, float, float, float)>(),
            "x", &glm::vec4::x,
            "y", &glm::vec4::y,
            "z", &glm::vec4::z,
            "w", &glm::vec4::w,
            sol::meta_function::multiplication, vec4_multiply_overloads,
            sol::meta_function::division, vec4_divide_overloads,
            sol::meta_function::addition, vec4_addition_overloads,
            sol::meta_function::subtraction, vec4_substraction_overloads
        );
    }

    void NScriptingInterface::MathFreeFunctions(sol::state& lua)
    {
        lua.set_function("distance",
            sol::overload(
                [](glm::vec2& a, glm::vec2& b) { return glm::distance(a, b); },
                [](glm::vec3& a, glm::vec3& b) { return glm::distance(a, b); },
                [](glm::vec4& a, glm::vec4& b) { return glm::distance(a, b); }
        ));

        lua.set_function("lerp",
            sol::overload( 
                [](float a, float b, float t) { return std::lerp(a,b,t); }
        ));

        lua.set_function("clamp",
            sol::overload(
                [](float value, float min, float max) { return  std::clamp(value, min, max); },
                [](double value, double min, double max) { return  std::clamp(value, min, max); },
                [](int value, int min, int max) { return  std::clamp(value, min, max); }

        ));
    }
    
}