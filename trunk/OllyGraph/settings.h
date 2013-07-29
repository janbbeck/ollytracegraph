/*******************************************************************************
 * OllyGraph - settings.h
 *
 * Copyright (c) 2013, Austyn Krutsinger
 * All rights reserved.
 *
 * OllyGraph is released under the New BSD license (see LICENSE.txt).
 *
 ******************************************************************************/

#ifndef _OLLYGRAPH_SETTINGS_H_
#define _OLLYGRAPH_SETTINGS_H_

/* Global Declarations */
wchar_t global_qwingraph_path[MAX_PATH];

/* Prototypes */
INT_PTR CALLBACK settings_dialog_procedure(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void save_settings(HWND hDlg);
void load_settings(HWND hDlg);

#endif	/*_OLLYGRAPH_SETTINGS_INCLUDED_ */