#include "pch.h"
#include "AddColorCode.h"
#include "resource.h"

//---------------------------------------------------------------------

// デバッグ用コールバック関数。デバッグメッセージを出力する
void ___outputLog(LPCTSTR text, LPCTSTR output)
{
	::OutputDebugString(output);
}

//---------------------------------------------------------------------

AviUtlInternal g_auin;
HINSTANCE g_instance = 0;
HWND g_hdlg = 0;

BOOL* ColorDialog_CanHandleCommand = 0;

//--------------------------------------------------------------------

// フックをセットする。
void initHook()
{
	MY_TRACE(_T("initHook()\n"));

	g_auin.initExEditAddress();

	DWORD exedit = g_auin.GetExedit();
	if (!exedit) return;

	castAddress(ColorDialog_CanHandleCommand, exedit + 0x134E64);

	castAddress(true_ColorDialog_SetColor, exedit + 0x22420);
	castAddress(true_ColorDialog_UpdateColorCircle, exedit + 0x21300);
	castAddress(true_ColorDialog_UpdateControls, exedit + 0x216A0);

	true_ShowColorDialog = hookCall(exedit + 0x4D386, hook_ShowColorDialog);

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
				TCHAR text[MAX_PATH] = {};
				::GetDlgItemText(hdlg, IDC_COLOR_CODE, text, MAX_PATH);
				MY_TRACE_TSTR(text);
				DWORD color = _tcstoul(text, 0, 16);
				MY_TRACE_HEX(color);
				int r = (color >> 16) & 0xff;
				int g = (color >>  8) & 0xff;
				int b = (color >>  0) & 0xff;

				true_ColorDialog_SetColor(r, g, b);
				true_ColorDialog_UpdateColorCircle(hdlg);
				true_ColorDialog_UpdateControls(hdlg, r, g, b);
			}

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

//--------------------------------------------------------------------
