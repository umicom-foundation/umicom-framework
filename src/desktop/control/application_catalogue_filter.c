/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/application_catalogue_filter.c
 * PURPOSE: Implement store reusable application catalogue filter criteria.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/application_catalogue_filter.h"

#include <string.h>

void umi_desktop_application_catalogue_filter_init(UmiDesktopApplicationCatalogueFilterSnapshot *value, const char *id)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_APPLICATION_CATALOGUE_FILTER_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

UmiStatus umi_desktop_application_catalogue_filter_validate(const UmiDesktopApplicationCatalogueFilterSnapshot *value)
{
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_APPLICATION_CATALOGUE_FILTER_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_application_catalogue_filter_set_subject(UmiDesktopApplicationCatalogueFilterSnapshot *value, const char *subject_id)
{
    UmiStatus status;
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_application_catalogue_filter_set_detail(UmiDesktopApplicationCatalogueFilterSnapshot *value, const char *detail)
{
    UmiStatus status;
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_application_catalogue_filter_set_state(UmiDesktopApplicationCatalogueFilterSnapshot *value, uint32_t state)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_application_catalogue_filter_set_priority(UmiDesktopApplicationCatalogueFilterSnapshot *value, uint32_t priority)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_application_catalogue_filter_set_enabled(UmiDesktopApplicationCatalogueFilterSnapshot *value, bool enabled)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_desktop_application_catalogue_filter_same_identity(const UmiDesktopApplicationCatalogueFilterSnapshot *left, const UmiDesktopApplicationCatalogueFilterSnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

bool umi_desktop_application_catalogue_filter_matches(const char *application_id, const char *display_name, const char *query)
{
    if (query == NULL || query[0] == '\0') return true;
    return umi_desktop_control_text_contains(application_id, query) ||
           umi_desktop_control_text_contains(display_name, query);
}
