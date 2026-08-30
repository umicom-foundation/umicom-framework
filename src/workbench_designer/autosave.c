/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/autosave.c
 *
 * PURPOSE:
 *   Implement autosave scheduling state separately from frontend event-loop
 *   timers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/autosave.h"
#include <string.h>


UmiWorkbenchDesignerAutosavePolicy umi_workbench_designer_autosave_policy_default(void)
{
    UmiWorkbenchDesignerAutosavePolicy policy;
    policy.enabled = true;
    policy.debounce_ms = 1500U;
    policy.maximum_delay_ms = 15000U;
    policy.retry_delay_ms = 5000U;
    return policy;
}

void umi_workbench_designer_autosave_init(
    UmiWorkbenchDesignerAutosave *autosave,
    const UmiWorkbenchDesignerAutosavePolicy *policy)
{
    if (autosave == NULL) return;
    (void)memset(autosave, 0, sizeof(*autosave));
    autosave->policy = policy != NULL
        ? *policy : umi_workbench_designer_autosave_policy_default();
}

void umi_workbench_designer_autosave_mark_dirty(
    UmiWorkbenchDesignerAutosave *autosave,
    uint64_t timestamp_ms)
{
    uint64_t debounce_due;
    uint64_t maximum_due;
    if (autosave == NULL) return;
    if (!autosave->dirty) autosave->dirty_since_ms = timestamp_ms;
    autosave->dirty = true;
    autosave->last_change_ms = timestamp_ms;
    debounce_due = timestamp_ms + autosave->policy.debounce_ms;
    maximum_due = autosave->dirty_since_ms + autosave->policy.maximum_delay_ms;
    autosave->next_due_ms = debounce_due < maximum_due ? debounce_due : maximum_due;
    autosave->revision += 1U;
}

bool umi_workbench_designer_autosave_due(
    const UmiWorkbenchDesignerAutosave *autosave,
    uint64_t now_ms)
{
    return autosave != NULL && autosave->policy.enabled && autosave->dirty &&
        !autosave->in_flight && now_ms >= autosave->next_due_ms;
}

UmiStatus umi_workbench_designer_autosave_begin(
    UmiWorkbenchDesignerAutosave *autosave,
    uint64_t now_ms)
{
    if (autosave == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_workbench_designer_autosave_due(autosave, now_ms)) {
        return UMI_STATUS_INVALID_STATE;
    }
    autosave->in_flight = true;
    autosave->last_attempt_ms = now_ms;
    autosave->revision += 1U;
    return UMI_STATUS_OK;
}

void umi_workbench_designer_autosave_complete(
    UmiWorkbenchDesignerAutosave *autosave,
    UmiStatus status,
    uint64_t now_ms)
{
    if (autosave == NULL) return;
    autosave->in_flight = false;
    if (status == UMI_STATUS_OK) {
        autosave->dirty = false;
        autosave->last_success_ms = now_ms;
        autosave->consecutive_failures = 0U;
        autosave->next_due_ms = 0U;
    } else {
        autosave->consecutive_failures += 1U;
        autosave->next_due_ms = now_ms + autosave->policy.retry_delay_ms;
    }
    autosave->revision += 1U;
}
