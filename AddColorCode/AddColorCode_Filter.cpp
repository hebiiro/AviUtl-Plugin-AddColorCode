#include "pch.h"
#include "AddColorCode.h"

//--------------------------------------------------------------------
//		フィルタ構造体のポインタを渡す関数
//--------------------------------------------------------------------
EXTERN_C FILTER_DLL* WINAPI GetFilterTable()
{
	static TCHAR filterName[] = TEXT("カラーコード追加");
	static TCHAR filterInformation[] = TEXT("カラーコード追加 1.2.0 by 蛇色");

	static FILTER_DLL filter =
	{
		FILTER_FLAG_NO_CONFIG |
		FILTER_FLAG_ALWAYS_ACTIVE |
		FILTER_FLAG_DISP_FILTER |
		FILTER_FLAG_EX_INFORMATION,
		0, 0,
		filterName,
		NULL, NULL, NULL,
		NULL, NULL,
		NULL, NULL, NULL,
		NULL,//func_proc,
		func_init,
		func_exit,
		NULL,
		NULL,//func_WndProc,
		NULL, NULL,
		NULL,
		NULL,
		filterInformation,
		NULL, NULL,
		NULL, NULL, NULL, NULL,
		NULL,
	};

	return &filter;
}

//--------------------------------------------------------------------
//		初期化
//--------------------------------------------------------------------

BOOL func_init(FILTER *fp)
{
	MY_TRACE(_T("func_init()\n"));

	loadProfile();
	initHook();

	return TRUE;
}

//--------------------------------------------------------------------
//		終了
//--------------------------------------------------------------------
BOOL func_exit(FILTER *fp)
{
	MY_TRACE(_T("func_exit()\n"));

	termHook();

	return TRUE;
}

//--------------------------------------------------------------------

BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		{
			MY_TRACE(_T("DLL_PROCESS_ATTACH\n"));

			g_instance = instance;

			break;
		}
	case DLL_PROCESS_DETACH:
		{
			MY_TRACE(_T("DLL_PROCESS_DETACH\n"));

			break;
		}
	}

	return TRUE;
}

//--------------------------------------------------------------------
