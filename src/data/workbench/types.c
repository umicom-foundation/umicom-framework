/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/types.c
 *
 * PURPOSE:
 *   Implement shared Database workbench text and lifecycle utilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/types.h"

#include <string.h>

const char *umi_data_workbench_phase_text(UmiDataWorkbenchPhase phase)
{
    switch (phase) {
        case UMI_DATA_WORKBENCH_IDLE: return "idle";
        case UMI_DATA_WORKBENCH_BROWSING: return "browsing";
        case UMI_DATA_WORKBENCH_EDITING_SQL: return "editing-sql";
        case UMI_DATA_WORKBENCH_EXECUTING: return "executing";
        case UMI_DATA_WORKBENCH_SHOWING_RESULTS: return "showing-results";
        case UMI_DATA_WORKBENCH_MODELLING: return "modelling";
        case UMI_DATA_WORKBENCH_COMPARING_SCHEMA: return "comparing-schema";
        case UMI_DATA_WORKBENCH_EDITING_DATA: return "editing-data";
        case UMI_DATA_WORKBENCH_TRANSFERRING: return "transferring";
        case UMI_DATA_WORKBENCH_ADMINISTERING: return "administering";
        case UMI_DATA_WORKBENCH_FAILED: return "failed";
        default: return "unknown";
    }
}

const char *umi_data_schema_change_kind_text(UmiDataSchemaChangeKind kind)
{
    switch (kind) {
        case UMI_DATA_SCHEMA_UNCHANGED: return "unchanged";
        case UMI_DATA_SCHEMA_ADDED: return "added";
        case UMI_DATA_SCHEMA_REMOVED: return "removed";
        case UMI_DATA_SCHEMA_CHANGED: return "changed";
        default: return "unknown";
    }
}

UmiStatus umi_data_workbench_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}
