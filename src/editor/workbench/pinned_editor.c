/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/pinned_editor.c
 *
 * PURPOSE:
 *   Track whether an editor is pinned against preview-style replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/pinned_editor.h"

#include <string.h>
/*
 * Initialise editor wb pinned editor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_pinned_editor_init(UmiEditorWbPinnedEditor *s,const char *id,bool enabled){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_editor_wb_id_valid(id))return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->item_id,sizeof s->item_id,id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->enabled=enabled;s->revision=1U;return UMI_STATUS_OK;}
/*
 * Copy editor wb pinned editor into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_editor_wb_pinned_editor_set(UmiEditorWbPinnedEditor *s,bool enabled){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->enabled=enabled;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(enabled)s->promoted=true;s->revision++;return UMI_STATUS_OK;}
/*
 * Check that editor wb pinned editor satisfies its contract before another service relies
 * on it.
 */
int umi_editor_wb_pinned_editor_valid(const UmiEditorWbPinnedEditor *s){return s!=NULL&&umi_editor_wb_id_valid(s->item_id)&&s->revision>0U;}
