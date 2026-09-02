/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/shortcut_binding.c
 * PURPOSE: Implement describe a global semantic shortcut binding without toolkit-specific accelerators.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/shortcut_binding.h"

#include <string.h>

/*
 * Initialise desktop shortcut binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_shortcut_binding_init(UmiDesktopShortcutBindingSnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_SHORTCUT_BINDING_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop shortcut binding satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_desktop_shortcut_binding_validate(const UmiDesktopShortcutBindingSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_SHORTCUT_BINDING_API_VERSION ||
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
 * Provide the desktop shortcut binding set subject operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_shortcut_binding_set_subject(UmiDesktopShortcutBindingSnapshot *value, const char *subject_id)
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
 * Provide the desktop shortcut binding set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_shortcut_binding_set_detail(UmiDesktopShortcutBindingSnapshot *value, const char *detail)
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
 * Provide the desktop shortcut binding set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_shortcut_binding_set_state(UmiDesktopShortcutBindingSnapshot *value, uint32_t state)
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
 * Provide the desktop shortcut binding set priority operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_shortcut_binding_set_priority(UmiDesktopShortcutBindingSnapshot *value, uint32_t priority)
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
 * Provide the desktop shortcut binding set enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_shortcut_binding_set_enabled(UmiDesktopShortcutBindingSnapshot *value, bool enabled)
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
 * Provide the desktop shortcut binding same identity operation used by this module and its
 * client applications.
 */
bool umi_desktop_shortcut_binding_same_identity(const UmiDesktopShortcutBindingSnapshot *left, const UmiDesktopShortcutBindingSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}
