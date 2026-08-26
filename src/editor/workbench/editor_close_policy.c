/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_close_policy.c
 *
 * PURPOSE:
 *   Evaluate whether closing a dirty/pinned item is allowed, denied or requires confirmation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_close_policy.h"

void umi_editor_wb_editor_close_policy_init(UmiEditorWbEditorClosePolicy *p){if(p!=NULL){p->confirm_dirty=true;p->protect_pinned=true;p->allow_force=true;}}
UmiEditorWbCloseDecision umi_editor_wb_editor_close_policy_evaluate(const UmiEditorWbEditorClosePolicy *p,bool dirty,bool pinned,bool force){if(p==NULL)return UMI_EDITOR_WB_CLOSE_DENY;if(force&&p->allow_force)return UMI_EDITOR_WB_CLOSE_ALLOW;if(pinned&&p->protect_pinned)return UMI_EDITOR_WB_CLOSE_DENY;if(dirty&&p->confirm_dirty)return UMI_EDITOR_WB_CLOSE_CONFIRM;return UMI_EDITOR_WB_CLOSE_ALLOW;}
