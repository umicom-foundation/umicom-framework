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
typedef struct UmiVcsConflictList UmiVcsConflictList;
UmiStatus umi_vcs_conflict_list_create(UmiVcsConflictList **out_list);
void umi_vcs_conflict_list_destroy(UmiVcsConflictList *list);
void umi_vcs_conflict_list_clear(UmiVcsConflictList *list);
UmiStatus umi_vcs_conflict_list_build(UmiVcsConflictList *list, const UmiVcsChangeList *changes);
size_t umi_vcs_conflict_list_count(const UmiVcsConflictList *list);
const UmiVcsConflict *umi_vcs_conflict_list_at(const UmiVcsConflictList *list, size_t index);
const UmiVcsConflict *umi_vcs_conflict_list_find(const UmiVcsConflictList *list, const char *path);
UmiVcsConflictKind umi_vcs_conflict_kind_from_code(const char code[3]);
#ifdef __cplusplus
}
#endif
#endif
