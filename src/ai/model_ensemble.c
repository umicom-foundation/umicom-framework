/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/model_ensemble.c
 *
 * PURPOSE:
 *   Run one bounded AI request through several policy-governed providers and
 *   retain each result for side-by-side comparison.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module never connects to a provider directly. It delegates every call
 * to UmiAiRuntime, which applies the application's remote-provider and approval
 * rules before invoking a registered adapter.
 */
#include "umicom/ai/model_ensemble.h"

#include <stdio.h>
#include <string.h>

static int has_bounded_text(const char *text, size_t capacity)
{
    /* A bounded search rejects corrupt records whose arrays lack a terminator. */
    return text != NULL && capacity > 0U &&
           memchr(text, '\0', capacity) != NULL && text[0] != '\0';
}

static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL) return 0;
    written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

static int target_is_valid(const UmiAiModelTarget *target)
{
    /* Self-description prevents an older or incomplete caller record being read. */
    return target != NULL &&
           target->structure_size >= sizeof(*target) &&
           target->api_version == UMI_AI_MODEL_ENSEMBLE_API_VERSION &&
           has_bounded_text(target->provider_id, sizeof(target->provider_id)) &&
           has_bounded_text(target->model_id, sizeof(target->model_id)) &&
           memchr(target->label, '\0', sizeof(target->label)) != NULL;
}

UmiStatus umi_ai_model_target_initialize(
    UmiAiModelTarget *target,
    const char *provider_id,
    const char *model_id,
    const char *label,
    int approved)
{
    if (target == NULL || provider_id == NULL || model_id == NULL ||
        label == NULL || provider_id[0] == '\0' || model_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Initialise all padding and future fields before publishing the record. */
    (void)memset(target, 0, sizeof(*target));
    target->structure_size = (uint32_t)sizeof(*target);
    target->api_version = UMI_AI_MODEL_ENSEMBLE_API_VERSION;
    target->enabled = 1;
    target->approved = approved != 0;
    if (!copy_text(target->provider_id, sizeof(target->provider_id), provider_id) ||
        !copy_text(target->model_id, sizeof(target->model_id), model_id) ||
        !copy_text(target->label, sizeof(target->label), label)) {
        (void)memset(target, 0, sizeof(*target));
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_model_ensemble_report_initialize_sized(
    UmiAiModelEnsembleReport *report,
    uint32_t caller_structure_size)
{
    if (report == NULL || caller_structure_size < sizeof(*report)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* The sentinel means no successful answer has been selected yet. */
    (void)memset(report, 0, sizeof(*report));
    report->structure_size = caller_structure_size;
    report->api_version = UMI_AI_MODEL_ENSEMBLE_API_VERSION;
    report->selected_index = SIZE_MAX;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_model_ensemble_query_sized(
    UmiAiRuntime *runtime,
    const UmiAiRequest *request,
    const UmiAiModelTarget *targets,
    size_t target_count,
    UmiAiModelEnsembleReport *report,
    uint32_t caller_report_size)
{
    size_t index;
    UmiStatus failure_status = UMI_STATUS_INVALID_STATE;
    UmiStatus status;

    /* Size is validated before the report is touched, protecting older callers. */
    if (runtime == NULL || request == NULL || targets == NULL || report == NULL ||
        target_count == 0U ||
        target_count > UMI_AI_MODEL_ENSEMBLE_MAX_TARGETS ||
        caller_report_size < sizeof(*report) ||
        memchr(request->request_id, '\0', sizeof(request->request_id)) == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Validate every target before any provider can observe the request. */
    for (index = 0U; index < target_count; ++index) {
        if (!target_is_valid(&targets[index])) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    status = umi_ai_model_ensemble_report_initialize_sized(
        report, caller_report_size);
    if (status != UMI_STATUS_OK) return status;
    if (!copy_text(report->request_id, sizeof(report->request_id),
                   request->request_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < target_count; ++index) {
        UmiAiModelResult *result;
        UmiAiRequest routed_request;

        /* Disabled targets are configuration choices, not failed provider calls. */
        if (!targets[index].enabled) continue;
        result = &report->results[report->result_count++];
        (void)memset(result, 0, sizeof(*result));
        result->structure_size = (uint32_t)sizeof(*result);
        result->api_version = UMI_AI_MODEL_ENSEMBLE_API_VERSION;
        result->target = targets[index];
        umi_ai_response_init(&result->response);

        /* Each provider receives its own request copy and chosen model identifier. */
        routed_request = *request;
        if (!copy_text(routed_request.model_id, sizeof(routed_request.model_id),
                       targets[index].model_id)) {
            result->status = UMI_STATUS_CAPACITY_EXCEEDED;
        } else {
            result->status = umi_ai_runtime_generate(
                runtime, targets[index].provider_id, targets[index].approved,
                &routed_request, &result->response);
        }
        if (result->status == UMI_STATUS_OK &&
            (memchr(result->response.request_id, '\0',
                    sizeof(result->response.request_id)) == NULL ||
             memchr(result->response.provider_id, '\0',
                    sizeof(result->response.provider_id)) == NULL ||
             memchr(result->response.model_id, '\0',
                    sizeof(result->response.model_id)) == NULL ||
             memchr(result->response.text, '\0',
                    sizeof(result->response.text)) == NULL)) {
            /* A provider may not publish unterminated data across the ABI. */
            result->status = UMI_STATUS_INTERNAL_ERROR;
        }
        if (result->status == UMI_STATUS_OK) {
            ++report->success_count;
            if (report->selected_index == SIZE_MAX) {
                report->selected_index = report->result_count - 1U;
            }
        } else {
            /* Keep the last useful failure for an all-failed return value. */
            failure_status = result->status;
        }
    }

    /* A revision lets UI adapters recognise a newly completed comparison. */
    report->revision = 1U;
    if (report->result_count == 0U) return UMI_STATUS_INVALID_STATE;
    return report->success_count > 0U ? UMI_STATUS_OK : failure_status;
}

UmiStatus umi_ai_model_ensemble_select(
    UmiAiModelEnsembleReport *report,
    size_t result_index)
{
    if (report == NULL || report->structure_size < sizeof(*report) ||
        report->api_version != UMI_AI_MODEL_ENSEMBLE_API_VERSION ||
        result_index >= report->result_count ||
        report->results[result_index].status != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Selection changes presentation only; it never re-runs a provider call. */
    report->selected_index = result_index;
    ++report->revision;
    return UMI_STATUS_OK;
}

const UmiAiModelResult *umi_ai_model_ensemble_selected(
    const UmiAiModelEnsembleReport *report)
{
    if (report == NULL || report->structure_size < sizeof(*report) ||
        report->api_version != UMI_AI_MODEL_ENSEMBLE_API_VERSION ||
        report->selected_index >= report->result_count) {
        return NULL;
    }
    return &report->results[report->selected_index];
}
