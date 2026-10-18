/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/promotion.h
 *
 * PURPOSE:
 *   Represent promotion of a verified release from one channel to another.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Promotion reuses the same immutable artifact set while changing its approved audience and release policy.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_PROMOTION_H
#define INCLUDE_UMICOM_DELIVERY_PROMOTION_H

#include "umicom/base/status.h"
#include "umicom/base/value_archive.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the promotion data shared with callers of this public contract.
 */
typedef struct UmiPromotion {
    char release_id[UMI_DELIVERY_ID_CAPACITY];
    UmiReleaseChannel from_channel;
    UmiReleaseChannel to_channel;
    UmiEvidenceStatus status;
} UmiPromotion;
/**
 * Initialise promotion from caller-provided values so later operations receive a known
 * state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_promotion_init(UmiPromotion *promotion,
                             const char *release_id,
                             UmiReleaseChannel from_channel,
                             UmiReleaseChannel to_channel);
/**
 * Check that promotion direction satisfies its contract before another service relies on
 * it.
 */
int umi_promotion_direction_valid(const UmiPromotion *promotion);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_promotion_direction_archive_encode(const UmiPromotion *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_promotion_direction_archive_decode(const void *bytes, size_t byte_count,
    UmiPromotion *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiPromotion object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_promotion_direction_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiPromotion *workspace, UmiPromotion *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_promotion_direction_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPromotion *expected, UmiPromotion *value);


#ifdef __cplusplus
}
#endif

#endif
