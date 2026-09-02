/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/types.c
 *
 * PURPOSE:
 *   Implement stable developer-productivity severity/location primitives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/types.h"

/*
 * Provide the developer productivity severity text operation used by this module and its
 * client applications.
 */
const char *umi_developer_productivity_severity_text(
    UmiDeveloperProductivitySeverity severity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (severity) {
        case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_HINT: return "hint";
        case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_INFO: return "info";
        case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_WARNING: return "warning";
        case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR: return "error";
        case UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_FATAL: return "fatal";
        default: return "unknown";
    }
}

/*
 * Check that developer productivity location satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_developer_productivity_location_validate(
    const UmiDeveloperProductivityLocation *location)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (location == NULL || location->uri[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (location->end_line != 0U &&
        location->end_line < location->line) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
