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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/head_set.h"

#include <stdio.h>
#include <string.h>

#include "umicom/repository/path.h"
#include "umicom/repository/sha.h"

void umi_repository_head_set_init(UmiRepositoryHeadSet *set)
{
    if (set == NULL) return;
    (void)memset(set, 0, sizeof(*set));
    set->revision = 1U;
}

const UmiRepositoryHead *umi_repository_head_set_find(
    const UmiRepositoryHeadSet *set, const char *path)
{
    size_t index;
    if (set == NULL || path == NULL) return NULL;
    for (index = 0U; index < set->count; ++index) {
        if (strcmp(set->items[index].path, path) == 0) {
            return &set->items[index];
        }
    }
    return NULL;
}

UmiStatus umi_repository_head_set_upsert(
    UmiRepositoryHeadSet *set, const char *path, const char *sha)
{
    size_t index;
    char normalized_path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    char normalized_sha[UMI_REPOSITORY_CONTROL_SHA_CAPACITY];
    UmiStatus status;
    if (set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_repository_control_path_normalize(
        path, normalized_path, sizeof(normalized_path));
    if (status != UMI_STATUS_OK) return status;
    status = umi_repository_sha_normalize(
        sha, normalized_sha, sizeof(normalized_sha));
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < set->count; ++index) {
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
