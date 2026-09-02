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

/*
 * Initialise editor wb editor search match from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_search_match_init(UmiEditorWbEditorSearchMatch *m,UmiEditorWbRange r,bool selected){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return UMI_STATUS_INVALID_ARGUMENT;m->range=umi_editor_wb_range_normalize(r);m->selected=selected;return UMI_STATUS_OK;}
