/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/cache_layout.h
 *
 * PURPOSE:
 *   cache namespace and eviction-budget configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_CACHE_LAYOUT_H
#define UMICOM_DISTRIBUTION_RUNTIME_CACHE_LAYOUT_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr cache layout data shared with callers of this public contract.
 */
typedef struct UmiDrCacheLayout { char id[UMI_DR_ID_CAPACITY]; char namespace_id[UMI_DR_ID_CAPACITY]; uint64_t max_bytes; bool disposable; } UmiDrCacheLayout;
/**
 * Initialise dr cache layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_cache_layout_init(UmiDrCacheLayout *value);
/**
 * Check that dr cache layout satisfies its contract before another service relies on it.
 */
bool umi_dr_cache_layout_valid(const UmiDrCacheLayout *value);
/**
 * Provide the dr cache layout fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_cache_layout_fingerprint(const UmiDrCacheLayout *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_cache_layout_archive_encode(const UmiDrCacheLayout *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_cache_layout_archive_decode(const void *bytes, size_t byte_count,
    UmiDrCacheLayout *value);

#ifdef __cplusplus
}
#endif
#endif
