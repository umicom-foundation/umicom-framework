/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_close_policy.h
 *
 * PURPOSE:
 *   Evaluate whether closing a dirty/pinned item is allowed, denied or requires confirmation.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_CLOSE_POLICY_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_CLOSE_POLICY_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor close policy data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbEditorClosePolicy { bool confirm_dirty; bool protect_pinned; bool allow_force; } UmiEditorWbEditorClosePolicy;
/**
 * Initialise editor wb editor close policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_editor_close_policy_init(UmiEditorWbEditorClosePolicy *policy);
/**
 * Provide the editor wb editor close policy evaluate operation used by this module and its
 * client applications.
 */
UmiEditorWbCloseDecision umi_editor_wb_editor_close_policy_evaluate(const UmiEditorWbEditorClosePolicy *policy,bool dirty,bool pinned,bool force);

#ifdef __cplusplus
}
#endif
#endif
