/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/types.h
 *
 * PURPOSE:
 *   Define stable bounded contracts shared by the Database, SQL and data-
 *   modelling workbench without exposing database-vendor UI objects.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_TYPES_H
#define UMICOM_DATA_WORKBENCH_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/data/explorer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DATA_WORKBENCH_API_VERSION 1U
#define UMI_DATA_WORKBENCH_MAX_NAVIGATOR_NODES 2048U
#define UMI_DATA_WORKBENCH_MAX_RESULT_ROWS 128U
#define UMI_DATA_WORKBENCH_MAX_RESULT_COLUMNS 64U
#define UMI_DATA_WORKBENCH_CELL_CAPACITY 256U
#define UMI_DATA_WORKBENCH_MAX_BUILDER_ITEMS 64U
#define UMI_DATA_WORKBENCH_MAX_SCHEMA_CHANGES 256U
#define UMI_DATA_WORKBENCH_MAX_RELATIONS 256U
#define UMI_DATA_WORKBENCH_MAX_EDITS 256U

typedef enum UmiDataWorkbenchPhase {
    UMI_DATA_WORKBENCH_IDLE = 0,
    UMI_DATA_WORKBENCH_BROWSING = 1,
    UMI_DATA_WORKBENCH_EDITING_SQL = 2,
    UMI_DATA_WORKBENCH_EXECUTING = 3,
    UMI_DATA_WORKBENCH_SHOWING_RESULTS = 4,
    UMI_DATA_WORKBENCH_MODELLING = 5,
    UMI_DATA_WORKBENCH_COMPARING_SCHEMA = 6,
    UMI_DATA_WORKBENCH_EDITING_DATA = 7,
    UMI_DATA_WORKBENCH_TRANSFERRING = 8,
    UMI_DATA_WORKBENCH_ADMINISTERING = 9,
    UMI_DATA_WORKBENCH_FAILED = 10
} UmiDataWorkbenchPhase;

typedef enum UmiDataNavigatorNodeKind {
    UMI_DATA_NAVIGATOR_CONNECTION = 1,
    UMI_DATA_NAVIGATOR_SCHEMA = 2,
    UMI_DATA_NAVIGATOR_TABLE = 3,
    UMI_DATA_NAVIGATOR_VIEW = 4,
    UMI_DATA_NAVIGATOR_COLUMN = 5
} UmiDataNavigatorNodeKind;

typedef enum UmiDataSchemaChangeKind {
    UMI_DATA_SCHEMA_UNCHANGED = 0,
    UMI_DATA_SCHEMA_ADDED = 1,
    UMI_DATA_SCHEMA_REMOVED = 2,
    UMI_DATA_SCHEMA_CHANGED = 3
} UmiDataSchemaChangeKind;

typedef enum UmiDataEditOperation {
    UMI_DATA_EDIT_INSERT = 1,
    UMI_DATA_EDIT_UPDATE = 2,
    UMI_DATA_EDIT_DELETE = 3
} UmiDataEditOperation;

typedef enum UmiDataEditState {
    UMI_DATA_EDIT_PENDING = 0,
    UMI_DATA_EDIT_VALID = 1,
    UMI_DATA_EDIT_CONFLICTED = 2,
    UMI_DATA_EDIT_APPLIED = 3
} UmiDataEditState;

const char *umi_data_workbench_phase_text(UmiDataWorkbenchPhase phase);
const char *umi_data_schema_change_kind_text(UmiDataSchemaChangeKind kind);
UmiStatus umi_data_workbench_copy_text(
    char *destination,
    size_t capacity,
    const char *source);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_TYPES_H */
