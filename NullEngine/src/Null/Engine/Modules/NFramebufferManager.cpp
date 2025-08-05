
//------------------------------------------------------------------------------
//
// File Name:	NFramebufferManager.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "NFramebufferManager.h"
#include "glad/glad.h"
#include "Null/Tools/GLHelper.h"
#include "NIncludes.h"



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	void NFramebufferManager::Load()
	{

        NWindow* window = NWindow::Instance();


		std::string filePath = std::string("../Assets/Framebuffers");

		for (const auto& entry : std::filesystem::directory_iterator(filePath))
		{
			if (entry.is_regular_file()) // Ensure it's a regular file (not a directory or symlink)
			{
				const std::string& filePath = entry.path().string();

				size_t lastSlash = filePath.find_last_of("\\/");
				size_t start = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;

				size_t lastDot = filePath.find_last_of('.');
				size_t end = (lastDot == std::string::npos) ? filePath.length() : lastDot;

				const std::string& name = filePath.substr(start, end - start);


                std::string path = "../Assets/Framebuffers/" + name + ".fb";


                std::ifstream file(path);
                if (!file.is_open()) {
                    NLE_CORE_WARN("Failed to open framebuffer config file: {}", path);
                    return;
                }




                nlohmann::json jsonData;
                file >> jsonData;

                Framebuffer* framebuffer = Create(name, window->Width(), window->Height());


                std::vector<Framebuffer::Format> formats;

                if (jsonData.contains("attachments")) {
                    for (const auto& attachment : jsonData["attachments"])
                    {
                        std::string internalFormatStr = attachment.value("internalFormat", "GL_RGBA8");
                        std::string formatStr = attachment.value("format", "GL_RGBA");
                        std::string typeStr = attachment.value("type", "GL_UNSIGNED_BYTE");

                        GLenum internalFormat = GLHelper::StringToGLenum(internalFormatStr);
                        GLenum format = GLHelper::StringToGLenum(formatStr);
                        GLenum type = GLHelper::StringToGLenum(typeStr);

                        formats.push_back({ internalFormat, format, type });
                    }
                }
                else
                {
                    NLE_CORE_WARN("No attachments defined in framebuffer config: {}", path);
                }

                framebuffer->Init();
                framebuffer->AddColorAttachment(formats);

 
                //NLE_CORE_INFO("Loaded framebuffer '{}'", name);

			}
		}
	}

    void NFramebufferManager::Init()
    {
        NEventManager* eventManager =   NEventManager::Instance();

        SUBSCRIBE_EVENT(WindowResizeEvent, &NFramebufferManager::OnWindowResize, eventManager, EventPriority::Low);
    }

	void NFramebufferManager::Unload()
	{
		NResourceManager::Unload();
	}
    void NFramebufferManager::Shutdown()
    {
    }

    void NFramebufferManager::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }

    bool NFramebufferManager::OnWindowResize(const WindowResizeEvent& e)
    {

            m_WinWidth = static_cast<float>(e.GetWidth());
            m_WinHeight = static_cast<float>(e.GetHeight());

            for (auto& [name, framebuffer] : m_ResourceList)
            {
                framebuffer->Resize(static_cast<unsigned int>(m_WinWidth), static_cast<unsigned int>(m_WinHeight));
            }

            SetViewport(0, 0, static_cast<uint32_t>(m_WinWidth), static_cast<uint32_t>(m_WinHeight));
        

        return true;
    }
}
