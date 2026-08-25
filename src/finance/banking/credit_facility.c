/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/credit_facility.c
 *
 * PURPOSE:
 *   Implement represent approved revolving credit limits and utilisation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/credit_facility.h"
#include <string.h>
UmiStatus umi_banking_credit_facility_init(UmiBankingCreditFacility *value,
    const char *id,
    const char *customer_id,
    int64_t limit_minor,
    int64_t utilized_minor) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->customer_id,customer_id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->limit_minor=limit_minor;
    value->utilized_minor=utilized_minor;
    return umi_banking_credit_facility_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_credit_facility_valid(const UmiBankingCreditFacility *value) {
    return value!=NULL && (value->limit_minor>0 && value->utilized_minor>=0 && value->utilized_minor<=value->limit_minor);
}

int64_t umi_banking_credit_facility_available_minor(const UmiBankingCreditFacility *value) {
    if(value==NULL) return (int64_t)0;
    return value->limit_minor-value->utilized_minor;
}
