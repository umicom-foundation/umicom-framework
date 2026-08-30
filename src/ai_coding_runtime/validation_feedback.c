/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_feedback.c
 *
 * PURPOSE:
 *   Format validation failures as explicit model repair evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/validation_feedback.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_ai_coding_validation_feedback(
    const UmiAiCodingValidationReport *report,
    char *out_text,
    size_t capacity)
{
    size_t index;
    size_t used = 0U;

    if (report == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_text[0] = '\0';

    for (index = 0U; index < report->result_count; ++index) {
        const UmiAiCodingValidationResult *result =
            &report->results[index];
        int written;

        if (result->passed) continue;

        written = snprintf(
            out_text + used,
            capacity - used,
            "Validation step '%s' (%s) failed with exit code %d.\n"
            "Output:\n%s\n---\n",
            result->step_id,
            umi_ai_coding_validation_kind_text(result->kind),
            result->exit_code,
            result->output);

        if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
        if ((size_t)written >= capacity - used) {
            out_text[capacity - 1U] = '\0';
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        used += (size_t)written;
    }

    return UMI_STATUS_OK;
}
