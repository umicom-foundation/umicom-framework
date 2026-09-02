/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/focus_mode.c
 *
 * PURPOSE:
 *   Track focus mode while preserving the previously visible workbench regions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/focus_mode.h"

#include <string.h>
/*
 * Initialise editor wb focus mode from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_editor_wb_focus_mode_init(UmiEditorWbFocusMode *s,const char *id,bool enabled){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_editor_wb_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->item_id,sizeof s->item_id,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->enabled=enabled;s->revision=1U;return UMI_STATUS_OK;}
/*
 * Copy editor wb focus mode into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_editor_wb_focus_mode_set(UmiEditorWbFocusMode *s,bool enabled){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->enabled=enabled;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(enabled)s->promoted=true;s->revision++;return UMI_STATUS_OK;}
/*
 * Check that editor wb focus mode satisfies its contract before another service relies on
 * it.
 */
int umi_editor_wb_focus_mode_valid(const UmiEditorWbFocusMode *s){return s!=NULL&&umi_editor_wb_id_valid(s->item_id)&&s->revision>0U;}
