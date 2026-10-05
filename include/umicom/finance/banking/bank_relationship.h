/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/bank_relationship.h
 *
 * PURPOSE:
 *   Represent customer-to-bank relationship ownership independent of presentation channels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_BANK_RELATIONSHIP_H
#define UMICOM_FINANCE_BANKING_BANK_RELATIONSHIP_H
#include "umicom/finance/banking/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking bank relationship data shared with callers of this public
 * contract.
 */
typedef struct UmiBankingBankRelationship {
    UmiFinancialId id;
    UmiFinancialId customer_id;
    char relationship_manager[UMI_BANKING_NAME_CAPACITY];
    bool primary_relationship;
} UmiBankingBankRelationship;
/**
 * Initialise banking bank relationship from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_bank_relationship_init(UmiBankingBankRelationship *value,
    const char *id,
    const char *customer_id,
    const char *relationship_manager,
    bool primary_relationship);
/**
 * Check that banking bank relationship satisfies its contract before another service
 * relies on it.
 */
bool umi_banking_bank_relationship_valid(const UmiBankingBankRelationship *value);
/**
 * Provide the banking bank relationship primary operation used by this module and its
 * client applications.
 */
bool umi_banking_bank_relationship_primary(const UmiBankingBankRelationship *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_banking_bank_relationship_archive_encode(const UmiBankingBankRelationship *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_banking_bank_relationship_archive_decode(const void *bytes, size_t byte_count,
    UmiBankingBankRelationship *value);

#ifdef __cplusplus
}
#endif
#endif
