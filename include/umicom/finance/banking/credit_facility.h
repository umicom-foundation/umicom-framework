/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/credit_facility.h
 *
 * PURPOSE:
 *   Represent approved revolving credit limits and utilisation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_CREDIT_FACILITY_H
#define UMICOM_FINANCE_BANKING_CREDIT_FACILITY_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking credit facility data shared with callers of this public contract.
 */
typedef struct UmiBankingCreditFacility {
    UmiFinancialId id;
    UmiFinancialId customer_id;
    int64_t limit_minor;
    int64_t utilized_minor;
} UmiBankingCreditFacility;
/**
 * Initialise banking credit facility from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_credit_facility_init(UmiBankingCreditFacility *value,
    const char *id,
    const char *customer_id,
    int64_t limit_minor,
    int64_t utilized_minor);
/**
 * Check that banking credit facility satisfies its contract before another service relies
 * on it.
 */
bool umi_banking_credit_facility_valid(const UmiBankingCreditFacility *value);
/**
 * Provide the banking credit facility available minor operation used by this module and
 * its client applications.
 */
int64_t umi_banking_credit_facility_available_minor(const UmiBankingCreditFacility *value);
#ifdef __cplusplus
}
#endif
#endif
