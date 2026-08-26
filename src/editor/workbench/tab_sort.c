/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/tab_sort.c
 *
 * PURPOSE:
 *   Provide deterministic editor-tab ordering rules.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/tab_sort.h"

#include <string.h>
int umi_editor_wb_tab_sort_compare(const UmiEditorWbTabSortKey *a,const UmiEditorWbTabSortKey *b){if(a==NULL||b==NULL)return 0;if(a->pinned!=b->pinned)return a->pinned?-1:1;if(a->dirty!=b->dirty)return a->dirty?-1:1;return strcmp(a->label,b->label);}
