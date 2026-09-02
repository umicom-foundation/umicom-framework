/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/branch_list.h
 *
 * PURPOSE:
 *   Publish the public branch list contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_BRANCH_LIST_H
#define UMICOM_VCS_BRANCH_LIST_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/vcs/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs branch list data shared with callers of this public contract.
 */
typedef struct UmiVcsBranchList UmiVcsBranchList;
/**
 * Initialise vcs branch list from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_branch_list_create(UmiVcsBranchList **out_list);
/**
 * Release or reset state held by vcs branch list so the same storage can be reused safely.
 */
void umi_vcs_branch_list_destroy(UmiVcsBranchList *list);
/**
 * Release or reset state held by vcs branch list so the same storage can be reused safely.
 */
void umi_vcs_branch_list_clear(UmiVcsBranchList *list);
/**
 * Add vcs branch list only after its inputs and available capacity have been checked.
 */
UmiStatus umi_vcs_branch_list_add(UmiVcsBranchList *list, const UmiVcsBranch *branch);
/**
 * Read vcs branch list into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_vcs_branch_list_parse(UmiVcsBranchList *list, const char *output);
/**
 * Return the number of records represented by vcs branch list without changing their
 * state.
 */
size_t umi_vcs_branch_list_count(const UmiVcsBranchList *list);
/**
 * Find vcs branch list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsBranch *umi_vcs_branch_list_at(const UmiVcsBranchList *list, size_t index);
/**
 * Find vcs branch list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsBranch *umi_vcs_branch_list_find(const UmiVcsBranchList *list, const char *name);
/**
 * Provide the vcs branch list current operation used by this module and its client
 * applications.
 */
const UmiVcsBranch *umi_vcs_branch_list_current(const UmiVcsBranchList *list);
#ifdef __cplusplus
}
#endif
#endif
