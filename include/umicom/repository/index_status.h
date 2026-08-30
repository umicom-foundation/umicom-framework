/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/index_status.h
 *
 * PURPOSE:
 *   Represent staged paths, staged gitlinks and index conflicts.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_REPOSITORY_INDEX_STATUS_H
#define INCLUDE_UMICOM_REPOSITORY_INDEX_STATUS_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRepositoryIndexStatus {
    size_t staged_paths;
    size_t staged_gitlinks;
    size_t conflicted_paths;
} UmiRepositoryIndexStatus;

void umi_repository_index_status_init(UmiRepositoryIndexStatus *status);
int umi_repository_index_status_dirty(const UmiRepositoryIndexStatus *status);
UmiStatus umi_repository_index_status_validate(const UmiRepositoryIndexStatus *status);

#ifdef __cplusplus
}
#endif
#endif
