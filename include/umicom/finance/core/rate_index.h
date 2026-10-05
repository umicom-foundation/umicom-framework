/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/rate_index.h
 *
 * PURPOSE:
 *   Define reusable rate-index metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_RATE_INDEX_H
#define UMICOM_FINANCE_CORE_RATE_INDEX_H

#include "umicom/finance/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rate index data shared with callers of this public contract.
 */
typedef struct UmiRateIndex { UmiFinancialId index_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; uint32_t state; bool active; } UmiRateIndex;
/* Initialize the typed financial record. */ UmiStatus umi_rate_index_init(UmiRateIndex *item,const char *id,const char *name,const char *code,uint32_t state);
/* Validate the typed financial record. */ bool umi_rate_index_is_valid(const UmiRateIndex *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rate_index_archive_encode(const UmiRateIndex *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rate_index_archive_decode(const void *bytes, size_t byte_count,
    UmiRateIndex *value);

#ifdef __cplusplus
}
#endif

#endif
