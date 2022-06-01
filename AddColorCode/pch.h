#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <tchar.h>
#include <crtdbg.h>
#include <strsafe.h>
#include <locale.h>

#include <algorithm>
#include <vector>
#include <memory>

#include "AviUtl/aviutl_plugin_sdk/filter.h"
#include "AviUtl/aviutl_exedit_sdk/exedit.hpp"
#include "Common/Tracer.h"
#include "Common/Hook.h"
#include "Common/AviUtlInternal.h"
#include "Detours.4.0.1/detours.h"
#pragma comment(lib, "Detours.4.0.1/detours.lib")
