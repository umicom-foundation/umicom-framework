/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/commodity_descriptor.h
 *
 * PURPOSE:
 *   Define reference-data metadata for a physical or financially settled commodity.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_COMMODITY_DESCRIPTOR_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_COMMODITY_DESCRIPTOR_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity descriptor data shared with callers of this public contract.
 */
typedef struct UmiCommodityDescriptor {
    UmiCommodityId id;
    char name[UMI_COMMODITY_NAME_CAPACITY];
    char code[UMI_COMMODITY_CODE_CAPACITY];
    UmiCommodityKind kind;
    UmiCurrency settlement_currency;
    bool physical_delivery;
    bool active;
} UmiCommodityDescriptor;

/* Initialise a bounded commodity descriptor record for reusable Framework workflows. */
UmiStatus umi_commodity_commodity_descriptor_init(UmiCommodityDescriptor *value, const char *id, const char *name, const char *code, UmiCommodityKind kind, const UmiCurrency *currency, bool physical_delivery);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_commodity_descriptor_valid(const UmiCommodityDescriptor *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_commodity_commodity_descriptor_archive_encode(const UmiCommodityDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_commodity_commodity_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiCommodityDescriptor *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiCommodityDescriptor object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_commodity_commodity_descriptor_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiCommodityDescriptor *workspace, UmiCommodityDescriptor *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_commodity_commodity_descriptor_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiCommodityDescriptor *expected, UmiCommodityDescriptor *value);


#ifdef __cplusplus
}
#endif

#endif
