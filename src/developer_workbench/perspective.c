/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/perspective.c
 *
 * PURPOSE:
 *   Validate reusable developer perspectives and their surface identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/perspective.h"

#include <string.h>

/*
 * Check that developer workbench perspective satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_developer_workbench_perspective_validate(
    const UmiDeveloperWorkbenchPerspectiveDefinition *perspective)
{
    size_t index;
    size_t other;
    int focus_found = 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (perspective == NULL ||
        perspective->structure_size != sizeof(*perspective) ||
        perspective->api_version != UMI_DEVELOPER_WORKBENCH_API_VERSION ||
        perspective->perspective_id == NULL ||
        perspective->perspective_id[0] == '\0' ||
        perspective->title == NULL ||
        perspective->title[0] == '\0' ||
        perspective->description == NULL ||
        perspective->surface_ids == NULL ||
        perspective->surface_count == 0U ||
        perspective->surface_count > UMI_DEVELOPER_WORKBENCH_MAX_SURFACES ||
        perspective->default_focus_surface_id == NULL ||
        perspective->default_focus_surface_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < perspective->surface_count; ++index) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (perspective->surface_ids[index] == NULL ||
            perspective->surface_ids[index][0] == '\0') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(perspective->surface_ids[index],
                   perspective->default_focus_surface_id) == 0) {
            focus_found = 1;
        }

        /* Visit each bounded item once so every record receives the same rule. */
        for (other = index + 1U;
             other < perspective->surface_count;
             ++other) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (strcmp(perspective->surface_ids[index],
                       perspective->surface_ids[other]) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }

    return focus_found ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
