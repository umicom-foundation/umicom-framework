/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/dpi_scaling.c
 * PURPOSE: Implement store DPI/scale-factor policy used when restoring persisted geometry.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/dpi_scaling.h"

#include <string.h>

/*
 * Initialise desktop dpi scaling from caller-provided values so later operations receive a
 * known state.
 */
void umi_desktop_dpi_scaling_init(UmiDesktopDpiScalingSnapshot *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_DPI_SCALING_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that desktop dpi scaling satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_desktop_dpi_scaling_validate(const UmiDesktopDpiScalingSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_DPI_SCALING_API_VERSION ||
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
 * Provide the desktop dpi scaling set subject operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_dpi_scaling_set_subject(UmiDesktopDpiScalingSnapshot *value, const char *subject_id)
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
 * Provide the desktop dpi scaling set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_dpi_scaling_set_detail(UmiDesktopDpiScalingSnapshot *value, const char *detail)
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
 * Provide the desktop dpi scaling set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_dpi_scaling_set_state(UmiDesktopDpiScalingSnapshot *value, uint32_t state)
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
 * Provide the desktop dpi scaling set priority operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_dpi_scaling_set_priority(UmiDesktopDpiScalingSnapshot *value, uint32_t priority)
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
 * Provide the desktop dpi scaling set enabled operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_dpi_scaling_set_enabled(UmiDesktopDpiScalingSnapshot *value, bool enabled)
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
 * Provide the desktop dpi scaling same identity operation used by this module and its
 * client applications.
 */
bool umi_desktop_dpi_scaling_same_identity(const UmiDesktopDpiScalingSnapshot *left, const UmiDesktopDpiScalingSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

/*
 * Provide the desktop dpi scaling scale rect operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_dpi_scaling_scale_rect(const UmiDesktopControlRect *input, uint32_t from_scale, uint32_t to_scale, UmiDesktopControlRect *output)
{
    int64_t numerator;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || output == NULL || from_scale == 0U || to_scale == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    *output = *input;
#define UMI_SCALE_MEMBER(member) do { \
    numerator = (int64_t)input->member * (int64_t)to_scale; \
    output->member = (int32_t)(numerator / (int64_t)from_scale); \
} while (0)
    UMI_SCALE_MEMBER(x); UMI_SCALE_MEMBER(y); UMI_SCALE_MEMBER(width); UMI_SCALE_MEMBER(height);
#undef UMI_SCALE_MEMBER
    return umi_desktop_control_rect_valid(output) ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
