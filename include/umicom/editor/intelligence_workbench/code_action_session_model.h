/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_action_session_model.h
 *
 * PURPOSE:
 *   Model code action session model as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_SESSION_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_SESSION_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel code action session model data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelCodeActionSessionModel { char session_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t item_count; bool changed; uint64_t revision; } UmiEditorIntelCodeActionSessionModel;
/**
 * Provide the editor intel code action session model begin operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_code_action_session_model_begin(UmiEditorIntelCodeActionSessionModel *session,const char *session_id);
/**
 * Provide the editor intel code action session model set ready operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_code_action_session_model_set_ready(UmiEditorIntelCodeActionSessionModel *session,uint32_t item_count);
/**
 * Provide the editor intel code action session model cancel operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_code_action_session_model_cancel(UmiEditorIntelCodeActionSessionModel *session);
/**
 * Check that editor intel code action session model satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_code_action_session_model_valid(const UmiEditorIntelCodeActionSessionModel *session);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_intel_code_action_session_model_archive_encode(const UmiEditorIntelCodeActionSessionModel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_intel_code_action_session_model_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorIntelCodeActionSessionModel *value);

#ifdef __cplusplus
}
#endif
#endif
