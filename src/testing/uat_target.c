/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/uat_target.c
 *
 * PURPOSE:
 *   Admit target identities and every planned operation before dispatching
 *   a workflow through the existing UAT result and cleanup service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "uat_workflow_internal.h"
#include <string.h>

static const uint32_t knownOperations = UMI_UAT_UI_OBSERVE | UMI_UAT_UI_INTERACT |
    UMI_UAT_WORKFLOW | UMI_UAT_SECURITY_ASSERT | UMI_UAT_DATABASE_READ |
    UMI_UAT_DATABASE_WRITE | UMI_UAT_UNIT;

typedef struct UatTargetExecution {
    UmiUatDriver driver;
    size_t count;
    char ids[UMI_UAT_MAX_STEPS][UMI_TEST_ID_CAPACITY];
    UmiUatOperation operations[UMI_UAT_MAX_STEPS];
} UatTargetExecution;

static int IdentifierLetterOrDigit(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9');
}

static int ValidIdentifier(const char name[UMI_UAT_TARGET_NAME_CAPACITY])
{
    if (!IdentifierLetterOrDigit(name[0])) return 0;
    for (size_t i = 1U; i < UMI_UAT_TARGET_NAME_CAPACITY; ++i) {
        const char ch = name[i];
        if (ch == '\0') return 1;
        if (!IdentifierLetterOrDigit(ch) && ch != '.' && ch != '-' && ch != '_') return 0;
    }
    return 0;
}

static int ValidTarget(const UmiUatTarget *target)
{
    return target->platform >= UMI_UAT_DESKTOP && target->platform <= UMI_UAT_EMBEDDED &&
        ValidIdentifier(target->targetId) && ValidIdentifier(target->sessionId);
}

static int ValidMask(uint32_t mask)
{
    return mask != 0U && (mask & ~knownOperations) == 0U;
}

/* The workflow runner skips rows during failure and cancellation. Use its
 * validated stable case identifier to recover the admitted operation, so
 * cleanup cannot accidentally inherit an operation from a skipped row. */
static UmiStatus ExecuteTarget(void *context, const UmiTestCase *test,
    UmiCancellationToken *cancellation, UmiTestResult *result)
{
    UatTargetExecution *state = context;
    for (size_t i = 0U; i < state->count; ++i) {
        if (strcmp(state->ids[i], test->test_id) == 0)
            return state->driver.execute(state->driver.context, &state->driver.binding,
                state->operations[i], test, cancellation, result);
    }
    return UMI_STATUS_INVALID_STATE;
}

UmiStatus UmiUatRunTarget(const UmiUatTarget *target, uint32_t allowedOperations,
    const UmiUatDriver *driver, const UmiUatStep *steps, const UmiUatOperation *operations,
    size_t count, UmiCancellationToken *cancellation,
    UmiCancellationToken *cleanupCancellation, UmiUatReport **outReport)
{
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = NULL;
    if (target == NULL || driver == NULL || driver->execute == NULL || steps == NULL ||
        operations == NULL || count == 0U || count > UMI_UAT_MAX_STEPS ||
        !ValidMask(allowedOperations) || !ValidMask(driver->supportedOperations) ||
        !ValidTarget(target) || !ValidTarget(&driver->binding))
        return UMI_STATUS_INVALID_ARGUMENT;

    if (target->protocol != UMI_UAT_TARGET_PROTOCOL || driver->binding.protocol != target->protocol)
        return UMI_STATUS_UNAVAILABLE;
    if (target->platform != driver->binding.platform || strcmp(target->targetId, driver->binding.targetId) != 0 ||
        strcmp(target->sessionId, driver->binding.sessionId) != 0)
        return UMI_STATUS_PERMISSION_DENIED;

    /* Validate the entire plan before setup can modify any fixture. Copy the
     * adapter configuration so callbacks cannot redirect later operations. */
    UatTargetExecution state = {.driver = *driver, .count = count};
    for (size_t i = 0U; i < count; ++i) {
        const uint32_t operation = (uint32_t)operations[i];
        if (!ValidMask(operation) || (operation & (operation - 1U)) != 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
        if ((operation & allowedOperations) == 0U) return UMI_STATUS_PERMISSION_DENIED;
        if ((operation & driver->supportedOperations) == 0U) return UMI_STATUS_UNAVAILABLE;
        memcpy(state.ids[i], steps[i].test.test_id, sizeof(state.ids[i]));
        state.operations[i] = operations[i];
    }
    /* The shared workflow owns the admitted identity and typed plan, so every
     * finding can identify its intended operation after the driver returns. */
    const UmiUatTargetEvidence scope = {state.driver.binding, allowedOperations,
        state.driver.supportedOperations};
    return UmiUatRunWithEvidence(steps, count, ExecuteTarget, &state, cancellation,
        cleanupCancellation, &scope, state.operations, outReport);
}
