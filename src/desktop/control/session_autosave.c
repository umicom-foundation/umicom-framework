/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/session_autosave.c
 * PURPOSE: Implement store crash-safe session autosave cadence and pending state.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/session_autosave.h"

#include <string.h>

/*
 * Initialise desktop session autosave from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_session_autosave_init(UmiDesktopSessionAutosaveSnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_SESSION_AUTOSAVE_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop session autosave satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_desktop_session_autosave_validate(const UmiDesktopSessionAutosaveSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_SESSION_AUTOSAVE_API_VERSION ||
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
 * Provide the desktop session autosave set subject operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_session_autosave_set_subject(UmiDesktopSessionAutosaveSnapshot *value, const char *subject_id)
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
 * Provide the desktop session autosave set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_session_autosave_set_detail(UmiDesktopSessionAutosaveSnapshot *value, const char *detail)
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
 * Provide the desktop session autosave set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_session_autosave_set_state(UmiDesktopSessionAutosaveSnapshot *value, uint32_t state)
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
 * Provide the desktop session autosave set priority operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_session_autosave_set_priority(UmiDesktopSessionAutosaveSnapshot *value, uint32_t priority)
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
 * Provide the desktop session autosave set enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_session_autosave_set_enabled(UmiDesktopSessionAutosaveSnapshot *value, bool enabled)
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
 * Provide the desktop session autosave same identity operation used by this module and its
 * client applications.
 */
bool umi_desktop_session_autosave_same_identity(const UmiDesktopSessionAutosaveSnapshot *left, const UmiDesktopSessionAutosaveSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

/*
 * Provide the desktop session autosave due operation used by this module and its client
 * applications.
 */
bool umi_desktop_session_autosave_due(uint64_t now_ms, uint64_t last_save_ms, uint64_t interval_ms, bool dirty)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (!dirty || interval_ms == 0U || now_ms < last_save_ms) return false;
    return now_ms - last_save_ms >= interval_ms;
}
