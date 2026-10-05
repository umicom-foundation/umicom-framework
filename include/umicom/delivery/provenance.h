/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/provenance.h
 *
 * PURPOSE:
 *   Record source revision, builder identity and build inputs for release provenance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Provenance links a package back to the exact source and build environment that created it.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_PROVENANCE_H
#define INCLUDE_UMICOM_DELIVERY_PROVENANCE_H

#include "umicom/base/status.h"
#include "umicom/base/value_archive.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the provenance data shared with callers of this public contract.
 */
typedef struct UmiProvenance {
    char source_revision[UMI_DELIVERY_ID_CAPACITY];
    char builder_id[UMI_DELIVERY_ID_CAPACITY];
    char build_preset[UMI_DELIVERY_ID_CAPACITY];
    char framework_version[UMI_DELIVERY_VERSION_CAPACITY];
    uint64_t created_epoch_ms;
} UmiProvenance;

/**
 * Initialise provenance from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_provenance_init(UmiProvenance *provenance,
                              const char *source_revision,
                              const char *builder_id,
                              const char *build_preset);
/**
 * Check that provenance satisfies its contract before another service relies on it.
 */
UmiStatus umi_provenance_validate(const UmiProvenance *provenance);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_provenance_archive_encode(const UmiProvenance *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_provenance_archive_decode(const void *bytes, size_t byte_count,
    UmiProvenance *value);

#ifdef __cplusplus
}
#endif

#endif
