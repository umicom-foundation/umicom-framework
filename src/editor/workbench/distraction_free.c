/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/distraction_free.c
 *
 * PURPOSE:
 *   Resolve which editor chrome elements remain visible in distraction-free mode.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/distraction_free.h"

void umi_editor_wb_distraction_free_resolve(UmiEditorWbDistractionFree *s,bool enabled){if(s==NULL)return;s->enabled=enabled;s->show_tabs=!enabled;s->show_breadcrumbs=!enabled;s->show_minimap=!enabled;s->show_status=true;}
