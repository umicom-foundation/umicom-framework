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

#ifdef __cplusplus
}
#endif
#endif
