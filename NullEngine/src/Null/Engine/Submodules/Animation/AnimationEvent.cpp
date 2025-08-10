
//------------------------------------------------------------------------------
//
// File Name:	NStub.cpp
// Author(s):	Anthon Reid
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "stdafx.h"
#include "AnimationEvent.h"
#include "NIncludes.h"	




//******************************************************************************//
// Public Variables															    //
//******************************************************************************//

//******************************************************************************//
// Function Declarations												        //
//******************************************************************************//

namespace NULLENGINE
{
	AnimationEvent::AnimationEvent(const std::string& assetName, const std::string& path, EventType eventType, sol::state& lua)
		: assetID(STRID(assetName)), scriptPathID(STRID(path)), type(eventType)
	{
		luaTable = lua.script_file(path);
	}
}
