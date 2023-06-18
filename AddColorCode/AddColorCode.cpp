#include "pch.h"
#include "AddColorCode.h"
#include "resource.h"

//--------------------------------------------------------------------

// デバッグ用コールバック関数。デバッグメッセージを出力する
void ___outputLog(LPCTSTR text, LPCTSTR output)
{
	::OutputDebugString(output);
}

//--------------------------------------------------------------------

AviUtlInternal g_auin;
HINSTANCE g_instance = 0;
HWND g_hdlg = 0;
POINT g_offset = { 0, 0 };

BOOL* ColorDialog_CanHandleCommand = 0;
int* ColorDialog_R = 0;
int* ColorDialog_G = 0;
int* ColorDialog_B = 0;

//--------------------------------------------------------------------

// ini ファイルから設定を読み込む。
void loadProfile()
{
	MY_TRACE(_T("loadProfile()\n"));

	TCHAR path[MAX_PATH];
	::GetModuleFileName(g_instance, path, MAX_PATH);
	::PathRenameExtension(path, _T(".ini"));
	MY_TRACE_TSTR(path);

	g_offset.x = ::GetPrivateProfileInt(_T("Settings"), _T("offset.x"), g_offset.x, path);
	g_offset.y = ::GetPrivateProfileInt(_T("Settings"), _T("offset.y"), g_offset.y, path);

	MY_TRACE_INT(g_offset.x);
	MY_TRACE_INT(g_offset.y);
}

// フックをセットする。
void initHook()
{
	MY_TRACE(_T("initHook()\n"));

	g_auin.initExEditAddress();

	DWORD exedit = g_auin.GetExEdit();
	if (!exedit) return;

	true_SetDIBitsToDevice = hookImportFunc(
		(HMODULE)exedit, "SetDIBitsToDevice", hook_SetDIBitsToDevice);
	MY_TRACE_HEX(true_SetDIBitsToDevice);

	castAddress(ColorDialog_CanHandleCommand, exedit + 0x134E64);
	castAddress(ColorDialog_R, exedit + 0x11F2D0);
	castAddress(ColorDialog_G, exedit + 0x11F0AC);
	castAddress(ColorDialog_B, exedit + 0x11F064);

	castAddress(true_ColorDialog_SetColor, exedit + 0x22420);
	castAddress(true_ColorDialog_UpdateColorCircle, exedit + 0x21300);
	castAddress(true_ColorDialog_UpdateControls, exedit + 0x216A0);

	true_ShowColorDialog = hookCall(exedit + 0x4D386, hook_ShowColorDialog);
	MY_TRACE_HEX(true_ShowColorDialog);

	// スポイト処理の ::GetPixel() をフックする。
	true_Dropper_GetPixel = hookAbsoluteCall(exedit + 0x22128, hook_Dropper_GetPixel);
	MY_TRACE_HEX(true_Dropper_GetPixel);

	DetourTransactionBegin();
	DetourUpdateThread(::GetCurrentThread());

	ATTACH_HOOK_PROC(ColorDialog_SetColor);
	ATTACH_HOOK_PROC(ColorDialog_UpdateColorCircle);
	ATTACH_HOOK_PROC(ColorDialog_UpdateControls);

	if (DetourTransactionCommit() == NO_ERROR)
	{
		MY_TRACE(_T("拡張編集のフックに成功しました\n"));
	}
	else
	{
		MY_TRACE(_T("拡張編集のフックに失敗しました\n"));
	}
}

// フックを解除する。
void termHook()
{
	MY_TRACE(_T("termHook()\n"));
}

//--------------------------------------------------------------------

void updateColorCode(int r, int g, int b)
{
	// フラグをセットしてからコントロールの値を更新する。

	*ColorDialog_CanHandleCommand = FALSE;

	TCHAR text[MAX_PATH] = {};
	::StringCbPrintf(text, sizeof(text), _T("%02x%02x%02x"), r, g, b);
	::SetDlgItemText(g_hdlg, 174, text);

	*ColorDialog_CanHandleCommand = TRUE;
}

//--------------------------------------------------------------------

IMPLEMENT_HOOK_PROC_NULL(int, WINAPI, SetDIBitsToDevice, (
	_In_ HDC hdc, _In_ int xDest, _In_ int yDest, _In_ DWORD w, _In_ DWORD h, _In_ int xSrc, _In_ int ySrc,
	_In_ UINT StartScan, _In_ UINT cLines, _In_ CONST VOID * lpvBits, _In_ CONST BITMAPINFO * lpbmi, _In_ UINT ColorUse))
{
	MY_TRACE(_T("SetDIBitsToDevice(%d, %d) 変更前\n"), xDest, yDest);

	// 描画位置をオフセットの分だけずらす。

	xDest += g_offset.x;
	yDest += g_offset.y;

	MY_TRACE(_T("SetDIBitsToDevice(%d, %d) 変更後\n"), xDest, yDest);

	return true_SetDIBitsToDevice(hdc, xDest, yDest, w, h, xSrc, ySrc, StartScan, cLines, lpvBits, lpbmi, ColorUse);
}

