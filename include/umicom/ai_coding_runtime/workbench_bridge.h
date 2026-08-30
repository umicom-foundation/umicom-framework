/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/workbench_bridge.h
 *
 * PURPOSE:
 *   Bind executable AI commands from the existing Developer Workbench catalogue
 *   to the operational Framework coding runtime. View-opening commands remain
 *   presentation-owned until a thin product supplies the corresponding panel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_WORKBENCH_BRIDGE_H
#define UMICOM_AI_CODING_RUNTIME_WORKBENCH_BRIDGE_H

#include "umicom/ai_coding_runtime/platform.h"
#include "umicom/developer_workbench/workbench.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingWorkbenchContext {
    char session_id[UMI_AI_ID_CAPACITY];
    char runtime_id[UMI_AI_ID_CAPACITY];
    char active_path[UMI_AI_TEXT_CAPACITY];
    char language_id[UMI_AI_ID_CAPACITY];
    char instruction[UMI_AI_TEXT_CAPACITY];
    char problems_summary[UMI_AI_TEXT_CAPACITY];
    UmiAiDataClassification classification;
    uint32_t selection_start_line;
    uint32_t selection_end_line;
    uint64_t timestamp_ns;
    int sensitive_approved;
} UmiAiCodingWorkbenchContext;

typedef struct UmiAiCodingWorkbenchBridge UmiAiCodingWorkbenchBridge;

UmiStatus umi_ai_coding_workbench_bridge_create(
    UmiDeveloperWorkbench *workbench,
    UmiAiCodingRuntimePlatform *platform,
    UmiAiCodingWorkbenchBridge **out_bridge);

void umi_ai_coding_workbench_bridge_destroy(
    UmiAiCodingWorkbenchBridge *bridge);

UmiStatus umi_ai_coding_workbench_bridge_set_context(
    UmiAiCodingWorkbenchBridge *bridge,
    const UmiAiCodingWorkbenchContext *context);

UmiStatus umi_ai_coding_workbench_bridge_bind(
    UmiAiCodingWorkbenchBridge *bridge);

#ifdef __cplusplus
}
#endif
#endif
