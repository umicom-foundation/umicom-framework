/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/conflict.h
 *
 * PURPOSE:
 *   Publish the public conflict contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_CONFLICT_H
#define UMICOM_VCS_CONFLICT_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/vcs/change.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs conflict list data shared with callers of this public contract.
 */
typedef struct UmiVcsConflictList UmiVcsConflictList;
/**
 * Initialise vcs conflict list from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_conflict_list_create(UmiVcsConflictList **out_list);
/**
 * Release or reset state held by vcs conflict list so the same storage can be reused
 * safely.
 */
void umi_vcs_conflict_list_destroy(UmiVcsConflictList *list);
/**
 * Release or reset state held by vcs conflict list so the same storage can be reused
 * safely.
 */
void umi_vcs_conflict_list_clear(UmiVcsConflictList *list);
/**
 * Provide the vcs conflict list build operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_conflict_list_build(UmiVcsConflictList *list, const UmiVcsChangeList *changes);
/**
 * Return the number of records represented by vcs conflict list without changing their
 * state.
 */
size_t umi_vcs_conflict_list_count(const UmiVcsConflictList *list);
/**
 * Find vcs conflict list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsConflict *umi_vcs_conflict_list_at(const UmiVcsConflictList *list, size_t index);
/**
 * Find vcs conflict list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsConflict *umi_vcs_conflict_list_find(const UmiVcsConflictList *list, const char *path);
/**
 * Provide the vcs conflict kind from code operation used by this module and its client
 * applications.
 */
UmiVcsConflictKind umi_vcs_conflict_kind_from_code(const char code[3]);
#ifdef __cplusplus
}
#endif
#endif
