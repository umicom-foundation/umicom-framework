/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/application.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/application.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", source);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

void umi_integration_application_init(UmiIntegrationApplication *application)
{
    if (application != NULL) {
        (void)memset(application, 0, sizeof(*application));
    }
}

UmiStatus umi_integration_application_set_identity(
    UmiIntegrationApplication *application,
    const char *id,
    const char *name)
{
    UmiStatus status;
    if (application == NULL || id == NULL || id[0] == '\0' ||
        name == NULL || name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_text(application->id, sizeof(application->id), id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return copy_text(application->name, sizeof(application->name), name);
}

UmiStatus umi_integration_application_add_capability(
    UmiIntegrationApplication *application,
    const char *capability_id)
{
    size_t index;
    UmiStatus status;
    if (application == NULL || capability_id == NULL ||
        capability_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < application->capability_count; ++index) {
        if (strcmp(application->capabilities[index], capability_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (application->capability_count >= UMI_INTEGRATION_MAX_CAPABILITIES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = copy_text(
        application->capabilities[application->capability_count],
        UMI_INTEGRATION_ID_CAPACITY,
        capability_id
    );
    if (status == UMI_STATUS_OK) {
        ++application->capability_count;
    }
    return status;
}

bool umi_integration_application_has_capability(
    const UmiIntegrationApplication *application,
    const char *capability_id)
{
    size_t index;
    if (application == NULL || capability_id == NULL) {
        return false;
    }
    for (index = 0U; index < application->capability_count; ++index) {
        if (strcmp(application->capabilities[index], capability_id) == 0) {
            return true;
        }
    }
    return false;
}
