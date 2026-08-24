/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/head_set.h
 *
 * PURPOSE:
 *   Track resolved checked-out submodule HEAD identifiers.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_HEAD_SET_H
#define UMICOM_REPOSITORY_HEAD_SET_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRepositoryHead {
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    char sha[UMI_REPOSITORY_CONTROL_SHA_CAPACITY];
} UmiRepositoryHead;
typedef struct UmiRepositoryHeadSet {
    UmiRepositoryHead items[UMI_REPOSITORY_CONTROL_ITEM_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiRepositoryHeadSet;
void umi_repository_head_set_init(UmiRepositoryHeadSet *set);
UmiStatus umi_repository_head_set_upsert(
    UmiRepositoryHeadSet *set, const char *path, const char *sha);
const UmiRepositoryHead *umi_repository_head_set_find(
    const UmiRepositoryHeadSet *set, const char *path);
#ifdef __cplusplus
}
#endif
#endif
