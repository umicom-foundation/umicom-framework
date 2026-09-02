/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/command_history.c
 * PURPOSE: Implement capture command invocation history metadata for ranking and audit.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/command_history.h"

#include <string.h>

/*
 * Initialise desktop command history from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_command_history_init(UmiDesktopCommandHistorySnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_COMMAND_HISTORY_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop command history satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_desktop_command_history_validate(const UmiDesktopCommandHistorySnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_COMMAND_HISTORY_API_VERSION ||
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
 * Provide the desktop command history set subject operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_command_history_set_subject(UmiDesktopCommandHistorySnapshot *value, const char *subject_id)
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
 * Provide the desktop command history set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_command_history_set_detail(UmiDesktopCommandHistorySnapshot *value, const char *detail)
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
 * Provide the desktop command history set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_command_history_set_state(UmiDesktopCommandHistorySnapshot *value, uint32_t state)
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
 * Provide the desktop command history set priority operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_command_history_set_priority(UmiDesktopCommandHistorySnapshot *value, uint32_t priority)
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
 * Provide the desktop command history set enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_command_history_set_enabled(UmiDesktopCommandHistorySnapshot *value, bool enabled)
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
 * Provide the desktop command history same identity operation used by this module and its
 * client applications.
 */
bool umi_desktop_command_history_same_identity(const UmiDesktopCommandHistorySnapshot *left, const UmiDesktopCommandHistorySnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}
