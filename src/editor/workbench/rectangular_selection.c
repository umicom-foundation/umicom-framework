/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/rectangular_selection.c
 *
 * PURPOSE:
 *   Normalize block/column selection coordinates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/rectangular_selection.h"

/*
 * Initialise editor wb rectangular selection from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_wb_rectangular_selection_init(UmiEditorWbRectangularSelection *s,uint32_t la,uint32_t ca,uint32_t lb,uint32_t cb){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||la==0U||lb==0U||ca==0U||cb==0U)return UMI_STATUS_INVALID_ARGUMENT;s->first_line=la<lb?la:lb;s->last_line=la>lb?la:lb;s->first_column=ca<cb?ca:cb;s->last_column=ca>cb?ca:cb;return UMI_STATUS_OK;}
/*
 * Return the number of records represented by editor wb rectangular selection line without
 * changing their state.
 */
size_t umi_editor_wb_rectangular_selection_line_count(const UmiEditorWbRectangularSelection *s){return s==NULL?0U:(size_t)(s->last_line-s->first_line+1U);}
