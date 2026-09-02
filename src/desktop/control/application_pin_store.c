/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/application_pin_store.c
 * PURPOSE: Implement persist pin/unpin intent for applications without moving taskbar ownership into an application.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/application_pin_store.h"

#include <string.h>

/*
 * Initialise desktop application pin store from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_application_pin_store_init(UmiDesktopApplicationPinStoreSnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_APPLICATION_PIN_STORE_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop application pin store satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_desktop_application_pin_store_validate(const UmiDesktopApplicationPinStoreSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_APPLICATION_PIN_STORE_API_VERSION ||
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
 * Provide the desktop application pin store set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_application_pin_store_set_subject(UmiDesktopApplicationPinStoreSnapshot *value, const char *subject_id)
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
 * Provide the desktop application pin store set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_application_pin_store_set_detail(UmiDesktopApplicationPinStoreSnapshot *value, const char *detail)
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
 * Provide the desktop application pin store set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_application_pin_store_set_state(UmiDesktopApplicationPinStoreSnapshot *value, uint32_t state)
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
 * Provide the desktop application pin store set priority operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_application_pin_store_set_priority(UmiDesktopApplicationPinStoreSnapshot *value, uint32_t priority)
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
 * Provide the desktop application pin store set enabled operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_application_pin_store_set_enabled(UmiDesktopApplicationPinStoreSnapshot *value, bool enabled)
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
 * Provide the desktop application pin store same identity operation used by this module
 * and its client applications.
 */
bool umi_desktop_application_pin_store_same_identity(const UmiDesktopApplicationPinStoreSnapshot *left, const UmiDesktopApplicationPinStoreSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}
