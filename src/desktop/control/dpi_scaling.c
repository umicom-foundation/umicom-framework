/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/dpi_scaling.c
 * PURPOSE: Implement store DPI/scale-factor policy used when restoring persisted geometry.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/dpi_scaling.h"

#include <string.h>

void umi_desktop_dpi_scaling_init(UmiDesktopDpiScalingSnapshot *value, const char *id)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_DPI_SCALING_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

UmiStatus umi_desktop_dpi_scaling_validate(const UmiDesktopDpiScalingSnapshot *value)
{
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_DPI_SCALING_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_dpi_scaling_set_subject(UmiDesktopDpiScalingSnapshot *value, const char *subject_id)
{
    UmiStatus status;
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_dpi_scaling_set_detail(UmiDesktopDpiScalingSnapshot *value, const char *detail)
{
    UmiStatus status;
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_dpi_scaling_set_state(UmiDesktopDpiScalingSnapshot *value, uint32_t state)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_dpi_scaling_set_priority(UmiDesktopDpiScalingSnapshot *value, uint32_t priority)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_dpi_scaling_set_enabled(UmiDesktopDpiScalingSnapshot *value, bool enabled)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_desktop_dpi_scaling_same_identity(const UmiDesktopDpiScalingSnapshot *left, const UmiDesktopDpiScalingSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

UmiStatus umi_desktop_dpi_scaling_scale_rect(const UmiDesktopControlRect *input, uint32_t from_scale, uint32_t to_scale, UmiDesktopControlRect *output)
{
    int64_t numerator;
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
