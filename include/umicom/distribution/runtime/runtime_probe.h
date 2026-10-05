/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/runtime_probe.h
 *
 * PURPOSE:
 *   deterministic probe snapshots describing detected host runtime properties.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_PROBE_H
#define UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_PROBE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr runtime probe data shared with callers of this public contract.
 */
typedef struct UmiDrRuntimeProbe { char id[UMI_DR_ID_CAPACITY]; UmiDrPlatform platform; UmiDrArchitecture architecture; UmiDrVersion version; uint64_t capabilities; uint64_t memory_mb; } UmiDrRuntimeProbe;
/**
 * Initialise dr runtime probe from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_runtime_probe_init(UmiDrRuntimeProbe *value);
/**
 * Check that dr runtime probe satisfies its contract before another service relies on it.
 */
bool umi_dr_runtime_probe_valid(const UmiDrRuntimeProbe *value);
/**
 * Provide the dr runtime probe fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_runtime_probe_fingerprint(const UmiDrRuntimeProbe *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_runtime_probe_archive_encode(const UmiDrRuntimeProbe *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_runtime_probe_archive_decode(const void *bytes, size_t byte_count,
    UmiDrRuntimeProbe *value);

#ifdef __cplusplus
}
#endif
#endif
