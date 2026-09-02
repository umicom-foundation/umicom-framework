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

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *out, size_t capacity, const char *value)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || value == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, value, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise ide location from caller-provided values so later operations receive a known
 * state.
 */
void umi_ide_location_init(UmiIdeLocation *location)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (location == NULL) return;
    (void)memset(location, 0, sizeof(*location));
    location->revision = 1U;
}

/*
 * Provide the ide location set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_location_set_path(
    UmiIdeLocation *location,
    const char *path,
    uint32_t line,
    uint32_t column)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (location == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = copy_text(location->path, sizeof(location->path), path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    location->line = line;
    location->column = column;
    location->has_path = 1;
    location->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ide location set uri operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_location_set_uri(
    UmiIdeLocation *location,
    const char *uri,
    uint32_t line,
    uint32_t column)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (location == NULL || uri == NULL || uri[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = copy_text(location->uri, sizeof(location->uri), uri);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    location->line = line;
    location->column = column;
    location->has_uri = 1;
    location->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ide location has target operation used by this module and its client
 * applications.
 */
int umi_ide_location_has_target(const UmiIdeLocation *location)
{
    return location != NULL && (location->has_path || location->has_uri);
}
