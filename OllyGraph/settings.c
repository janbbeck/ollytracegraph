// OllyTraceGraph - OllyTraceGraph.c

// OllyTraceGraph is a modification made by Jan Beck of OllyGraph by Austyn Krutsinger

// Thanks for providing the source, Austyn!
// https://github.com/akrutsinger/OllyGraph

// I don't claim any additional copyright for the modifications.

// From the original OllyGraph:

/*******************************************************************************
 * OllyGraph - settings.c 
 *
 * Copyright (c) 2013, Austyn Krutsinger
 * All rights reserved.
 *
 * OllyGraph is released under the New BSD license (see LICENSE.txt).
 *
 ******************************************************************************/

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <ShellAPI.h>

#include "plugin.h"
#include "OllyTraceGraph.h"
#include "settings.h"
#include "resource.h"

/* Globals Definitions - Program specific */
extern wchar_t	global_qwingraph_path[MAX_PATH];

static int		qwingraph_path_changed		= FALSE;	/* Module specific indicator if qwingraph path changed */

INT_PTR CALLBACK settings_dialog_procedure(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int ret;
	switch (uMsg) { 
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case IDC_OK:
				/* Save the settings */
				save_settings(hDlg);
				EndDialog(hDlg, 1);
				return TRUE;
			case IDC_CANCEL:
				/* End dialog without saving anything */
				EndDialog(hDlg, 0);
				return TRUE;
			case IDC_BROWSE:
				ret = Browsefilename(L"OllyGraph - Find Graph Handler", global_qwingraph_path, L"*.txt", (wchar_t*)plugindir, L"txt", hwollymain, BRO_FILE);
				if (ret != 0) {
					SetDlgItemText(hDlg, IDC_WINGRAPH32_PATH, (LPCWSTR)global_qwingraph_path);
					qwingraph_path_changed = TRUE;
				}
				return TRUE;
			case IDC_WINGRAPH32_PATH:
				if (HIWORD(wParam) == EN_CHANGE)
					qwingraph_path_changed = TRUE;
				return TRUE;
		}
		return TRUE;
	case WM_SYSCHAR:
		MessageBox(NULL, L"keyup", L"keyup", MB_OK);
		if (GetFocus() == GetDlgItem(hDlg, IDC_WINGRAPH32_PATH))
			qwingraph_path_changed = TRUE;
		return TRUE;
	case WM_DROPFILES:
	{
		HDROP hdrop = (HDROP)wParam;
		DragQueryFile(hdrop, 0, global_qwingraph_path, sizeof(global_qwingraph_path));
		DragFinish(hdrop);
		SetDlgItemText(hDlg, IDC_WINGRAPH32_PATH, (LPCWSTR)global_qwingraph_path);
		qwingraph_path_changed = TRUE;
		SetFocus(GetDlgItem(hDlg, IDC_WINGRAPH32_PATH));
		return TRUE;
	}
	case WM_CLOSE:
		DragAcceptFiles(hDlg, FALSE);
		EndDialog(hDlg, 0);
		return TRUE;
	case WM_INITDIALOG:
		/* Load settings from ollydbg.ini. If there is no
		 * setting already in the ollydbg.ini, set the default
		 * values so we can save them if we want
		 */
		DragAcceptFiles(hDlg, TRUE);
		load_settings(hDlg);
		SetFocus(GetDlgItem(hDlg, IDC_CANCEL));
		return TRUE;
	}
	return FALSE; 
}

void save_settings(HWND hDlg)
{
	/* qwingraph Path */
	if (qwingraph_path_changed == TRUE) {
		GetDlgItemText(hDlg, IDC_WINGRAPH32_PATH, global_qwingraph_path, MAXPATH);
		Writetoini(NULL, PLUGIN_NAME, L"qwingraph path", global_qwingraph_path);
	}
}

void load_settings(HWND hDlg)
{
	/* Local variables */
	int ret;
	int n;			/* Used for string concatination */

	/* Database Path */
	ret = Stringfromini(PLUGIN_NAME, L"qwingraph path", global_qwingraph_path, MAXPATH);
	if (ret == 0) {
		/* Set default database name to userdb.txt in plugin directory */
		n = StrcopyW(global_qwingraph_path, MAXPATH, plugindir);
		n += StrcopyW(global_qwingraph_path + n, MAXPATH - n, L"\\qwingraph.exe");
	}
	SetDlgItemText(hDlg, IDC_WINGRAPH32_PATH, (LPCWSTR)global_qwingraph_path);
}