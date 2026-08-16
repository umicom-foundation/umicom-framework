/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/result.c
 *
 * PURPOSE:
 *   Implement build-result lifecycle and concise summaries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/result.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UmiStatus umi_build_result_create(UmiBuildResult **out_result)
{
    UmiBuildResult *result;
    if (out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_result = NULL;
    result = (UmiBuildResult *)calloc(1U, sizeof(*result));
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *out_result = result;
    return UMI_STATUS_OK;
}

void umi_build_result_destroy(UmiBuildResult *result)
{
    free(result);
}

void umi_build_result_init(UmiBuildResult *result,
                           uint64_t operation_id,
                           UmiBuildPhase phase,
                           const char *profile_id)
{
    if (result == NULL) {
        return;
    }
    (void)memset(result, 0, sizeof(*result));
    result->operation_id = operation_id;
    result->phase = phase;
    result->state = UMI_BUILD_STATE_CREATED;
    result->status = UMI_STATUS_OK;
    if (profile_id != NULL) {
        (void)snprintf(result->profile_id,
                       sizeof(result->profile_id),
                       "%s",
                       profile_id);
    }
    umi_build_diagnostic_list_init(&result->diagnostics);
}

void umi_build_result_finish(UmiBuildResult *result,
                             UmiStatus status,
                             int exit_code,
                             uint64_t duration_ms)
{
    if (result == NULL) {
        return;
    }
    result->status = status;
    result->exit_code = exit_code;
    result->duration_ms = duration_ms;
    if (status == UMI_STATUS_OK && exit_code == 0) {
        result->state = UMI_BUILD_STATE_SUCCEEDED;
    } else if (status == UMI_STATUS_CANCELLED) {
        result->state = UMI_BUILD_STATE_CANCELLED;
    } else if (status == UMI_STATUS_TIMEOUT) {
        result->state = UMI_BUILD_STATE_TIMED_OUT;
    } else {
        result->state = UMI_BUILD_STATE_FAILED;
    }
}

UmiStatus umi_build_result_summary(const UmiBuildResult *result,
                                   char *out_text,
                                   size_t capacity)
{
    int written;
    if (result == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(
        out_text,
        capacity,
        "Build %llu %s: %s, exit=%d, duration=%llums, "
        "diagnostics=%zu, errors=%zu",
        (unsigned long long)result->operation_id,
        umi_build_phase_text(result->phase),
        umi_build_state_text(result->state),
        result->exit_code,
        (unsigned long long)result->duration_ms,
        result->diagnostics.count,
        umi_build_diagnostic_list_count_severity(
            &result->diagnostics,
            UMI_BUILD_DIAGNOSTIC_ERROR
        )
    );
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}
