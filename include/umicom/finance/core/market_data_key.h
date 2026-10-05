/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/market_data_key.h
 *
 * PURPOSE:
 *   Define canonical market-data keys.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_MARKET_DATA_KEY_H
#define UMICOM_FINANCE_CORE_MARKET_DATA_KEY_H

#include "umicom/finance/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the market data key data shared with callers of this public contract.
 */
typedef struct UmiMarketDataKey { UmiFinancialId key_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; uint32_t state; bool active; } UmiMarketDataKey;
/* Initialize the typed financial record. */ UmiStatus umi_market_data_key_init(UmiMarketDataKey *item,const char *id,const char *name,const char *code,uint32_t state);
/* Validate the typed financial record. */ bool umi_market_data_key_is_valid(const UmiMarketDataKey *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_market_data_key_archive_encode(const UmiMarketDataKey *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_market_data_key_archive_decode(const void *bytes, size_t byte_count,
    UmiMarketDataKey *value);

#ifdef __cplusplus
}
#endif

#endif
