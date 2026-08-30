/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/location.c
 *
 * PURPOSE:
 *   Implement bounded IDE location construction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/location.h"

#include <string.h>

static UmiStatus copy_text(char *out, size_t capacity, const char *value)
{
    size_t length;

    if (out == NULL || value == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(value);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, value, length + 1U);
    return UMI_STATUS_OK;
}

void umi_ide_location_init(UmiIdeLocation *location)
{
    if (location == NULL) return;
    (void)memset(location, 0, sizeof(*location));
    location->revision = 1U;
}

UmiStatus umi_ide_location_set_path(
    UmiIdeLocation *location,
    const char *path,
    uint32_t line,
    uint32_t column)
{
    UmiStatus status;

    if (location == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = copy_text(location->path, sizeof(location->path), path);
    if (status != UMI_STATUS_OK) return status;

    location->line = line;
    location->column = column;
    location->has_path = 1;
    location->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ide_location_set_uri(
    UmiIdeLocation *location,
    const char *uri,
    uint32_t line,
    uint32_t column)
{
    UmiStatus status;

    if (location == NULL || uri == NULL || uri[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = copy_text(location->uri, sizeof(location->uri), uri);
    if (status != UMI_STATUS_OK) return status;

    location->line = line;
    location->column = column;
    location->has_uri = 1;
    location->revision += 1U;
    return UMI_STATUS_OK;
}

int umi_ide_location_has_target(const UmiIdeLocation *location)
{
    return location != NULL && (location->has_path || location->has_uri);
}
