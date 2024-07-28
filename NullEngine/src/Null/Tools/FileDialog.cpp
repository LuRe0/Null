
//------------------------------------------------------------------------------
//
// File Name:	FileDialog.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "FileDialog.h"
#include <commdlg.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include<GLFW/glfw3native.h>


//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{

    std::string FileDialog::OpenFile(const char* filter)
    {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };

        // Initialize OPENFILENAMEA
        ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
        ofn.lStructSize = sizeof(OPENFILENAMEA);
        ofn.hwndOwner = glfwGetWin32Window(NEngine::Instance().Get<NWindow>()->GetWinddow()); // If you have a window handle, replace nullptr with it
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        // Display the Open dialog box
        if (GetOpenFileNameA(&ofn) == TRUE) 
        {
            std::string path = std::string(ofn.lpstrFile);

            auto pos = path.find_last_of('\\');
            path = (pos == std::string::npos) ? path : path.substr(pos + 1);

            auto dotPos = path.find_last_of('.');
            path = (pos == std::string::npos) ? path : path.substr(0, dotPos);

            return path;
        }

        return std::string();
    }

    std::string FileDialog::SaveFile(const char* filter)
    {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };

        // Initialize OPENFILENAMEA
        ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
        ofn.lStructSize = sizeof(OPENFILENAMEA);
        ofn.hwndOwner = glfwGetWin32Window(NEngine::Instance().Get<NWindow>()->GetWinddow()); // If you have a window handle, replace nullptr with it
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        // Display the Open dialog box
        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            std::string path = std::string(ofn.lpstrFile);

            auto pos = path.find_last_of('\\');
            path = (pos == std::string::npos) ? path : path.substr(pos + 1);

            auto dotPos = path.find_last_of('.');
            path = (pos == std::string::npos) ? path : path.substr(0, dotPos);

            return path;
        }

        return std::string();
    }


}
