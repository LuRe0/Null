#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	FileDialog.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{
	class FileDialog
	{
	public:
	
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);

		//static std::string SaveFile(const std::string& title, const char* filterPatterns[], const std::string& description);
	private:

	};

}