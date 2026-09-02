/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/viewport_sync.c
 *
 * PURPOSE:
 *   Synchronize selected viewport dimensions between linked editors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/viewport_sync.h"

/*
 * Perform editor wb viewport sync through the module contract so client applications do
 * not duplicate its policy.
 */
void umi_editor_wb_viewport_sync_apply(UmiEditorWbViewportSync *s,double h,double v){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return;s->master_horizontal=h;s->master_vertical=v;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->horizontal)s->follower_horizontal=h;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->vertical)s->follower_vertical=v;}
