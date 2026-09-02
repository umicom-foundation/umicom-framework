/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/tab_sort.c
 *
 * PURPOSE:
 *   Provide deterministic editor-tab ordering rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/tab_sort.h"

#include <string.h>
/*
 * Provide the editor wb tab sort compare operation used by this module and its client
 * applications.
 */
int umi_editor_wb_tab_sort_compare(const UmiEditorWbTabSortKey *a,const UmiEditorWbTabSortKey *b){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a==NULL||b==NULL)return 0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a->pinned!=b->pinned)return a->pinned?-1:1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(a->dirty!=b->dirty)return a->dirty?-1:1;return strcmp(a->label,b->label);}
