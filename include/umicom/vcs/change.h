/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/change.h
 *
 * PURPOSE:
 *   Own a bounded collection of working-tree changes for source-control panes and commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_CHANGE_H
#define UMICOM_VCS_CHANGE_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/vcs/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs change list data shared with callers of this public contract.
 */
typedef struct UmiVcsChangeList UmiVcsChangeList;

/**
 * Initialise vcs change list from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_change_list_create(UmiVcsChangeList **out_list);
/**
 * Release or reset state held by vcs change list so the same storage can be reused safely.
 */
void umi_vcs_change_list_destroy(UmiVcsChangeList *list);
/**
 * Release or reset state held by vcs change list so the same storage can be reused safely.
 */
void umi_vcs_change_list_clear(UmiVcsChangeList *list);
/**
 * Add vcs change list only after its inputs and available capacity have been checked.
 */
UmiStatus umi_vcs_change_list_add(UmiVcsChangeList *list,
                                  const UmiVcsChange *change);
/**
 * Return the number of records represented by vcs change list without changing their
 * state.
 */
size_t umi_vcs_change_list_count(const UmiVcsChangeList *list);
/**
 * Find vcs change list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsChange *umi_vcs_change_list_at(const UmiVcsChangeList *list,
                                           size_t index);
/**
 * Find vcs change list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsChange *umi_vcs_change_list_find(const UmiVcsChangeList *list,
                                             const char *path);
/**
 * Return the number of records represented by vcs change list staged without changing
 * their state.
 */
size_t umi_vcs_change_list_staged_count(const UmiVcsChangeList *list);

#ifdef __cplusplus
}
#endif

#endif
