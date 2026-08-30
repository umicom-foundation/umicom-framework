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
typedef struct UmiVcsBranchList UmiVcsBranchList;
UmiStatus umi_vcs_branch_list_create(UmiVcsBranchList **out_list);
void umi_vcs_branch_list_destroy(UmiVcsBranchList *list);
void umi_vcs_branch_list_clear(UmiVcsBranchList *list);
UmiStatus umi_vcs_branch_list_add(UmiVcsBranchList *list, const UmiVcsBranch *branch);
UmiStatus umi_vcs_branch_list_parse(UmiVcsBranchList *list, const char *output);
size_t umi_vcs_branch_list_count(const UmiVcsBranchList *list);
const UmiVcsBranch *umi_vcs_branch_list_at(const UmiVcsBranchList *list, size_t index);
const UmiVcsBranch *umi_vcs_branch_list_find(const UmiVcsBranchList *list, const char *name);
const UmiVcsBranch *umi_vcs_branch_list_current(const UmiVcsBranchList *list);
#ifdef __cplusplus
}
#endif
#endif
