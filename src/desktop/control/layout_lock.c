/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/layout_lock.c
 * PURPOSE: Implement represent layout lock ownership and mutation policy.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/layout_lock.h"

#include <string.h>

/*
 * Initialise desktop layout lock from caller-provided values so later operations receive a
 * known state.
 */
void umi_desktop_layout_lock_init(UmiDesktopLayoutLockSnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_LAYOUT_LOCK_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop layout lock satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_desktop_layout_lock_validate(const UmiDesktopLayoutLockSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_LAYOUT_LOCK_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop layout lock set subject operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_lock_set_subject(UmiDesktopLayoutLockSnapshot *value, const char *subject_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

/*
 * Provide the desktop layout lock set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_lock_set_detail(UmiDesktopLayoutLockSnapshot *value, const char *detail)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

/*
 * Provide the desktop layout lock set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_lock_set_state(UmiDesktopLayoutLockSnapshot *value, uint32_t state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop layout lock set priority operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_layout_lock_set_priority(UmiDesktopLayoutLockSnapshot *value, uint32_t priority)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop layout lock set enabled operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_layout_lock_set_enabled(UmiDesktopLayoutLockSnapshot *value, bool enabled)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop layout lock same identity operation used by this module and its
 * client applications.
 */
bool umi_desktop_layout_lock_same_identity(const UmiDesktopLayoutLockSnapshot *left, const UmiDesktopLayoutLockSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

/*
 * Provide the desktop layout lock mutation allowed operation used by this module and its
 * client applications.
 */
bool umi_desktop_layout_lock_mutation_allowed(bool locked, const char *lock_owner, const char *request_owner)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (!locked) return true;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lock_owner == NULL || request_owner == NULL) return false;
    return strcmp(lock_owner, request_owner) == 0;
}
