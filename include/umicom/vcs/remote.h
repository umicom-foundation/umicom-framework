/* Umicom Framework remote collection. Created by Sammy Hegab, Umicom Foundation. MIT. */
#ifndef UMICOM_VCS_REMOTE_H
#define UMICOM_VCS_REMOTE_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/vcs/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsRemoteList UmiVcsRemoteList;
UmiStatus umi_vcs_remote_list_create(UmiVcsRemoteList **out_list);
void umi_vcs_remote_list_destroy(UmiVcsRemoteList *list);
void umi_vcs_remote_list_clear(UmiVcsRemoteList *list);
UmiStatus umi_vcs_remote_list_upsert(UmiVcsRemoteList *list, const UmiVcsRemote *remote);
UmiStatus umi_vcs_remote_list_parse(UmiVcsRemoteList *list, const char *output);
size_t umi_vcs_remote_list_count(const UmiVcsRemoteList *list);
const UmiVcsRemote *umi_vcs_remote_list_at(const UmiVcsRemoteList *list, size_t index);
const UmiVcsRemote *umi_vcs_remote_list_find(const UmiVcsRemoteList *list, const char *name);
#ifdef __cplusplus
}
#endif
#endif