IMPLEMENT_HOOK_PROC_NULL(INT_PTR, CALLBACK, ColorDialogProc, (HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam))
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			MY_TRACE(_T("WM_INITDIALOG\n"));

			g_hdlg = hdlg;
			MY_TRACE_HEX(g_hdlg);

			break;
		}
	case WM_COMMAND:
		{
			UINT code = HIWORD(wParam);
			UINT id = LOWORD(wParam);
			HWND sender = (HWND)lParam;

//			MY_TRACE(_T("WM_COMMAND, 0x%04X, 0x%04X, 0x%08X)\n"), code, id, sender);

			if (code == EN_UPDATE && id == IDC_COLOR_CODE && *ColorDialog_CanHandleCommand)
			{
				// エディットボックスのテキストを取得する。
				TCHAR text[MAX_PATH] = {};
				::GetDlgItemText(hdlg, IDC_COLOR_CODE, text, MAX_PATH);
				MY_TRACE_TSTR(text);

				// テキスト内の数値開始位置へのオフセットを取得する。
				int offset = 0;
				if (text[0] == _T('#')) offset = 1;

				// テキストを数値に変換する。
				DWORD color = _tcstoul(text + offset, 0, 16);
				MY_TRACE_HEX(color);

				int r, g, b;

				if (::lstrlen(text + offset) > 3)
				{
					// rrggbb の形式の RGB を取得する。
					r = (color >> 16) & 0xff;
					g = (color >>  8) & 0xff;
					b = (color >>  0) & 0xff;
				}
				else
				{
					// rgb の形式の RGB を取得する。
					r = (color >> 8) & 0x0f;
					g = (color >> 4) & 0x0f;
					b = (color >> 0) & 0x0f;

					r |= r << 4;
					g |= g << 4;
					b |= b << 4;
				}
				MY_TRACE(_T("%d, %d, %d\n"), r, g, b);

				*ColorDialog_R = r;
				*ColorDialog_G = g;
				*ColorDialog_B = b;
				true_ColorDialog_SetColor(r, g, b);
				true_ColorDialog_UpdateColorCircle(hdlg);
				true_ColorDialog_UpdateControls(hdlg, r, g, b);
			}

			break;
		}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		{
			// マウス位置をオフセットの分だけずらす。

			POINT point = LP2PT(lParam);
			point.x -= g_offset.x;
			point.y -= g_offset.y;
			lParam = PT2LP(point);

			break;
		}
	}

	return true_ColorDialogProc(hdlg, message, wParam, lParam);
}

IMPLEMENT_HOOK_PROC_NULL(INT_PTR, CDECL, ShowColorDialog, (HINSTANCE instance, LPCSTR templateName, HWND parent, DLGPROC dialogProc))
{
	MY_TRACE(_T("ShowColorDialog(0x%08X, %hs, 0x%08X, 0x%08X)\n"), instance, templateName, parent, dialogProc);

	if (::lstrcmpiA(templateName, "GET_COLOR") == 0)
	{
		MY_TRACE(_T("「色の選択」ダイアログをフックします\n"));

		true_ColorDialogProc = dialogProc;
		return true_ShowColorDialog(g_instance, templateName, parent, hook_ColorDialogProc);
	}

	return true_ShowColorDialog(instance, templateName, parent, dialogProc);
}

IMPLEMENT_HOOK_PROC_NULL(void, CDECL, ColorDialog_SetColor, (int r, int g, int b))
{
	MY_TRACE(_T("ColorDialog_SetColor(%d, %d, %d)\n"), r, g, b);

	true_ColorDialog_SetColor(r, g, b);

	updateColorCode(r, g, b);
}

IMPLEMENT_HOOK_PROC_NULL(void, CDECL, ColorDialog_UpdateColorCircle, (HWND hdlg))
{
	MY_TRACE(_T("ColorDialog_UpdateColorCircle(0x%08X)\n"), hdlg);

	true_ColorDialog_UpdateColorCircle(hdlg);
}

IMPLEMENT_HOOK_PROC_NULL(void, CDECL, ColorDialog_UpdateControls, (HWND hdlg, int r, int g, int b))
{
	MY_TRACE(_T("ColorDialog_UpdateControls(0x%08X, %d, %d, %d)\n"), hdlg, r, g, b);

	true_ColorDialog_UpdateControls(hdlg, r, g, b);

	updateColorCode(r, g, b);
}

IMPLEMENT_HOOK_PROC_NULL(COLORREF, WINAPI, Dropper_GetPixel, (HDC _dc, int x, int y))
{
	MY_TRACE(_T("Dropper_GetPixel(0x%08X, %d, %d)\n"), _dc, x, y);

	// すべてのモニタのすべての場所から色を抽出できるようにする。

	HWND hwnd = 0;
	POINT point; ::GetCursorPos(&point);
	::LogicalToPhysicalPointForPerMonitorDPI(hwnd, &point);
	HDC dc = ::GetDC(hwnd);
	COLORREF color = ::GetPixel(dc, point.x, point.y);
	::ReleaseDC(hwnd, dc);
	return color;
}

//--------------------------------------------------------------------
