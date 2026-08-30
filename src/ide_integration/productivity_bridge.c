/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/productivity_bridge.c
 *
 * PURPOSE:
 *   Implement IDE-location projection into Developer Productivity navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/productivity_bridge.h"

#include <string.h>

static UmiStatus copy_uri(
    const UmiIdeLocation *location,
    char *out_uri,
    size_t capacity)
{
    const char *source;
    size_t length;

    source = location->has_uri
        ? location->uri
        : location->path;

    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out_uri, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_ide_location_to_productivity(
    const UmiIdeLocation *location,
    UmiDeveloperProductivityLocation *out_location)
{
    UmiStatus status;

    if (location == NULL || out_location == NULL ||
        !umi_ide_location_has_target(location)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_location, 0, sizeof(*out_location));

    status = copy_uri(
        location,
        out_location->uri,
        sizeof(out_location->uri));
    if (status != UMI_STATUS_OK) return status;

    out_location->line = (size_t)location->line;
    out_location->column = (size_t)location->column;
    out_location->end_line = (size_t)location->end_line;
    out_location->end_column = (size_t)location->end_column;
    return UMI_STATUS_OK;
}

UmiStatus umi_ide_productivity_record_visit(
    UmiDeveloperNavigationService *navigation,
    const UmiIdeLocation *location)
{
    UmiDeveloperProductivityLocation projected;
    UmiStatus status;

    if (navigation == NULL || location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ide_location_to_productivity(
        location,
        &projected);
    if (status != UMI_STATUS_OK) return status;

    return umi_developer_navigation_service_visit(
        navigation,
        &projected);
}
