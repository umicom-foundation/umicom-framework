/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/document_bridge.c
 *
 * PURPOSE:
 *   Implement cross-navigation to the authoritative DocumentCoordinator.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/document_bridge.h"

#include <string.h>

#include "umicom/ide_integration/uri.h"

/*
 * Provide the ide document open location operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_document_open_location(
    UmiDocumentCoordinator *documents,
    const UmiIdeLocation *location,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset)
{
    UmiDocumentWorkingCopySnapshot active;
    char path[UMI_IDE_INTEGRATION_PATH_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (documents == NULL || location == NULL ||
        out_view_id == NULL || view_id_capacity == 0U ||
        out_offset == NULL ||
        !umi_ide_location_has_target(location)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    path[0] = '\0';

    /* Apply this branch only when its contract condition is satisfied. */
    if (location->has_path) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strlen(location->path) >= sizeof(path)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)strcpy(path, location->path);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = umi_ide_uri_to_local_path(
            location->uri,
            path,
            sizeof(path));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    status = umi_document_coordinator_active_snapshot(
        documents,
        &active);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK ||
        !active.has_path ||
        strcmp(active.path, path) != 0) {
        status = umi_document_coordinator_open(
            documents,
            path,
            out_view_id,
            view_id_capacity);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strlen(active.view_id) >= view_id_capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)strcpy(out_view_id, active.view_id);
    }

    *out_offset = 0U;

    /* Apply this branch only when its contract condition is satisfied. */
    if (location->line > 0U) {
        status = umi_document_coordinator_go_to_line(
            documents,
            (size_t)location->line,
            out_offset);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
