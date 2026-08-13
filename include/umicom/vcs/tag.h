/* Umicom Framework tag collection. Created by Sammy Hegab, Umicom Foundation. MIT. */
#ifndef UMICOM_VCS_TAG_H
#define UMICOM_VCS_TAG_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/vcs/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsTagList UmiVcsTagList;
UmiStatus umi_vcs_tag_list_create(UmiVcsTagList **out_list);
void umi_vcs_tag_list_destroy(UmiVcsTagList *list);
void umi_vcs_tag_list_clear(UmiVcsTagList *list);
UmiStatus umi_vcs_tag_list_add(UmiVcsTagList *list, const UmiVcsTag *tag);
UmiStatus umi_vcs_tag_list_parse(UmiVcsTagList *list, const char *output);
size_t umi_vcs_tag_list_count(const UmiVcsTagList *list);
const UmiVcsTag *umi_vcs_tag_list_at(const UmiVcsTagList *list, size_t index);
const UmiVcsTag *umi_vcs_tag_list_find(const UmiVcsTagList *list, const char *name);
#ifdef __cplusplus
}
#endif
#endif
