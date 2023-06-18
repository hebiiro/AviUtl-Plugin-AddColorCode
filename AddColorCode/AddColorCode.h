#pragma once

//--------------------------------------------------------------------

DECLARE_HOOK_PROC(int, WINAPI, SetDIBitsToDevice, (
	_In_ HDC hdc, _In_ int xDest, _In_ int yDest, _In_ DWORD w, _In_ DWORD h, _In_ int xSrc, _In_ int ySrc,
	_In_ UINT StartScan, _In_ UINT cLines, _In_ CONST VOID * lpvBits, _In_ CONST BITMAPINFO * lpbmi, _In_ UINT ColorUse));
DECLARE_HOOK_PROC(INT_PTR, CALLBACK, ColorDialogProc, (HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam));
DECLARE_HOOK_PROC(INT_PTR, CDECL, ShowColorDialog, (HINSTANCE instance, LPCSTR templateName, HWND parent, DLGPROC dialogProc));
DECLARE_HOOK_PROC(void, CDECL, ColorDialog_SetColor, (int r, int g, int b));
DECLARE_HOOK_PROC(void, CDECL, ColorDialog_UpdateColorCircle, (HWND hdlg));
DECLARE_HOOK_PROC(void, CDECL, ColorDialog_UpdateControls, (HWND hdlg, int r, int g, int b));
DECLARE_HOOK_PROC(COLORREF, WINAPI, Dropper_GetPixel, (HDC dc, int x, int y));

//--------------------------------------------------------------------

void loadProfile();
void initHook();
void termHook();

//--------------------------------------------------------------------

extern AviUtlInternal g_auin;
extern HINSTANCE g_instance;

//--------------------------------------------------------------------
