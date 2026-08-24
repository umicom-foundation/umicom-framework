/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/document_bridge.c
 *
 * PURPOSE:
 *   Implement cross-navigation to the authoritative DocumentCoordinator.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/document_bridge.h"

#include <string.h>

#include "umicom/ide_integration/uri.h"

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

    if (documents == NULL || location == NULL ||
        out_view_id == NULL || view_id_capacity == 0U ||
        out_offset == NULL ||
        !umi_ide_location_has_target(location)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    path[0] = '\0';

    if (location->has_path) {
        if (strlen(location->path) >= sizeof(path)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)strcpy(path, location->path);
    } else {
        status = umi_ide_uri_to_local_path(
            location->uri,
            path,
            sizeof(path));
        if (status != UMI_STATUS_OK) return status;
    }

    status = umi_document_coordinator_active_snapshot(
        documents,
        &active);

    if (status != UMI_STATUS_OK ||
        !active.has_path ||
        strcmp(active.path, path) != 0) {
        status = umi_document_coordinator_open(
            documents,
            path,
            out_view_id,
            view_id_capacity);
        if (status != UMI_STATUS_OK) return status;
    } else {
        if (strlen(active.view_id) >= view_id_capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)strcpy(out_view_id, active.view_id);
    }

    *out_offset = 0U;

    if (location->line > 0U) {
        status = umi_document_coordinator_go_to_line(
            documents,
            (size_t)location->line,
            out_offset);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
