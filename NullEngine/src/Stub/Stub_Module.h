#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	stubfile.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core/c.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//



class NLE_API Stub_Module
{
public:
	Stub_Module();
	~Stub_Module();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Shutdown() = 0;

private:

};


