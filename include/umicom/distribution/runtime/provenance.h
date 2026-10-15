/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/provenance.h
 *
 * PURPOSE:
 *   build/source/toolchain provenance evidence for packaged releases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_PROVENANCE_H
#define UMICOM_DISTRIBUTION_RUNTIME_PROVENANCE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr provenance data shared with callers of this public contract.
 */
typedef struct UmiDrProvenance { char id[UMI_DR_ID_CAPACITY]; char source_revision[UMI_DR_DIGEST_CAPACITY]; char toolchain[UMI_DR_TEXT_CAPACITY]; char builder[UMI_DR_TEXT_CAPACITY]; bool reproducible; } UmiDrProvenance;
/**
 * Initialise dr provenance from caller-provided values so later operations receive a known
 * state.
 */
void umi_dr_provenance_init(UmiDrProvenance *value);
/**
 * Check that dr provenance satisfies its contract before another service relies on it.
 */
bool umi_dr_provenance_valid(const UmiDrProvenance *value);
/**
 * Provide the dr provenance fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_provenance_fingerprint(const UmiDrProvenance *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_provenance_archive_encode(const UmiDrProvenance *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_provenance_archive_decode(const void *bytes, size_t byte_count,
    UmiDrProvenance *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_dr_provenance_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDrProvenance *expected, UmiDrProvenance *value);


#ifdef __cplusplus
}
#endif
#endif
