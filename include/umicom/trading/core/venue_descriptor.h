/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/venue_descriptor.h
 *
 * PURPOSE:
 *   Define exchange and execution-venue identity and capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_VENUE_DESCRIPTOR_H
#define UMICOM_TRADING_CORE_VENUE_DESCRIPTOR_H
#include "umicom/trading/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the trading venue descriptor data shared with callers of this public contract.
 */
typedef struct UmiTradingVenueDescriptor {
    UmiFinancialId venue_id;
    char mic[8];
    char name[UMI_TRADING_CORE_TEXT_CAPACITY];
    bool supports_auctions;
    bool supports_hidden_liquidity;
    uint32_t priority;
} UmiTradingVenueDescriptor;
/* Initialise a venue descriptor with bounded identifiers and text. */
UmiStatus umi_trading_venue_descriptor_init(UmiTradingVenueDescriptor *venue,const char *id,const char *mic,const char *name,bool auctions,bool hidden,uint32_t priority);
/* Validate the required venue identity fields. */
bool umi_trading_venue_descriptor_valid(const UmiTradingVenueDescriptor *venue);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trading_venue_descriptor_archive_encode(const UmiTradingVenueDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trading_venue_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiTradingVenueDescriptor *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTradingVenueDescriptor object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_trading_venue_descriptor_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTradingVenueDescriptor *workspace, UmiTradingVenueDescriptor *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_trading_venue_descriptor_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTradingVenueDescriptor *expected, UmiTradingVenueDescriptor *value);


#ifdef __cplusplus
}
#endif
#endif
