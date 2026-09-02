/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_conformance.h
 *
 * PURPOSE:
 *   Evaluate editor production-readiness requirements across frontend renderers.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_CONFORMANCE_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_CONFORMANCE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor conformance data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbEditorConformance { bool split_groups; bool navigation; bool search; bool multi_cursor; bool accessibility; bool session_restore; } UmiEditorWbEditorConformance;
/**
 * Return the number of records represented by editor wb editor conformance pass without
 * changing their state.
 */
size_t umi_editor_wb_editor_conformance_pass_count(const UmiEditorWbEditorConformance *state);
/**
 * Provide the editor wb editor conformance ready operation used by this module and its
 * client applications.
 */
int umi_editor_wb_editor_conformance_ready(const UmiEditorWbEditorConformance *state);

#ifdef __cplusplus
}
#endif
#endif
