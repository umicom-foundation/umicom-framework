/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/remote_status.h
 *
 * PURPOSE:
 *   Represent repository remote and upstream configuration health.
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
#ifndef INCLUDE_UMICOM_REPOSITORY_REMOTE_STATUS_H
#define INCLUDE_UMICOM_REPOSITORY_REMOTE_STATUS_H
#include <stddef.h>
#include "umicom/base/value_archive.h"
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the repository remote status data shared with callers of this public contract.
 */
typedef struct UmiRepositoryRemoteStatus {
    size_t remote_count;
    int has_origin;
    int upstream_configured;
    int fetch_available;
} UmiRepositoryRemoteStatus;

/**
 * Initialise repository remote status from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_remote_status_init(UmiRepositoryRemoteStatus *status);
/**
 * Check that repository remote status satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_repository_remote_status_validate(const UmiRepositoryRemoteStatus *status);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_repository_remote_status_archive_encode(const UmiRepositoryRemoteStatus *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_repository_remote_status_archive_decode(const void *bytes, size_t byte_count,
    UmiRepositoryRemoteStatus *value);

#ifdef __cplusplus
}
#endif
#endif
