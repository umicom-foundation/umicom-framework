/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_search_match.c
 *
 * PURPOSE:
 *   Describe one in-editor search match and its selected state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_search_match.h"

UmiStatus umi_editor_wb_editor_search_match_init(UmiEditorWbEditorSearchMatch *m,UmiEditorWbRange r,bool selected){if(m==NULL)return UMI_STATUS_INVALID_ARGUMENT;m->range=umi_editor_wb_range_normalize(r);m->selected=selected;return UMI_STATUS_OK;}
