/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/credit_facility.h
 *
 * PURPOSE:
 *   Represent approved revolving credit limits and utilisation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_CREDIT_FACILITY_H
#define UMICOM_FINANCE_BANKING_CREDIT_FACILITY_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankingCreditFacility {
    UmiFinancialId id;
    UmiFinancialId customer_id;
    int64_t limit_minor;
    int64_t utilized_minor;
} UmiBankingCreditFacility;
UmiStatus umi_banking_credit_facility_init(UmiBankingCreditFacility *value,
    const char *id,
    const char *customer_id,
    int64_t limit_minor,
    int64_t utilized_minor);
bool umi_banking_credit_facility_valid(const UmiBankingCreditFacility *value);
int64_t umi_banking_credit_facility_available_minor(const UmiBankingCreditFacility *value);
#ifdef __cplusplus
}
#endif
#endif
