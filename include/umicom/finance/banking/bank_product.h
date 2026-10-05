/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/bank_product.h
 *
 * PURPOSE:
 *   Describe reusable banking product templates independent of channel applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_BANK_PRODUCT_H
#define UMICOM_FINANCE_BANKING_BANK_PRODUCT_H
#include "umicom/finance/banking/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking bank product data shared with callers of this public contract.
 */
typedef struct UmiBankingBankProduct {
    UmiFinancialId id;
    char name[UMI_BANKING_NAME_CAPACITY];
    UmiBankingProductKind kind;
    bool active;
} UmiBankingBankProduct;
/**
 * Initialise banking bank product from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_bank_product_init(UmiBankingBankProduct *value,
    const char *id,
    const char *name,
    UmiBankingProductKind kind,
    bool active);
/**
 * Check that banking bank product satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_bank_product_valid(const UmiBankingBankProduct *value);
/**
 * Provide the banking bank product available operation used by this module and its client
 * applications.
 */
bool umi_banking_bank_product_available(const UmiBankingBankProduct *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_banking_bank_product_archive_encode(const UmiBankingBankProduct *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_banking_bank_product_archive_decode(const void *bytes, size_t byte_count,
    UmiBankingBankProduct *value);

#ifdef __cplusplus
}
#endif
#endif
