/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/bank_account_registry.h
 *
 * PURPOSE:
 *   Register external treasury bank accounts with deterministic lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_BANK_ACCOUNT_REGISTRY_H
#define UMICOM_FINANCE_TREASURY_BANK_ACCOUNT_REGISTRY_H
#include "umicom/finance/treasury/bank_account.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury bank account registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryBankAccountRegistry {
    UmiTreasuryBankAccount items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryBankAccountRegistry;
/**
 * Initialise treasury bank account registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_treasury_bank_account_registry_init(UmiTreasuryBankAccountRegistry *registry);
/**
 * Add treasury bank account registry only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_treasury_bank_account_registry_add(UmiTreasuryBankAccountRegistry *registry, const UmiTreasuryBankAccount *item);
/**
 * Find treasury bank account registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiTreasuryBankAccount *umi_treasury_bank_account_registry_find(const UmiTreasuryBankAccountRegistry *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
