/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/panel_filter.c
 * PURPOSE: Implement store application/category filtering policy for the component and panel palette.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/panel_filter.h"

#include <string.h>

void umi_desktop_panel_filter_init(UmiDesktopPanelFilterSnapshot *value, const char *id)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_PANEL_FILTER_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

UmiStatus umi_desktop_panel_filter_validate(const UmiDesktopPanelFilterSnapshot *value)
{
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_PANEL_FILTER_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_panel_filter_set_subject(UmiDesktopPanelFilterSnapshot *value, const char *subject_id)
{
    UmiStatus status;
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_panel_filter_set_detail(UmiDesktopPanelFilterSnapshot *value, const char *detail)
{
    UmiStatus status;
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_panel_filter_set_state(UmiDesktopPanelFilterSnapshot *value, uint32_t state)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_panel_filter_set_priority(UmiDesktopPanelFilterSnapshot *value, uint32_t priority)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_panel_filter_set_enabled(UmiDesktopPanelFilterSnapshot *value, bool enabled)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_desktop_panel_filter_same_identity(const UmiDesktopPanelFilterSnapshot *left, const UmiDesktopPanelFilterSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

bool umi_desktop_panel_filter_matches(const char *application_id, const char *category, const char *query)
{
    if (query == NULL || query[0] == '\0') return true;
    return umi_desktop_control_text_contains(application_id, query) ||
           umi_desktop_control_text_contains(category, query);
}
