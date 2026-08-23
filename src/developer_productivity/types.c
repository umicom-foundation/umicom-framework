/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/types.c
 *
 * PURPOSE:
 *   Implement stable developer-productivity severity/location primitives.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/types.h"

const char *umi_developer_productivity_severity_text(
    UmiDeveloperProductivitySeverity severity)
{
    switch (severity) {
        case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_HINT: return "hint";
        case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_INFO: return "info";
        case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_WARNING: return "warning";
        case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR: return "error";
        case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_FATAL: return "fatal";
        default: return "unknown";
    }
}

UmiStatus umi_developer_productivity_location_validate(
    const UmiDeveloperProductivityLocation *location)
{
    if (location == NULL || location->uri[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (location->end_line != 0U &&
        location->end_line < location->line) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
