/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/product_id.h
 *
 * PURPOSE:
 *   Provide a strongly typed product id wrapper over the existing financial identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_PRODUCT_ID_H
#define UMICOM_FINANCE_CORE_PRODUCT_ID_H

#include "umicom/finance/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the product id data shared with callers of this public contract.
 */
typedef struct UmiProductId { UmiFinancialId id; } UmiProductId;
/* Assign identifier. */ UmiStatus umi_product_id_set(UmiProductId *id,const char *value);
/* Validate identifier. */ bool umi_product_id_is_valid(const UmiProductId *id);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_product_id_archive_encode(const UmiProductId *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_product_id_archive_decode(const void *bytes, size_t byte_count,
    UmiProductId *value);

#ifdef __cplusplus
}
#endif

#endif
