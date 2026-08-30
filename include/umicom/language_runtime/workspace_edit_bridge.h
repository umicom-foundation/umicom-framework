/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/workspace_edit_bridge.h
 *
 * PURPOSE:
 *   Convert decoded LSP WorkspaceEdit into the existing conflict-aware Editor edit set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_WORKSPACE_EDIT_BRIDGE_H
#define UMICOM_LANGUAGE_RUNTIME_WORKSPACE_EDIT_BRIDGE_H
#include "umicom/editor/workspace_edit.h"
#include "umicom/language_runtime/results.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_workspace_edit_to_editor(const UmiLanguageRuntimeWorkspaceEdit*r,UmiEditorWorkspaceEditSet**out);
#ifdef __cplusplus
}
#endif
#endif
