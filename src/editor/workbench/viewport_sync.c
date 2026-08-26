/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/viewport_sync.c
 *
 * PURPOSE:
 *   Synchronize selected viewport dimensions between linked editors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/viewport_sync.h"

void umi_editor_wb_viewport_sync_apply(UmiEditorWbViewportSync *s,double h,double v){if(s==NULL)return;s->master_horizontal=h;s->master_vertical=v;if(s->horizontal)s->follower_horizontal=h;if(s->vertical)s->follower_vertical=v;}
