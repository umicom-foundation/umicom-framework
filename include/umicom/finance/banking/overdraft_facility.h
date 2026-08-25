/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/overdraft_facility.h
 *
 * PURPOSE:
 *   Represent deposit-account overdraft limits and utilised amount.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_OVERDRAFT_FACILITY_H
#define UMICOM_FINANCE_BANKING_OVERDRAFT_FACILITY_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankingOverdraftFacility {
    UmiFinancialId id;
    UmiFinancialId account_id;
    int64_t limit_minor;
    int64_t utilized_minor;
} UmiBankingOverdraftFacility;
UmiStatus umi_banking_overdraft_facility_init(UmiBankingOverdraftFacility *value,
    const char *id,
    const char *account_id,
    int64_t limit_minor,
    int64_t utilized_minor);
bool umi_banking_overdraft_facility_valid(const UmiBankingOverdraftFacility *value);
int64_t umi_banking_overdraft_facility_available_minor(const UmiBankingOverdraftFacility *value);
#ifdef __cplusplus
}
#endif
#endif
