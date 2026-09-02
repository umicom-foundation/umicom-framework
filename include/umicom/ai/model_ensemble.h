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

/**
 * Represent the ai model target data shared with callers of this public contract.
 */
typedef struct UmiAiModelTarget {
    uint32_t structure_size;
    uint32_t api_version;
    char provider_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    char label[UMI_AI_SMALL_TEXT_CAPACITY];
    int enabled;
    int approved;
} UmiAiModelTarget;

/**
 * Represent the ai model result data shared with callers of this public contract.
 */
typedef struct UmiAiModelResult {
    uint32_t structure_size;
    uint32_t api_version;
    UmiAiModelTarget target;
    UmiStatus status;
    UmiAiResponse response;
} UmiAiModelResult;

/**
 * Represent the ai model ensemble report data shared with callers of this public contract.
 */
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

/**
 * Initialise ai model target from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_model_target_initialize(
    UmiAiModelTarget *target,
    const char *provider_id,
    const char *model_id,
    const char *label,
    int approved);

/**
 * Provide the ai model ensemble report initialize sized operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_model_ensemble_report_initialize_sized(
    UmiAiModelEnsembleReport *report,
    uint32_t caller_structure_size);

#define umi_ai_model_ensemble_report_initialize(report) \
    umi_ai_model_ensemble_report_initialize_sized( \
        (report), (uint32_t)sizeof(*(report)))

/**
 * Provide the ai model ensemble query sized operation used by this module and its client
 * applications.
 */
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

/**
 * Provide the ai model ensemble select operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_model_ensemble_select(
    UmiAiModelEnsembleReport *report,
    size_t result_index);

/**
 * Find ai model ensemble while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiAiModelResult *umi_ai_model_ensemble_selected(
    const UmiAiModelEnsembleReport *report);

#ifdef __cplusplus
}
#endif

#endif
