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
#include "umicom/base/value_archive.h"
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the repository index status data shared with callers of this public contract.
 */
typedef struct UmiRepositoryIndexStatus {
    size_t staged_paths;
    size_t staged_gitlinks;
    size_t conflicted_paths;
} UmiRepositoryIndexStatus;

/**
 * Initialise repository index status from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_index_status_init(UmiRepositoryIndexStatus *status);
/**
 * Provide the repository index status dirty operation used by this module and its client
 * applications.
 */
int umi_repository_index_status_dirty(const UmiRepositoryIndexStatus *status);
/**
 * Check that repository index status satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_repository_index_status_validate(const UmiRepositoryIndexStatus *status);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_repository_index_status_archive_encode(const UmiRepositoryIndexStatus *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_repository_index_status_archive_decode(const void *bytes, size_t byte_count,
    UmiRepositoryIndexStatus *value);

#ifdef __cplusplus
}
#endif
#endif
