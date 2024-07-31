#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	stdafx.h
// Author(s):	Anthon Reid 
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
// add headers that you want to pre-compile here
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <memory>
#include <bitset>
#include <functional>
#include <sstream>
#include <tuple>
#include <set>
#include <fstream>
#include <string_view>
#include <typeinfo>
#include <typeindex>
#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <utility> // For std::forward
#include <queue>
#include <cstdio>
#include <unordered_map>
#include <glm/glm.hpp>
#include "NIncludes.h"


#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_SIMD_AVX2
#define GLM_FORCE_SSE2
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define _USE_MATH_DEFINES
#include <math.h>


#ifdef NLE_PLATFORM_WINDOWS
#include <Windows.h>
#include <windows.h>
#endif // NLE_PLATFORM_WINDOWS

using JSON = nlohmann::json;


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//

//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//





//******************************************************************************//
// Private Functions													        //
//******************************************************************************//



