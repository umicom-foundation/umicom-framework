/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/head_set.c
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
#include "umicom/repository/head_set.h"

#include <stdio.h>
#include <string.h>

#include "umicom/repository/path.h"
#include "umicom/repository/sha.h"

/*
 * Initialise repository head set from caller-provided values so later operations receive a
 * known state.
 */
void umi_repository_head_set_init(UmiRepositoryHeadSet *set)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return;
    (void)memset(set, 0, sizeof(*set));
    set->revision = 1U;
}

/*
 * Find repository head set while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiRepositoryHead *umi_repository_head_set_find(
    const UmiRepositoryHeadSet *set, const char *path)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || path == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < set->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(set->items[index].path, path) == 0) {
            return &set->items[index];
        }
    }
    return NULL;
}

/*
 * Provide the repository head set upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_head_set_upsert(
    UmiRepositoryHeadSet *set, const char *path, const char *sha)
{
    size_t index;
    char normalized_path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    char normalized_sha[UMI_REPOSITORY_CONTROL_SHA_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_repository_control_path_normalize(
        path, normalized_path, sizeof(normalized_path));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_repository_sha_normalize(
        sha, normalized_sha, sizeof(normalized_sha));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < set->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(set->items[index].path, normalized_path) == 0) {
            (void)snprintf(
                set->items[index].sha,
                sizeof(set->items[index].sha),
                "%s",
                normalized_sha);
            set->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (set->count >= UMI_REPOSITORY_CONTROL_ITEM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(
        set->items[set->count].path,
        sizeof(set->items[set->count].path),
        "%s",
        normalized_path);
    (void)snprintf(
        set->items[set->count].sha,
        sizeof(set->items[set->count].sha),
        "%s",
        normalized_sha);
    set->count += 1U;
    set->revision += 1U;
    return UMI_STATUS_OK;
}
