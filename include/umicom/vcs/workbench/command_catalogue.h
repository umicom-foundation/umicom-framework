/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workbench/command_catalogue.h
 *
 * PURPOSE:
 *   Publish stable commands for compare, merge, review and partial-stage views.
 *   Applications contribute placement and key bindings only.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_WORKBENCH_COMMAND_CATALOGUE_H
#define UMICOM_VCS_WORKBENCH_COMMAND_CATALOGUE_H

#include "umicom/vcs/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs workbench command data shared with callers of this public contract.
 */
typedef struct UmiVcsWorkbenchCommand {
    uint32_t struct_size;
    uint32_t api_version;
    const char *command_id;
    const char *label;
    const char *category;
    const char *description;
    int mutating;
    int destructive;
    int requires_compare;
    int requires_merge;
    int requires_selection;
} UmiVcsWorkbenchCommand;

/**
 * Return the number of records represented by vcs workbench command without changing their
 * state.
 */
size_t umi_vcs_workbench_command_count(void);
/**
 * Find vcs workbench command while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsWorkbenchCommand *umi_vcs_workbench_command_at(size_t index);
/**
 * Find vcs workbench command while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsWorkbenchCommand *umi_vcs_workbench_command_find(
    const char *command_id);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_COMMAND_CATALOGUE_H */
