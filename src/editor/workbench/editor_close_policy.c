/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_close_policy.c
 *
 * PURPOSE:
 *   Evaluate whether closing a dirty/pinned item is allowed, denied or requires confirmation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_close_policy.h"

/*
 * Initialise editor wb editor close policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_editor_close_policy_init(UmiEditorWbEditorClosePolicy *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL){p->confirm_dirty=true;p->protect_pinned=true;p->allow_force=true;}}
/*
 * Provide the editor wb editor close policy evaluate operation used by this module and its
 * client applications.
 */
UmiEditorWbCloseDecision umi_editor_wb_editor_close_policy_evaluate(const UmiEditorWbEditorClosePolicy *p,bool dirty,bool pinned,bool force){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_EDITOR_WB_CLOSE_DENY;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(force&&p->allow_force)return UMI_EDITOR_WB_CLOSE_ALLOW;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pinned&&p->protect_pinned)return UMI_EDITOR_WB_CLOSE_DENY;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(dirty&&p->confirm_dirty)return UMI_EDITOR_WB_CLOSE_CONFIRM;return UMI_EDITOR_WB_CLOSE_ALLOW;}
