/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/model_ensemble.h
 *
 * PURPOSE:
 *   Define a bounded, provider-neutral request fan-out and result comparison
 *   contract for local and remote language models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A model ensemble sends the same request to several selected providers. Each
 * result keeps its own status, so one unavailable model does not hide useful
 * answers returned by the other models. Existing runtime policy still decides
 * whether a local, remote or tool-enabled request is allowed.
 */
#ifndef UMICOM_AI_MODEL_ENSEMBLE_H
#define UMICOM_AI_MODEL_ENSEMBLE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/ai/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_MODEL_ENSEMBLE_API_VERSION 1U
#define UMI_AI_MODEL_ENSEMBLE_MAX_TARGETS 8U

typedef struct UmiAiModelTarget {
    uint32_t structure_size;
    uint32_t api_version;
    char provider_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    char label[UMI_AI_SMALL_TEXT_CAPACITY];
    int enabled;
    int approved;
} UmiAiModelTarget;

typedef struct UmiAiModelResult {
    uint32_t structure_size;
    uint32_t api_version;
    UmiAiModelTarget target;
    UmiStatus status;
    UmiAiResponse response;
} UmiAiModelResult;

typedef struct UmiAiModelEnsembleReport {
    uint32_t structure_size;
    uint32_t api_version;
    char request_id[UMI_AI_ID_CAPACITY];
    UmiAiModelResult results[UMI_AI_MODEL_ENSEMBLE_MAX_TARGETS];
    size_t result_count;
    size_t success_count;
    size_t selected_index;
    uint64_t revision;
} UmiAiModelEnsembleReport;

UmiStatus umi_ai_model_target_initialize(
    UmiAiModelTarget *target,
    const char *provider_id,
    const char *model_id,
    const char *label,
    int approved);

UmiStatus umi_ai_model_ensemble_report_initialize_sized(
    UmiAiModelEnsembleReport *report,
    uint32_t caller_structure_size);

#define umi_ai_model_ensemble_report_initialize(report) \
    umi_ai_model_ensemble_report_initialize_sized( \
        (report), (uint32_t)sizeof(*(report)))

UmiStatus umi_ai_model_ensemble_query_sized(
    UmiAiRuntime *runtime,
    const UmiAiRequest *request,
    const UmiAiModelTarget *targets,
    size_t target_count,
    UmiAiModelEnsembleReport *report,
    uint32_t caller_report_size);

#define umi_ai_model_ensemble_query( \
    runtime, request, targets, target_count, report) \
    umi_ai_model_ensemble_query_sized( \
        (runtime), (request), (targets), (target_count), (report), \
        (uint32_t)sizeof(*(report)))

UmiStatus umi_ai_model_ensemble_select(
    UmiAiModelEnsembleReport *report,
    size_t result_index);

const UmiAiModelResult *umi_ai_model_ensemble_selected(
    const UmiAiModelEnsembleReport *report);

#ifdef __cplusplus
}
#endif

#endif
