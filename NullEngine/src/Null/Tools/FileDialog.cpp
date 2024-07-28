
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
#include "tinyfiledialogs.h"



//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	std::string FileDialog::OpenFile(const std::string& title, const char* filterPatterns[], const std::string& description)
	{
        //const char* filterPatterns[] = { "*.png", "*.jpg", "*.bmp", "*.tga" };
        const char* selectedFile = tinyfd_openFileDialog(
            title.c_str(),   // Dialog title
            "",                  // Default path
            4,                   // Number of filter patterns
            filterPatterns,      // Filter patterns
            description.c_str(),                // Single filter description
            0                    // Allow multiple selections (0 or 1)
        );

        if (selectedFile) {
            // Do something with the selected file
            // For example, you can store the path in a variable or load the file
            std::string filePath = selectedFile;
            return filePath;
        }

        return "";
	}
    std::string FileDialog::SaveFile(const std::string& title, const char* filterPatterns[], const std::string& description)
    {
        //const char* filterPatterns[] = { "*.png", "*.jpg", "*.bmp", "*.tga" };

        const char* saveFileName = tinyfd_saveFileDialog(
            title.c_str(),   // Dialog title
            "",                  // Default path
            4,                   // Number of filter patterns
            filterPatterns,      // Filter patterns
            description.c_str()                 // Single filter description
        );

        if (saveFileName) {
            // Do something with the selected file
            std::string filePath = saveFileName;
            return saveFileName;
        }
        return std::string();
    }
}
