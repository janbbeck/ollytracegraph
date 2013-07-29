// OllyTraceGraph is a modification made by Jan Beck of OllyGraph by Austyn Krutsinger

// Thanks for providing the source, Austyn!
// https://github.com/akrutsinger/OllyGraph

// I don't claim any additional copyright for the modifications.

// From the original OllyGraph:

/*******************************************************************************
 * OllyGraph - OllyGraph.h
 *
 * Copyright (c) 2013, Austyn Krutsinger
 * All rights reserved.
 *
 * OllyGraph is released under the New BSD license (see LICENSE.txt).
 *
 ******************************************************************************/

#ifndef _OLLYGRAPH_H_
#define _OLLYGRAPH_H_

#define PLUGIN_NAME		L"OllyTraceGraph"		/* Unique plugin name */
#define PLUGIN_VERS		L"1.0.0"			/* Plugin version (stable . update . patch  - status) */

/* Menu items */
#define	MENU_FUNCTION_FLOWCHART			1
#define MENU_FUNCTION_CALL_GRAPH		2
#define MENU_XREFS_TO_ADDRESS_GRAPH		3
#define MENU_XREFS_FROM_ADDRESS_GRAPH	4
#define	MENU_SETTINGS					5
#define	MENU_ABOUT						6
#define MENU_TEST_CODE					255

#define MAX_EDGELIST_SIZE 256000
#define MAX_NODES 512

/**
 * Forward declarations
 */
 /* Menu functions */
int menu_handler(t_table *pTable, wchar_t *pName, ulong index, int nMode);
void display_about_message(void);

/* Helper functions */
void generate_function_flowchart(void);
void generate_function_call_graph(void);		// not implemented
void generate_xrefs_to_address_graph(void);		// not implemented
void generate_xrefs_from_address_graph(void);	// not implemented

static void free_list(void *t_disasm_list);
static void append_nodelist(ulong addr);
static int nodestart(ulong addr);
static void sanitize(char *comment, char *cleaned); 
static void cleanup_tempfiles(wchar_t *tmppath);

struct disasm_list_element_s {
	t_disasm disasm;
	struct disasm_list_element_s *next_element;
};

struct previous_edge_struct_s {
	ulong source;
	ulong targetfalse;
	ulong targettrue;
};

/*
 * Plugin menu that will appear in the main OllyDbg menu
 * and in popup menu.
 */
static t_menu ollygraph_menu[] =
{
	{ L"Generate Function Flowchart",
		L"Generate Function Flowchart",
		KK_DIRECT|KK_CTRL|KK_SHIFT|'F', menu_handler, NULL, MENU_FUNCTION_FLOWCHART },
	//{ L"Generate Function Call Graph",
	//	L"Generate Function Call Graph",
	//	KK_DIRECT|KK_CTRL|KK_SHIFT|'C', menu_handler, NULL, MENU_FUNCTION_CALL_GRAPH },
	//{ L"Generate XRefs To Address Graph",
	//	L"Generate XRefs To Address Graph",
	//	KK_DIRECT|KK_CTRL|KK_SHIFT|'X', menu_handler, NULL, MENU_XREFS_TO_ADDRESS_GRAPH },
	//{ L"Generate XRefs From Address Graph",
	//	L"Generate XRefs From Address Graph",
	//	KK_DIRECT|KK_CTRL|KK_SHIFT|'A', menu_handler, NULL, MENU_XREFS_FROM_ADDRESS_GRAPH },
	{ L"|Settings",
		L"Configure Path to Handler Program",
		K_NONE, menu_handler, NULL, MENU_SETTINGS },
	{ L"|About",
		L"About OllyGraph",
		K_NONE, menu_handler, NULL, MENU_ABOUT },
	/* End of menu. */
	{ NULL, NULL, K_NONE, NULL, NULL, 0 }
};

/*
 * Plugin menu that will appear in the popup menu.
 */
static t_menu ollygraph_popup_menu[] =
{
	{ L"Generate Function Flowchart",
		L"Generate Function Flowchart",
		KK_DIRECT|KK_CTRL|KK_SHIFT|'F', menu_handler, NULL, MENU_FUNCTION_FLOWCHART },
	//{ L"Generate Function Call Graph",
	//	L"Generate Function Call Graph",
	//	KK_DIRECT|KK_CTRL|KK_SHIFT|'C', menu_handler, NULL, MENU_FUNCTION_CALL_GRAPH },
	//{ L"Generate XRefs To Address Graph",
	//	L"Generate XRefs To Address Graph",
	//	KK_DIRECT|KK_CTRL|KK_SHIFT|'X', menu_handler, NULL, MENU_XREFS_TO_ADDRESS_GRAPH },
	{ L"|Settings",
		L"Configure Path to Handler Program",
		K_NONE, menu_handler, NULL, MENU_SETTINGS },
	/* End of menu. */
	{ NULL, NULL, K_NONE, NULL, NULL, 0 }
};

#endif /* _OLLYGRAPH_H_ */