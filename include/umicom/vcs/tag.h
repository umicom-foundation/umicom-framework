/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/tag.h
 *
 * PURPOSE:
 *   Publish the public tag contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_TAG_H
#define UMICOM_VCS_TAG_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/vcs/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs tag list data shared with callers of this public contract.
 */
typedef struct UmiVcsTagList UmiVcsTagList;
/**
 * Initialise vcs tag list from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_vcs_tag_list_create(UmiVcsTagList **out_list);
/**
 * Release or reset state held by vcs tag list so the same storage can be reused safely.
 */
void umi_vcs_tag_list_destroy(UmiVcsTagList *list);
/**
 * Release or reset state held by vcs tag list so the same storage can be reused safely.
 */
void umi_vcs_tag_list_clear(UmiVcsTagList *list);
/**
 * Add vcs tag list only after its inputs and available capacity have been checked.
 */
UmiStatus umi_vcs_tag_list_add(UmiVcsTagList *list, const UmiVcsTag *tag);
/**
 * Read vcs tag list into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_vcs_tag_list_parse(UmiVcsTagList *list, const char *output);
/**
 * Return the number of records represented by vcs tag list without changing their state.
 */
size_t umi_vcs_tag_list_count(const UmiVcsTagList *list);
/**
 * Find vcs tag list while leaving the underlying catalogue or model owned by this module.
 */
const UmiVcsTag *umi_vcs_tag_list_at(const UmiVcsTagList *list, size_t index);
/**
 * Find vcs tag list while leaving the underlying catalogue or model owned by this module.
 */
const UmiVcsTag *umi_vcs_tag_list_find(const UmiVcsTagList *list, const char *name);
#ifdef __cplusplus
}
#endif
#endif
