/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/tab_overflow.c
 *
 * PURPOSE:
 *   Calculate visible and overflowed editor-tab counts for available width.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/tab_overflow.h"

/*
 * Provide the editor wb tab overflow resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_wb_tab_overflow_resolve(UmiEditorWbTabOverflow *s,size_t total,int32_t width,int32_t tabw){size_t visible;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||width<0||tabw<=0)return UMI_STATUS_INVALID_ARGUMENT;visible=(size_t)(width/tabw);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(visible>total)visible=total;s->total_tabs=total;s->visible_tabs=visible;s->overflow_tabs=total-visible;s->available_width=width;s->preferred_tab_width=tabw;return UMI_STATUS_OK;}
