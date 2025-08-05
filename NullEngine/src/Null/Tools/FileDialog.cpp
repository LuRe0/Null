//------------------------------------------------------------------------------
// File Name:	FileDialog.cpp
// Author(s):	Anthon Reid
// Description: Handles native Windows file dialog for Open and Save functionality.
//------------------------------------------------------------------------------

/****************************************************************************************
* Includes
****************************************************************************************/
#include "stdafx.h"
#include "FileDialog.h"
#include <commdlg.h>                //!< Windows file dialog API
#include <GLFW/glfw3.h>             //!< GLFW core
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>       //!< GLFW native access for Win32 window
//#include "Null/Engine/Submodules/Window/NWindow.h" //!< For window handle access
#include "NIncludes.h"              //!< Common includes for NullEngine


namespace NULLENGINE
{
    /**
     * @brief Opens a file dialog using native Windows API.
     *
     * This method returns only the file name without the path or extension.
     *
     * @param filter File type filters (e.g., "All Files\0*.*\0Text Files\0*.txt\0")
     * @return std::string The name of the selected file, excluding path and extension.
     */
    std::string FileDialog::OpenFile(const char* filter)
    {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };

        ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
        ofn.lStructSize = sizeof(OPENFILENAMEA);
        ofn.hwndOwner = glfwGetWin32Window(NWindow::Instance()->GetWinddow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            std::string path = ofn.lpstrFile;

            auto pos = path.find_last_of('\\');
            path = (pos == std::string::npos) ? path : path.substr(pos + 1);

            auto dotPos = path.find_last_of('.');
            path = (dotPos == std::string::npos) ? path : path.substr(0, dotPos);

            return path;
        }

        return {};
    }

    /**
     * @brief Opens a save file dialog using native Windows API.
     *
     * This method returns only the intended file name without path or extension.
     *
     * @param filter File type filters (e.g., "All Files\0*.*\0Text Files\0*.txt\0")
     * @return std::string The name of the file to save, excluding path and extension.
     */
    std::string FileDialog::SaveFile(const char* filter)
    {
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };

        ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
        ofn.lStructSize = sizeof(OPENFILENAMEA);
        ofn.hwndOwner = glfwGetWin32Window(NWindow::Instance()->GetWinddow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            std::string path = ofn.lpstrFile;

            auto pos = path.find_last_of('\\');
            path = (pos == std::string::npos) ? path : path.substr(pos + 1);

            auto dotPos = path.find_last_of('.');
            path = (dotPos == std::string::npos) ? path : path.substr(0, dotPos);

            return path;
        }

        return {};
    }
}
