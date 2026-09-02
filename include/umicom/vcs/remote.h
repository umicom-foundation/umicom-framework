/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/remote.h
 *
 * PURPOSE:
 *   Publish the public remote contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_REMOTE_H
#define UMICOM_VCS_REMOTE_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/vcs/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs remote list data shared with callers of this public contract.
 */
typedef struct UmiVcsRemoteList UmiVcsRemoteList;
/**
 * Initialise vcs remote list from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_remote_list_create(UmiVcsRemoteList **out_list);
/**
 * Release or reset state held by vcs remote list so the same storage can be reused safely.
 */
void umi_vcs_remote_list_destroy(UmiVcsRemoteList *list);
/**
 * Release or reset state held by vcs remote list so the same storage can be reused safely.
 */
void umi_vcs_remote_list_clear(UmiVcsRemoteList *list);
/**
 * Provide the vcs remote list upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_remote_list_upsert(UmiVcsRemoteList *list, const UmiVcsRemote *remote);
/**
 * Read vcs remote list into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_vcs_remote_list_parse(UmiVcsRemoteList *list, const char *output);
/**
 * Return the number of records represented by vcs remote list without changing their
 * state.
 */
size_t umi_vcs_remote_list_count(const UmiVcsRemoteList *list);
/**
 * Find vcs remote list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsRemote *umi_vcs_remote_list_at(const UmiVcsRemoteList *list, size_t index);
/**
 * Find vcs remote list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsRemote *umi_vcs_remote_list_find(const UmiVcsRemoteList *list, const char *name);
#ifdef __cplusplus
}
#endif
#endif
