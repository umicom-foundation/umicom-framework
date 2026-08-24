/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/inline_executor.h
 *
 * PURPOSE:
 *   Execute low-latency inline code completion through the existing UmiAiRuntime
 *   provider boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_INLINE_EXECUTOR_H
#define UMICOM_IDE_INTEGRATION_INLINE_EXECUTOR_H

#include "umicom/ai/runtime.h"
#include "umicom/ide_integration/selection.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeInlineExecutor {
    UmiAiRuntime *runtime;
    char provider_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    uint32_t max_output_tokens;
    double temperature;
    int provider_approved;
} UmiIdeInlineExecutor;

UmiStatus umi_ide_inline_executor_init(
    UmiIdeInlineExecutor *executor,
    UmiAiRuntime *runtime,
    const char *provider_id,
    const char *model_id);

UmiStatus umi_ide_inline_execute(
    UmiIdeInlineExecutor *executor,
    const UmiIdeEditorSelection *context,
    uint64_t request_sequence,
    UmiIdeInlineSuggestion *out_suggestion);

#ifdef __cplusplus
}
#endif
#endif
