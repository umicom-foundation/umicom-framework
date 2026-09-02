/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/command_catalogue.h
 *
 * PURPOSE:
 *   Publish stable Database/SQL/modelling workbench commands. Applications own
 *   placement and branding only.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_COMMAND_CATALOGUE_H
#define UMICOM_DATA_WORKBENCH_COMMAND_CATALOGUE_H

#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data workbench command data shared with callers of this public contract.
 */
typedef struct UmiDataWorkbenchCommand {
    uint32_t struct_size;
    uint32_t api_version;
    const char *command_id;
    const char *label;
    const char *category;
    const char *description;
    int mutating;
    int destructive;
    int requires_connection;
    int requires_transaction;
    int requires_selection;
} UmiDataWorkbenchCommand;

/**
 * Return the number of records represented by data workbench command without changing
 * their state.
 */
size_t umi_data_workbench_command_count(void);
/**
 * Find data workbench command while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiDataWorkbenchCommand *umi_data_workbench_command_at(size_t index);
/**
 * Find data workbench command while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiDataWorkbenchCommand *umi_data_workbench_command_find(
    const char *command_id);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_COMMAND_CATALOGUE_H */
