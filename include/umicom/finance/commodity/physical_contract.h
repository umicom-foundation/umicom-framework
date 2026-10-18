/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/physical_contract.h
 *
 * PURPOSE:
 *   Define a reusable bilateral physical commodity contract with quantity and delivery terms.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_PHYSICAL_CONTRACT_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_PHYSICAL_CONTRACT_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity physical contract data shared with callers of this public
 * contract.
 */
typedef struct UmiCommodityPhysicalContract {
    UmiCommodityId id;
    UmiCommodityId commodity_id;
    UmiFinancialId buyer_party_id;
    UmiFinancialId seller_party_id;
    UmiCommodityQuantity quantity;
    UmiCurrency price_currency;
    int64_t price_minor_units_per_unit;
    int64_t delivery_start_ms;
    int64_t delivery_end_ms;
    bool active;
} UmiCommodityPhysicalContract;

/* Initialise a bounded physical contract record for reusable Framework workflows. */
UmiStatus umi_commodity_physical_contract_init(UmiCommodityPhysicalContract *value, const char *id, const char *commodity_id, const UmiFinancialId *buyer_party_id, const UmiFinancialId *seller_party_id, int64_t units, int32_t scale, const char *unit_code, const UmiCurrency *price_currency, int64_t price_minor_units_per_unit, int64_t delivery_start_ms, int64_t delivery_end_ms);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_physical_contract_valid(const UmiCommodityPhysicalContract *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_commodity_physical_contract_archive_encode(const UmiCommodityPhysicalContract *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_commodity_physical_contract_archive_decode(const void *bytes, size_t byte_count,
    UmiCommodityPhysicalContract *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiCommodityPhysicalContract object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_commodity_physical_contract_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiCommodityPhysicalContract *workspace, UmiCommodityPhysicalContract *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_commodity_physical_contract_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiCommodityPhysicalContract *expected, UmiCommodityPhysicalContract *value);


#ifdef __cplusplus
}
#endif

#endif
