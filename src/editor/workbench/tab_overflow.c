/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/tab_overflow.c
 *
 * PURPOSE:
 *   Calculate visible and overflowed editor-tab counts for available width.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/tab_overflow.h"

UmiStatus umi_editor_wb_tab_overflow_resolve(UmiEditorWbTabOverflow *s,size_t total,int32_t width,int32_t tabw){size_t visible;if(s==NULL||width<0||tabw<=0)return UMI_STATUS_INVALID_ARGUMENT;visible=(size_t)(width/tabw);if(visible>total)visible=total;s->total_tabs=total;s->visible_tabs=visible;s->overflow_tabs=total-visible;s->available_width=width;s->preferred_tab_width=tabw;return UMI_STATUS_OK;}
