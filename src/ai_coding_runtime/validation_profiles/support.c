/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_profiles/support.c
 *
 * PURPOSE:
 *   Implement reusable validation-step construction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "support.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;

    if (out == NULL || capacity == 0U || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(text);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_validation_profile_add(
    UmiAiCodingValidationPlan *plan,
    const char *id,
    UmiAiCodingValidationKind kind,
    const char *label,
    const char *program,
    const char *arguments,
    const char *working_directory,
    int required,
    uint32_t timeout_ms)
{
    UmiAiCodingValidationStep step;
    UmiStatus status;

    if (plan == NULL || id == NULL || label == NULL ||
        program == NULL || arguments == NULL ||
        working_directory == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&step, 0, sizeof(step));
    step.kind = kind;
    step.required = required != 0;
    step.enabled = 1;
    step.timeout_ms = timeout_ms;

    status = copy_text(step.id, sizeof(step.id), id);
    if (status == UMI_STATUS_OK)
        status = copy_text(step.label, sizeof(step.label), label);
    if (status == UMI_STATUS_OK)
        status = copy_text(step.executable, sizeof(step.executable), program);
    if (status == UMI_STATUS_OK)
        status = copy_text(step.arguments, sizeof(step.arguments), arguments);
    if (status == UMI_STATUS_OK)
        status = copy_text(
            step.working_directory,
            sizeof(step.working_directory),
            working_directory);
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_validation_plan_add(plan, &step);
}
