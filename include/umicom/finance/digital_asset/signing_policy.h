/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/signing_policy.h
 *
 * PURPOSE:
 *   Define threshold and hardware requirements for governed transaction signing.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_SIGNING_POLICY_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_SIGNING_POLICY_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital signing policy data shared with callers of this public contract.
 */
typedef struct UmiDigitalSigningPolicy {
    UmiDigitalAssetId id;
    uint32_t required_approvals;
    uint32_t available_approvers;
    bool hardware_required;
    bool active;
} UmiDigitalSigningPolicy;

/* Initialise a bounded signing policy record for reusable Framework workflows. */
UmiStatus umi_digital_asset_signing_policy_init(UmiDigitalSigningPolicy *value, const char *id, uint32_t required_approvals, uint32_t available_approvers, bool hardware_required);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_signing_policy_valid(const UmiDigitalSigningPolicy *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_digital_asset_signing_policy_archive_encode(const UmiDigitalSigningPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_digital_asset_signing_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiDigitalSigningPolicy *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDigitalSigningPolicy object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_digital_asset_signing_policy_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDigitalSigningPolicy *workspace, UmiDigitalSigningPolicy *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_digital_asset_signing_policy_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDigitalSigningPolicy *expected, UmiDigitalSigningPolicy *value);


#ifdef __cplusplus
}
#endif

#endif
