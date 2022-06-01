#pragma once

//---------------------------------------------------------------------

DECLARE_HOOK_PROC(INT_PTR, CALLBACK, ColorDialogProc, (HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam));
DECLARE_HOOK_PROC(INT_PTR, CDECL, ShowColorDialog, (HINSTANCE instance, LPCSTR templateName, HWND parent, DLGPROC dialogProc));
DECLARE_HOOK_PROC(void, CDECL, ColorDialog_SetColor, (int r, int g, int b));
DECLARE_HOOK_PROC(void, CDECL, ColorDialog_UpdateColorCircle, (HWND hdlg));
DECLARE_HOOK_PROC(void, CDECL, ColorDialog_UpdateControls, (HWND hdlg, int r, int g, int b));

//---------------------------------------------------------------------

void initHook();
void termHook();

//---------------------------------------------------------------------

extern AviUtlInternal g_auin;
extern HINSTANCE g_instance;

//---------------------------------------------------------------------
