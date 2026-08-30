/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/scroll_state.c
 *
 * PURPOSE:
 *   Track and clamp horizontal/vertical editor scroll offsets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/scroll_state.h"

static double clampd(double v,double lo,double hi){return v<lo?lo:(v>hi?hi:v);}
UmiStatus umi_editor_wb_scroll_state_init(UmiEditorWbScrollState *s,double mh,double mv){if(s==NULL||mh<0.0||mv<0.0)return UMI_STATUS_INVALID_ARGUMENT;s->horizontal=0.0;s->vertical=0.0;s->max_horizontal=mh;s->max_vertical=mv;return UMI_STATUS_OK;}
UmiStatus umi_editor_wb_scroll_state_set(UmiEditorWbScrollState *s,double h,double v){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->horizontal=clampd(h,0.0,s->max_horizontal);s->vertical=clampd(v,0.0,s->max_vertical);return UMI_STATUS_OK;}
