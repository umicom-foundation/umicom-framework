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

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || capacity == 0U || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Add ai coding validation profile only after its inputs and available capacity have been
 * checked.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(step.label, sizeof(step.label), label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(step.executable, sizeof(step.executable), program);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(step.arguments, sizeof(step.arguments), arguments);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(
            step.working_directory,
            sizeof(step.working_directory),
            working_directory);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_coding_validation_plan_add(plan, &step);
}
