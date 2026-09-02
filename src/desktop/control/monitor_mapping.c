/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/monitor_mapping.c
 * PURPOSE: Implement describe persisted-to-current monitor mapping decisions.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/monitor_mapping.h"

#include <string.h>

/*
 * Initialise desktop monitor mapping from caller-provided values so later operations
 * receive a known state.
 */
void umi_desktop_monitor_mapping_init(UmiDesktopMonitorMappingSnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_MONITOR_MAPPING_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop monitor mapping satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_desktop_monitor_mapping_validate(const UmiDesktopMonitorMappingSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_MONITOR_MAPPING_API_VERSION ||
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
 * Provide the desktop monitor mapping set subject operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_monitor_mapping_set_subject(UmiDesktopMonitorMappingSnapshot *value, const char *subject_id)
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
 * Provide the desktop monitor mapping set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_monitor_mapping_set_detail(UmiDesktopMonitorMappingSnapshot *value, const char *detail)
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
 * Provide the desktop monitor mapping set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_monitor_mapping_set_state(UmiDesktopMonitorMappingSnapshot *value, uint32_t state)
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
 * Provide the desktop monitor mapping set priority operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_monitor_mapping_set_priority(UmiDesktopMonitorMappingSnapshot *value, uint32_t priority)
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
 * Provide the desktop monitor mapping set enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_monitor_mapping_set_enabled(UmiDesktopMonitorMappingSnapshot *value, bool enabled)
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
 * Provide the desktop monitor mapping same identity operation used by this module and its
 * client applications.
 */
bool umi_desktop_monitor_mapping_same_identity(const UmiDesktopMonitorMappingSnapshot *left, const UmiDesktopMonitorMappingSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

/*
 * Provide the desktop monitor mapping confidence operation used by this module and its
 * client applications.
 */
uint32_t umi_desktop_monitor_mapping_confidence(bool exact_identity, bool same_geometry, bool primary_fallback)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (exact_identity) return 100U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (same_geometry) return 75U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (primary_fallback) return 25U;
    return 0U;
}
