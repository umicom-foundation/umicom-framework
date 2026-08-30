/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/application_availability.c
 * PURPOSE: Implement represent whether an application can be launched and explain unavailable providers.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/application_availability.h"

#include <string.h>

void umi_desktop_application_availability_init(UmiDesktopApplicationAvailabilitySnapshot *value, const char *id)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_DESKTOP_APPLICATION_AVAILABILITY_API_VERSION;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_desktop_control_copy_text(value->id, sizeof(value->id), id);
}

UmiStatus umi_desktop_application_availability_validate(const UmiDesktopApplicationAvailabilitySnapshot *value)
{
    if (value == NULL || value->struct_size != (uint32_t)sizeof(*value) ||
        value->api_version != UMI_DESKTOP_APPLICATION_AVAILABILITY_API_VERSION ||
        !umi_desktop_control_id_valid(value->id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (value->subject_id[0] != '\0' && !umi_desktop_control_id_valid(value->subject_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_application_availability_set_subject(UmiDesktopApplicationAvailabilitySnapshot *value, const char *subject_id)
{
    UmiStatus status;
    if (value == NULL || subject_id == NULL || !umi_desktop_control_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->subject_id, sizeof(value->subject_id), subject_id);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_application_availability_set_detail(UmiDesktopApplicationAvailabilitySnapshot *value, const char *detail)
{
    UmiStatus status;
    if (value == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_control_copy_text(value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_desktop_application_availability_set_state(UmiDesktopApplicationAvailabilitySnapshot *value, uint32_t state)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_application_availability_set_priority(UmiDesktopApplicationAvailabilitySnapshot *value, uint32_t priority)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->priority = priority;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_application_availability_set_enabled(UmiDesktopApplicationAvailabilitySnapshot *value, bool enabled)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->enabled = enabled;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_desktop_application_availability_same_identity(const UmiDesktopApplicationAvailabilitySnapshot *left, const UmiDesktopApplicationAvailabilitySnapshot *right)
{
    return left != NULL && right != NULL && strcmp(left->id, right->id) == 0;
}

bool umi_desktop_application_availability_launchable(UmiDesktopControlAvailability availability)
{
    return availability == UMI_DESKTOP_CONTROL_AVAILABILITY_AVAILABLE ||
           availability == UMI_DESKTOP_CONTROL_AVAILABILITY_DEGRADED;
}
