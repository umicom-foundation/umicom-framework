/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/types.c
 *
 * PURPOSE:
 *   Implement shared Database workbench text and lifecycle utilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/types.h"

#include <string.h>

/*
 * Provide the data workbench phase text operation used by this module and its client
 * applications.
 */
const char *umi_data_workbench_phase_text(UmiDataWorkbenchPhase phase)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the data schema change kind text operation used by this module and its client
 * applications.
 */
const char *umi_data_schema_change_kind_text(UmiDataSchemaChangeKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_DATA_SCHEMA_UNCHANGED: return "unchanged";
        case UMI_DATA_SCHEMA_ADDED: return "added";
        case UMI_DATA_SCHEMA_REMOVED: return "removed";
        case UMI_DATA_SCHEMA_CHANGED: return "changed";
        default: return "unknown";
    }
}

/*
 * Provide the data workbench copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_workbench_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}
