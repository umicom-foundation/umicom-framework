/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/editor_code_intelligence_service.h
 *
 * PURPOSE:
 *   Aggregate Framework-owned search, navigation, code-action and refactoring state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_EDITOR_CODE_INTELLIGENCE_SERVICE_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_EDITOR_CODE_INTELLIGENCE_SERVICE_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel editor code intelligence service data shared with callers of
 * this public contract.
 */
typedef struct UmiEditorIntelEditorCodeIntelligenceService { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelEditorCodeIntelligenceService;
/**
 * Initialise editor intel editor code intelligence service from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_editor_intel_editor_code_intelligence_service_init(UmiEditorIntelEditorCodeIntelligenceService *model);
/**
 * Add editor intel editor code intelligence service only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_editor_intel_editor_code_intelligence_service_add(UmiEditorIntelEditorCodeIntelligenceService *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel editor code intelligence service while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_editor_code_intelligence_service_find(const UmiEditorIntelEditorCodeIntelligenceService *model,const char *id);
/**
 * Release or reset state held by editor intel editor code intelligence service so the same
 * storage can be reused safely.
 */
UmiStatus umi_editor_intel_editor_code_intelligence_service_clear(UmiEditorIntelEditorCodeIntelligenceService *model);
/**
 * Check that editor intel editor code intelligence service satisfies its contract before
 * another service relies on it.
 */
int umi_editor_intel_editor_code_intelligence_service_valid(const UmiEditorIntelEditorCodeIntelligenceService *model);

#ifdef __cplusplus
}
#endif
#endif
