/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/change.h
 *
 * PURPOSE:
 *   Own a bounded collection of working-tree changes for source-control panes and commands.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_CHANGE_H
#define UMICOM_VCS_CHANGE_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/vcs/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsChangeList UmiVcsChangeList;

UmiStatus umi_vcs_change_list_create(UmiVcsChangeList **out_list);
void umi_vcs_change_list_destroy(UmiVcsChangeList *list);
void umi_vcs_change_list_clear(UmiVcsChangeList *list);
UmiStatus umi_vcs_change_list_add(UmiVcsChangeList *list,
                                  const UmiVcsChange *change);
size_t umi_vcs_change_list_count(const UmiVcsChangeList *list);
const UmiVcsChange *umi_vcs_change_list_at(const UmiVcsChangeList *list,
                                           size_t index);
const UmiVcsChange *umi_vcs_change_list_find(const UmiVcsChangeList *list,
                                             const char *path);
size_t umi_vcs_change_list_staged_count(const UmiVcsChangeList *list);

#ifdef __cplusplus
}
#endif

#endif
