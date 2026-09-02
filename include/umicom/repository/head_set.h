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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_HEAD_SET_H
#define UMICOM_REPOSITORY_HEAD_SET_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the repository head data shared with callers of this public contract.
 */
typedef struct UmiRepositoryHead {
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    char sha[UMI_REPOSITORY_CONTROL_SHA_CAPACITY];
} UmiRepositoryHead;
/**
 * Represent the repository head set data shared with callers of this public contract.
 */
typedef struct UmiRepositoryHeadSet {
    UmiRepositoryHead items[UMI_REPOSITORY_CONTROL_ITEM_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiRepositoryHeadSet;
/**
 * Initialise repository head set from caller-provided values so later operations receive a
 * known state.
 */
void umi_repository_head_set_init(UmiRepositoryHeadSet *set);
/**
 * Provide the repository head set upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_head_set_upsert(
    UmiRepositoryHeadSet *set, const char *path, const char *sha);
/**
 * Find repository head set while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiRepositoryHead *umi_repository_head_set_find(
    const UmiRepositoryHeadSet *set, const char *path);
#ifdef __cplusplus
}
#endif
#endif
