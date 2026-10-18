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
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
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

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiProvenance object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_provenance_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiProvenance *workspace, UmiProvenance *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_provenance_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiProvenance *expected, UmiProvenance *value);


#ifdef __cplusplus
}
#endif

#endif
