#pragma once


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Core.h
// Author(s):	Anthon Reid 
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//

//******************************************************************************//
// Definitions  														        //
//******************************************************************************//

#ifdef NLE_PLATFORM_WINDOWS
	#ifdef NLE_BUILD_DLL
		#define NLE_API __declspec(dllexport)
	#else
		#define NLE_API __declspec(dllimport)
	#endif
#else
	#error NullEngine only supports Windows.

#endif // NE_PLATFORM_WINDOWN


//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //

