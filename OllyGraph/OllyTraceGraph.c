// OllyTraceGraph - OllyTraceGraph.c

// OllyTraceGraph is a modification made by Jan Beck of OllyGraph by Austyn Krutsinger

// Thanks for providing the source, Austyn!
// https://github.com/akrutsinger/OllyGraph

// I don't claim any additional copyright for the modifications.

// From the original OllyGraph:

/*******************************************************************************
 * OllyGraph - OllyGraph.c
 *
 * Copyright (c) 2013, Austyn Krutsinger
 * All rights reserved.
 *
 * OllyGraph is released under the New BSD license (see LICENSE.txt).
 *
 ******************************************************************************/

/*
 * VERY IMPORTANT NOTICE: PLUGINS ARE UNICODE LIBRARIES! COMPILE THEM WITH BYTE
 * ALIGNMENT OF STRUCTURES AND DEFAULT UNSIGNED CHAR!
 */

/*******************************************************************************
 * Things to change as I think of them...
 * [ ] = To do
 * [?] = Might be a good idea?
 * [!] = Implemented
 * [+] = Added
 * [-] = Removed
 * [*] = Changed
 * [~] = Almost there...
 *
 * Version 0.1.0 (17MAR2013)
 * [+] Initial release
 *
 *
 * -----------------------------------------------------------------------------
 * TODO
 * -----------------------------------------------------------------------------
 *
 * [ ] Finish
 *
 ******************************************************************************/

/* Defines */
/*
 * Microsoft compilers hate (and maybe justifiably) old-school functions like
 * wcscpy() that may cause buffer overflow and all related dangers. Still, I
 * don't want to see all these bloody warnings.
 */
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN		/* Remove extra windows.h information */

/* Includes */
#include <Windows.h>
#include <ShellAPI.h>
#include <time.h>
#include <stdio.h>

#include "plugin.h"
#include "OllyTraceGraph.h"
#include "settings.h"
#include "resource.h"

/* Globals Definitions - Module specific */
static HINSTANCE	main_plugin_instance = NULL;	/* Instance of plugin DLL */
static ulong		nodelist[MAX_NODES];
static int			nodes;
static char vcg_params[] =	"manhattan_edges: yes\n"
							"layoutalgorithm: mindepth\n"
							"finetuning: no\n"
							"layout_downfactor: 100\n"
							"layout_upfactor: 0\n"
							"layout_nearfactor: 0\n"
							"xlspace: 12\n"
							"yspace: 30\n"
							"colorentry 32: 0 0 0\n"
							"colorentry 33: 0 0 255\n"
							"colorentry 34: 0 0 255\n"
							"colorentry 35: 128 128 128\n"
							"colorentry 36: 128 128 128\n"
							"colorentry 37: 0 0 128\n"
							"colorentry 38: 0 0 128\n"
							"colorentry 39: 0 0 255\n"
							"colorentry 40: 0 0 255\n"
							"colorentry 41: 0 0 128\n"
							"colorentry 42: 0 128 0\n"
							"colorentry 43: 0 255 0\n"
							"colorentry 44: 0 128 0\n"
							"colorentry 45: 255 128 0\n"
							"colorentry 46: 0 128 0\n"
							"colorentry 47: 128 128 255\n"
							"colorentry 48: 255 0 0\n"
							"colorentry 49: 128 128 0\n"
							"colorentry 50: 1 1 1\n"
							"colorentry 51: 192 192 192\n"
							"colorentry 52: 0 0 255\n"
							"colorentry 53: 0 0 255\n"
							"colorentry 54: 0 0 255\n"
							"colorentry 55: 128 128 128\n"
							"colorentry 56: 128 128 255\n"
							"colorentry 57: 0 128 0\n"
							"colorentry 58: 0 0 128\n"
							"colorentry 59: 0 0 255\n"
							"colorentry 60: 128 0 128\n"
							"colorentry 61: 0 128 0\n"
							"colorentry 62: 0 128 0\n"
							"colorentry 63: 0 128 64\n"
							"colorentry 64: 0 0 128\n"
							"colorentry 65: 0 0 128\n"
							"colorentry 66: 255 0 255\n"
							"colorentry 67: 128 128 0\n"
							"colorentry 68: 0 0 128\n"
							"colorentry 69: 0 0 255\n"
							"colorentry 70: 0 0 128\n"
							"colorentry 71: 0 0 255\n"
							"colorentry 72: 0 0 0\n"
							"colorentry 73: 255 255 255\n"
							"colorentry 74: 192 187 175\n"
							"colorentry 75: 0 255 255\n"
							"colorentry 76: 0 0 0\n"
							"colorentry 77: 128 0 0\n"
							"colorentry 78: 128 128 128\n"
							"colorentry 79: 128 128 0\n"
							"colorentry 80: 255 0 255\n"
							"colorentry 81: 0 0 0\n"
							"colorentry 82: 0 0 255\n"
							"colorentry 83: 100 255 255\n";
/**
 * @display_about_message
 *
 *		Displays "About" message box
 */
void display_about_message(void)
{
	wchar_t about_message[TEXTLEN] = { 0 };
	wchar_t buf[SHORTNAME];
	int n;

	/* Debuggee should continue execution while message box is displayed. */
	Resumeallthreads();
	/* In this case, swprintf() would be as good as a sequence of StrcopyW(), */
	/* but secure copy makes buffer overflow impossible. */
	n = StrcopyW(about_message, TEXTLEN, L"OllyTraceGraph v");
	n += StrcopyW(about_message + n, TEXTLEN - n, PLUGIN_VERS);
	n += StrcopyW(about_message + n, TEXTLEN - n, L"\nOllyGraph Originally coded by OllyGraph by Austyn Krutsinger <akrutsinger@gmail.com>");
	n += StrcopyW(about_message + n, TEXTLEN - n, L"\nAdditions for OllyTraceGraph by Jan Beck <janbeck@gmail.com>");
	/* The conditionals below are here to verify that this plugin can be */
	/* compiled with all supported compilers. They are not necessary in the */
	/* final code. */
		n += StrcopyW(about_message + n, TEXTLEN - n, L"\n\nCompiled on ");
		Asciitounicode(__DATE__, SHORTNAME, buf, SHORTNAME);
		n += StrcopyW(about_message + n, TEXTLEN - n, buf);
		n += StrcopyW(about_message + n, TEXTLEN - n, L" ");
		Asciitounicode(__TIME__, SHORTNAME, buf, SHORTNAME);
		n += StrcopyW(about_message + n, TEXTLEN - n, buf);
		n += StrcopyW(about_message + n, TEXTLEN - n, L" with ");
	#if defined(__BORLANDC__)
		n += StrcopyW(about_message + n, TEXTLEN - n, L"Borland (R) ");
	#elif defined(_MSC_VER)
		n += StrcopyW(about_message + n, TEXTLEN - n, L"Microsoft (R) ");
	#elif defined(__MINGW32__)
		n += StrcopyW(about_message + n, TEXTLEN - n, L"MinGW32 ");
	#else
		n += StrcopyW(about_message + n, TEXTLEN - n, L"\n\nCompiled with ");
	#endif
	#ifdef __cplusplus
		StrcopyW(about_message + n, TEXTLEN - n, L"C++ compiler");
	#else
		StrcopyW(about_message + n, TEXTLEN - n, L"C compiler");
	#endif
	MessageBox(hwollymain, about_message, L"About OllyGraph", MB_OK|MB_ICONINFORMATION);
	/* Suspendallthreads() and Resumeallthreads() must be paired, even if they */
	/* are called in inverse order! */
	Suspendallthreads();
}

/**
 * @menu_handler
 * 
 *      Menu callback for our plugin to process our menu commands.
 */
int menu_handler(t_table* pTable, wchar_t* pName, ulong index, int nMode)
{
	UNREFERENCED_PARAMETER(pTable);
	UNREFERENCED_PARAMETER(pName);

	switch (nMode) {
	case MENU_VERIFY:
		return MENU_NORMAL;

	case MENU_EXECUTE:
		switch (index) {
		case MENU_SETTINGS: /* Menu -> Settings */
			DialogBox(main_plugin_instance,
						MAKEINTRESOURCE(IDD_SETTINGS),
						hwollymain,
						(DLGPROC)settings_dialog_procedure);
			break;
		case MENU_FUNCTION_FLOWCHART: /* Menu | Disasm Menu -> Generate Function Flowchart */
			generate_function_flowchart();
			break;
		case MENU_FUNCTION_CALL_GRAPH: /* Menu | Disasm Menu -> Generate Function Call Graph */
			break;
		case MENU_XREFS_TO_ADDRESS_GRAPH:	/* Menu | Disasm Menu -> Generate XRefs To Address Graph */
			break;
		case MENU_XREFS_FROM_ADDRESS_GRAPH:	/* Menu | Disasm Menu -> Generate XRefs From Address Graph */
			break;
		case MENU_ABOUT: /* Menu -> About */
			display_about_message();
			break;
		}
		return MENU_NOREDRAW;
	}

	return MENU_ABSENT;
}

/**
 * @ODBG2_Pluginmenu
 *
 *      Adds items to OllyDbgs menu system.
 */
extc t_menu * __cdecl ODBG2_Pluginmenu(wchar_t *type)
{
	if (wcscmp(type, PWM_MAIN) == 0)
		/* Main menu. */
		return ollygraph_menu;
	else if (wcscmp(type, PWM_DISASM) == 0)
		/* Disassembler pane of CPU window. */
		return ollygraph_popup_menu;
	return NULL;		/* No menu */
};

void generate_function_flowchart(void)
{
	HANDLE temp_file; 
	DWORD buf_size = DATALEN;
	wchar_t temp_name[MAX_PATH];
	wchar_t path_buffer[DATALEN];
	ulong start, end, psize, next_addr, current_addr, blocksize, currentnode, dsize;
	int writelen, orphannode;
	DWORD len_written;
	t_dump *cpuasm;
	uchar *decode;
	char *edgelist;
	uchar cmdbuf[MAXCMDSIZE];
	char buffer[TEXTLEN*2+4];
	int extra;
	int returncode;
	wchar_t buffer2[TEXTLEN];
	t_table * cpuwindowtable;
	t_dump * cpuwindowdump;
	uchar theMask[TEXTLEN];
	int theInt[TEXTLEN];
	ulong oldAddress;
	char clean_comment[TEXTLEN*2];
	struct disasm_list_element_s *disasm_list_element;
	struct disasm_list_element_s *last_list_element = NULL; 
	void *disasm_list = NULL;
	void *next_disasm;
	t_disasm disasm_result;
	t_reg *reg;
	struct previous_edge_struct_s previous_edge;
	char comment_ascii[TEXTLEN];
	char result_ascii[TEXTLEN];

	cpuasm = Getcpudisasmdump();
	start = 0; end = 0; nodes = 0;
	previous_edge.source = 0;
	previous_edge.targettrue = 0;
	previous_edge.targetfalse = 0;

	if (Getproclimits(cpuasm->sel0, &start, &end) == -1) {
		MessageBox(hwollymain,
			L"Address is not within known function boundaries\n"
			L"(Did you run Analyze Code?)",
			L"Function boundaries not found", MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	edgelist = (char *)Memalloc(MAX_EDGELIST_SIZE, SILENT|ZEROINIT);
	
	if (edgelist == NULL) {
		MessageBox(hwollymain,
			L"Could not allocate memory\n",
			L"Out of Memory", MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	GetTempPath(buf_size, path_buffer); 
	cleanup_tempfiles(path_buffer);
	GetTempFileName(path_buffer, L"ogh", 0, temp_name);
	temp_file = CreateFile((LPTSTR) temp_name, GENERIC_READ | GENERIC_WRITE, 
					 FILE_SHARE_READ,NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 
	if (temp_file == INVALID_HANDLE_VALUE) { 
		MessageBox(hwollymain, L"Failed to create temporary file!", 
			L"Create Temp File Failed",MB_OK|MB_ICONINFORMATION);
		return;					         
	}

	currentnode = start;
	psize = 0;
	current_addr = start; 
	blocksize = end - start;
	orphannode = 1;

	writelen = sprintf(buffer, "graph: {\ntitle: \"Graph of %x\"\n", start); 
	WriteFile(temp_file, buffer, (DWORD) writelen, &len_written, NULL);
	WriteFile(temp_file, vcg_params, (DWORD) strlen(vcg_params), &len_written, NULL);
	writelen = sprintf(buffer, "node: { title: \"%x\" vertical_order: 0 color: 83 fontname: \"courR12\" label: \"%x:", currentnode, currentnode); 
	WriteFile(temp_file, buffer, (DWORD) writelen, &len_written, NULL);
  
  // first pass - disassemble instructions and enumerate nodes
	do {
		current_addr += psize;
		decode = Finddecode(current_addr, &dsize);

		Readmemory(cmdbuf, current_addr, MAXCMDSIZE, MM_SILENT|MM_PARTIAL);
		next_addr = Disassembleforward(NULL, start, blocksize, current_addr, 1, USEDECODE);
		psize = next_addr - current_addr;
		if (psize <= 0) {
			psize = 1;
		} 

		disasm_list_element = (struct disasm_list_element_s *)Memalloc(sizeof(struct disasm_list_element_s), SILENT|ZEROINIT);
	 
		if (disasm_list_element == NULL) {
			MessageBox(hwollymain,
				L"Could not allocate memory\n",
				L"Out of Memory", MB_OK|MB_ICONEXCLAMATION);
			free_list(disasm_list);
			return;
		}

		if (last_list_element != NULL) {
			last_list_element->next_element = disasm_list_element; 
		}
		last_list_element = disasm_list_element;

		// initialize pointer to first element
		if (disasm_list == NULL) { 
			disasm_list = disasm_list_element;
		}

		reg = Threadregisters(cpuasm->threadid);
		Disasm(cmdbuf, psize, current_addr, decode, &(disasm_list_element->disasm), DA_TEXT|DA_OPCOMM|DA_MEMORY, reg, NULL);
		disasm_result = disasm_list_element->disasm;
		// enumerate nodes
		if (currentnode == 0) {
			currentnode = current_addr;
			append_nodelist(currentnode);
		}
		if ((disasm_result.jmpaddr >= start) && (disasm_result.jmpaddr < end)) {
			// this is a jump - start of an edge and pointer to a node
			append_nodelist(disasm_result.jmpaddr);
			currentnode = 0; // update current_addrnode on next pass
		}
	} while (current_addr + psize < end);

	// walk through saved disasm list and split into nodes
	next_disasm = disasm_list;
	currentnode = start;
  
	while (next_disasm != NULL) {
		disasm_list_element = (struct disasm_list_element_s *)next_disasm;
		next_disasm = disasm_list_element->next_element;
		disasm_result = disasm_list_element->disasm;
		current_addr = disasm_result.ip;
		if (nodestart(current_addr)) {
			if (orphannode == 1) {
				writelen = sprintf(buffer, "edge: { sourcename: \"%x\" targetname: \"%x\" }\n", currentnode, current_addr);

				if (StrlenA(edgelist, MAX_EDGELIST_SIZE) + writelen >= MAX_EDGELIST_SIZE) {
					MessageBox(hwollymain,
						L"Maximum edges per function exceeded\n",
						L"Too many edges in graph", MB_OK|MB_ICONEXCLAMATION);
					free_list(disasm_list);
					return;
				}	   
				strcat(edgelist, buffer);
			}
			orphannode = 1;
			currentnode = current_addr;
			writelen = sprintf(buffer, "\" }\n"); 
			WriteFile(temp_file, buffer, (DWORD) writelen, &len_written, NULL);
			writelen = sprintf(buffer, "node: { title: \"%x\" color: 83 fontname: \"courR12\" label: \"%x:", current_addr, current_addr); 
			WriteFile(temp_file, buffer, (DWORD) writelen, &len_written, NULL);
		}
		writelen = sprintf(buffer, "\n");// re-initialize buffer
		Unicodetoascii(disasm_result.result, TEXTLEN, result_ascii, TEXTLEN);
	    cpuwindowtable = Getcpudisasmtable();
		cpuwindowdump = Getcpudisasmdump();
		oldAddress = cpuwindowdump->addr;
		Scrolldumpwindow(cpuwindowdump,disasm_result.ip+1000,0);
		Scrolldumpwindow(cpuwindowdump,disasm_result.ip,0);
		Updatetable(Getcpudisasmtable(),1);		
		returncode = Gettabletext(Getcpudisasmtable(),disasm_result.ip,2,buffer2,theMask,theInt);
		Scrolldumpwindow(cpuwindowdump,disasm_result.ip+1000,0);
		Scrolldumpwindow(cpuwindowdump,oldAddress,0);
		Updatetable(Getcpudisasmtable(),1);
		//Addtolist(disasm_result.ip,DRAW_NORMAL,buffer2);
		decode = Finddecode(disasm_result.ip, &dsize);
		if (decode)
		{
			if (((*decode)&DEC_TRACED)==DEC_TRACED)
			{
				if (theMask[0] & DRAW_COLOR & DRAW_HILITE == DRAW_HILITE)
				//Addtolist(theMask[0] & DRAW_COLOR,DRAW_NORMAL,buffer2);
					writelen = sprintf(buffer, "\n\\f02\\fi164\\f31"); 
				else
					writelen = sprintf(buffer, "\n\\fi164");
			}
		}
		
		extra = strlen(buffer);
		if (StrlenW(disasm_result.comment, TEXTLEN) > 0) {
			Unicodetoascii(disasm_result.comment, TEXTLEN, comment_ascii, TEXTLEN);
			sanitize(comment_ascii, clean_comment);
			writelen = sprintf(buffer+extra, "%s\t; %s", result_ascii, clean_comment); 
		} else {
			writelen = sprintf(buffer+extra, "%s", result_ascii); 
		}

		//Gettabletext seems to ignore the row parameter. All these lines return the same result
		//returncode = Gettabletext(Getcpudisasmtable(),0,2,buffer2,theMask,theInt);
		//Addtolist(returncode,DRAW_NORMAL,buffer2);
		//returncode = Gettabletext(Getcpudisasmtable(),1,2,buffer2,theMask,theInt);
		//Addtolist(returncode,DRAW_NORMAL,buffer2);
		//returncode = Gettabletext(Getcpudisasmtable(),4,2,buffer2,theMask,theInt);
		//Addtolist(returncode,DRAW_NORMAL,buffer2);

		WriteFile(temp_file, buffer, (DWORD)(writelen+extra), &len_written, NULL);
	
		if (previous_edge.source != 0) {
			// append stored edge info with currentnode info to edgelist buffer
			previous_edge.targetfalse = currentnode;
			writelen = sprintf(buffer, "edge: { sourcename: \"%x\" targetname: \"%x\" label: \"false\" color: red }\n", previous_edge.source, previous_edge.targetfalse);
			if (StrlenA(edgelist, MAX_EDGELIST_SIZE) + writelen >= MAX_EDGELIST_SIZE) {
				MessageBox(hwollymain,
					L"Maximum edges per function exceeded\n",
					L"Too many edges in graph", MB_OK|MB_ICONEXCLAMATION);
				free_list(disasm_list);
				return;
			}	   
			strcat(edgelist, buffer);
			writelen = sprintf(buffer, "edge: { sourcename: \"%x\" targetname: \"%x\" label: \"true\" color: darkgreen }\n", previous_edge.source, previous_edge.targettrue);
			if (StrlenA(edgelist, MAX_EDGELIST_SIZE) + writelen >= MAX_EDGELIST_SIZE) {
				MessageBox(hwollymain,
					L"Maximum edges per function exceeded\n",
					L"Too many edges in graph", MB_OK|MB_ICONEXCLAMATION);
				free_list(disasm_list);
				return;
			}	   
			strcat(edgelist, buffer);
			previous_edge.source = 0;
		}

		disasm_result = disasm_list_element->disasm;

		if ((disasm_result.jmpaddr >= start) && (disasm_result.jmpaddr < end)) {
			// this is a jump - start of an edge and pointer to a node
			orphannode = 0;
			previous_edge.source = currentnode;
			previous_edge.targettrue = disasm_result.jmpaddr;
			previous_edge.targetfalse = 0;
			if ((*disasm_result.result == 'J') && (*(disasm_result.result+1) == 'M')) {
				// straight jmp, no true/false
				writelen = sprintf(buffer, "edge: { sourcename: \"%x\" targetname: \"%x\" }\n", previous_edge.source, previous_edge.targettrue);

				if (StrlenA(edgelist, DATALEN) + writelen >= MAX_EDGELIST_SIZE) {
					MessageBox(hwollymain,
						L"Maximum edges per function exceeded\n",
						L"Too many edges in graph", MB_OK|MB_ICONEXCLAMATION);
					free_list(disasm_list);
					return;
				}	   
				strcat(edgelist, buffer);
				previous_edge.source = 0;
			}
		}
	}

	// close last node
	writelen = sprintf(buffer, "\" vertical_order: %d }\n", nodes); 
	WriteFile(temp_file, buffer, (DWORD)writelen, &len_written, NULL);

	// write edgelist 
	WriteFile(temp_file, edgelist, (DWORD)StrlenA(edgelist, MAX_EDGELIST_SIZE), &len_written, NULL);
  
	writelen = sprintf(buffer, "}\n"); 
	WriteFile(temp_file, buffer, (DWORD)writelen, &len_written, NULL);

	CloseHandle(temp_file);
	free_list(disasm_list);

	if ((int)ShellExecute(hwollymain, L"open", global_qwingraph_path, temp_name, NULL, SW_SHOWNORMAL) < 32) {
		Error(L"Error while executing %s\n(check qwingraph path configuration)", global_qwingraph_path);   
	}
}

void generate_function_call_graph(void)
{
	// not implemented
}

void generate_xrefs_to_address_graph(void)
{
	// not implemented
}

void generate_xrefs_from_address_graph(void)
{
	// not implemented
}

void append_nodelist(ulong addr)
{
	if (nodes > MAX_NODES) {
		return;
	}
	if (nodestart(addr)) {
		return;
	}
	nodelist[nodes] = addr;
	nodes++;
}

int nodestart(ulong addr)
{
	int i;
	for (i = 0; i < nodes; i++) {
		if (nodelist[i] == addr) {
			return 1;
		}
	}
	return 0;
}

void sanitize(char *comment, char *cleaned)
{
	char *t;

	for (t = cleaned; *comment; comment++) {
		if (*comment == '"') {
			*t++ = '\\';
			*t = '"';
		} else if (*comment == '\\') {
			*t++ = '\\';
			*t = '\\';
		} else {
			*t = *comment;
		}
		t++;
	}
	*t = '\0';
}  

static void free_list(void *t_disasm_list)
{
	void *next;
	struct disasm_list_element_s *tmp;
  
	if (t_disasm_list == NULL) {
		return;
	}
	tmp = (struct disasm_list_element_s *)t_disasm_list; 
	while (tmp->next_element != NULL) {
		next = tmp->next_element;
		Memfree(tmp);
		tmp = (struct disasm_list_element_s *)next;
		if (tmp == NULL) {
			return;
		}
	}
}

void cleanup_tempfiles(wchar_t *tmppath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	wchar_t DirSpec[MAX_PATH];
	wchar_t FoundFile[MAX_PATH];
	DWORD dwError;

	StrcopyW(DirSpec, MAX_PATH, tmppath);
	wcsncat(DirSpec, L"ogh*.tmp", 8);
	hFind = FindFirstFile(DirSpec, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	} else {
		StrcopyW(FoundFile, MAX_PATH, tmppath);
		wcscat(FoundFile, FindFileData.cFileName);
		DeleteFile(FoundFile);
		while (FindNextFile(hFind, &FindFileData) != 0) {
			StrcopyW(FoundFile, MAX_PATH, tmppath);
			wcscat(FoundFile, FindFileData.cFileName);
			DeleteFile(FoundFile);
		}
	
		dwError = GetLastError();
		if (dwError == ERROR_NO_MORE_FILES) {
			FindClose(hFind);
		} 
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////// PLUGIN INITIALIZATION /////////////////////////////

/**
 * @DllMain
 * 
 *      Dll entrypoint - mainly unused.
 */
BOOL WINAPI DllEntryPoint(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		main_plugin_instance = hinstDll;		/* Save plugin instance */
	return 1;							/* Report success */
};

/**
 * @ODBG2_Pluginquery - required!
 * 
 *      Handles initializing the plugin.
 */
extc int __cdecl ODBG2_Pluginquery(int ollydbgversion, ulong *features, wchar_t pluginname[SHORTNAME], wchar_t pluginversion[SHORTNAME])
{
	/*
	 * Check whether OllyDbg has compatible version. This plugin uses only the
	 * most basic functions, so this check is done pro forma, just to remind of
	 * this option.
	 */
	if (ollydbgversion < 201)
		return 0;
	/* Report name and version to OllyDbg */
	StrcopyW(pluginname, SHORTNAME, PLUGIN_NAME);
	StrcopyW(pluginversion, SHORTNAME, PLUGIN_VERS);
	return PLUGIN_VERSION;			/* Expected API version */
};

/**
 * @ODBG2_Plugininit - optional
 * 
 *      Handles one-time initializations and allocate resources.
 */
extc int __cdecl ODBG2_Plugininit(void)
{
	int ret = 0;

	load_settings(NULL);

	Addtolist(0, DRAW_NORMAL, L"");
	Addtolist(0, DRAW_NORMAL, L"[*] %s v%s", PLUGIN_NAME, PLUGIN_VERS);
	Addtolist(0, DRAW_NORMAL, L"[*] Coded by: Austyn Krutsinger <akrutsinger@gmail.com>");
	Addtolist(0, DRAW_NORMAL, L"");

	/* Report success. */
	return 0;
};

/*
 * Optional entry, called each time OllyDbg analyses some module and analysis
 * is finished. Plugin can make additional analysis steps. Debugged application
 * is paused for the time of processing. Bookmark plugin, of course, does not
 * analyse code. If you don't need this feature, remove ODBG2_Pluginanalyse()
 * from the plugin code.
 */
extc void __cdecl ODBG2_Pluginanalyse(t_module *pmod)
{

};

/*
 * OllyDbg calls this optional function once on exit. At this moment, all MDI
 * windows created by plugin are already destroyed (and received WM_DESTROY
 * messages). Function must free all internally allocated resources, like
 * window classes, files, memory etc.
 */
extc void __cdecl ODBG2_Plugindestroy(void)
{

};

/*
 * Function is called when user opens new or restarts current_addr application.
 * Plugin should reset internal variables and data structures to the initial
 * state.
 */
//extc void __cdecl ODBG2_Pluginreset(void)
//{
//};

/*
 * OllyDbg calls this optional function when user wants to terminate OllyDbg.
 * All MDI windows created by plugins still exist. Function must return 0 if
 * it is safe to terminate. Any non-zero return will stop closing sequence. Do
 * not misuse this possibility! Always inform user about the reasons why
 * termination is not good and ask for his decision! Attention, don't make any
 * unrecoverable actions for the case that some other plugin will decide that
 * OllyDbg should continue running.
 */
//extc int __cdecl ODBG2_Pluginclose(void)
//{
//	return 0;
//};


////////////////////////////////////////////////////////////////////////////////
/////////////////////////// EVENTS AND NOTIFICATIONS ///////////////////////////
/*
 * If you define ODBG2_Pluginmainloop(), this function will be called each time
 * from the main Windows loop in OllyDbg. If there is some (real) debug event
 * from the debugged application, debugevent points to it, otherwise it's NULL.
 * If fast command emulation is active, it does not receive all (emulated)
 * exceptions, use ODBG2_Pluginexception() instead. Do not declare these two
 * functions unnecessarily, as this may negatively influence the overall speed!
 */
// extc void __cdecl ODBG2_Pluginmainloop(DEBUG_EVENT *debugevent) {
// };
// extc void __cdecl ODBG2_Pluginexception(t_run *prun, t_thread *pthr, t_reg *preg) {
// };

/*
 * Optional entry, notifies plugin on relatively infrequent events.
 */
extc void __cdecl ODBG2_Pluginnotify(int code, void *data, ulong parm1, ulong parm2)
{
	if (code == PN_NEWPROC) {
//		if (global_scan_on_mod_load == TRUE)
//			scan_module();
	}
};