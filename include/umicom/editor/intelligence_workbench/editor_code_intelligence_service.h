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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_EDITOR_CODE_INTELLIGENCE_SERVICE_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_EDITOR_CODE_INTELLIGENCE_SERVICE_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelEditorCodeIntelligenceService { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelEditorCodeIntelligenceService;
UmiStatus umi_editor_intel_editor_code_intelligence_service_init(UmiEditorIntelEditorCodeIntelligenceService *model);
UmiStatus umi_editor_intel_editor_code_intelligence_service_add(UmiEditorIntelEditorCodeIntelligenceService *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_editor_code_intelligence_service_find(const UmiEditorIntelEditorCodeIntelligenceService *model,const char *id);
UmiStatus umi_editor_intel_editor_code_intelligence_service_clear(UmiEditorIntelEditorCodeIntelligenceService *model);
int umi_editor_intel_editor_code_intelligence_service_valid(const UmiEditorIntelEditorCodeIntelligenceService *model);

#ifdef __cplusplus
}
#endif
#endif
