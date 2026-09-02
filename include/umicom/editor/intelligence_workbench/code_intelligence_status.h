/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_intelligence_status.h
 *
 * PURPOSE:
 *   Model code intelligence status as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_INTELLIGENCE_STATUS_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_INTELLIGENCE_STATUS_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel code intelligence status data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelCodeIntelligenceStatus { char session_id[UMI_EDITOR_INTEL_ID_CAPACITY]; UmiEditorIntelPhase phase; uint32_t item_count; bool changed; uint64_t revision; } UmiEditorIntelCodeIntelligenceStatus;
/**
 * Provide the editor intel code intelligence status begin operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_code_intelligence_status_begin(UmiEditorIntelCodeIntelligenceStatus *session,const char *session_id);
/**
 * Provide the editor intel code intelligence status set ready operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_code_intelligence_status_set_ready(UmiEditorIntelCodeIntelligenceStatus *session,uint32_t item_count);
/**
 * Provide the editor intel code intelligence status cancel operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_code_intelligence_status_cancel(UmiEditorIntelCodeIntelligenceStatus *session);
/**
 * Check that editor intel code intelligence status satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_code_intelligence_status_valid(const UmiEditorIntelCodeIntelligenceStatus *session);

#ifdef __cplusplus
}
#endif
#endif
