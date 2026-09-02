/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/overdraft_facility.h
 *
 * PURPOSE:
 *   Represent deposit-account overdraft limits and utilised amount.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_OVERDRAFT_FACILITY_H
#define UMICOM_FINANCE_BANKING_OVERDRAFT_FACILITY_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking overdraft facility data shared with callers of this public
 * contract.
 */
typedef struct UmiBankingOverdraftFacility {
    UmiFinancialId id;
    UmiFinancialId account_id;
    int64_t limit_minor;
    int64_t utilized_minor;
} UmiBankingOverdraftFacility;
/**
 * Initialise banking overdraft facility from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_overdraft_facility_init(UmiBankingOverdraftFacility *value,
    const char *id,
    const char *account_id,
    int64_t limit_minor,
    int64_t utilized_minor);
/**
 * Check that banking overdraft facility satisfies its contract before another service
 * relies on it.
 */
bool umi_banking_overdraft_facility_valid(const UmiBankingOverdraftFacility *value);
/**
 * Provide the banking overdraft facility available minor operation used by this module and
 * its client applications.
 */
int64_t umi_banking_overdraft_facility_available_minor(const UmiBankingOverdraftFacility *value);
#ifdef __cplusplus
}
#endif
#endif
