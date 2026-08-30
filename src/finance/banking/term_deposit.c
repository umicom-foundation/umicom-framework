/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/term_deposit.c
 *
 * PURPOSE:
 *   Implement represent principal, maturity and rollover intent for term deposits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/term_deposit.h"
#include <string.h>
UmiStatus umi_banking_term_deposit_init(UmiBankingTermDeposit *value,
    const char *id,
    const char *customer_id,
    int64_t principal_minor,
    UmiFinancialDate start_date,
    UmiFinancialDate maturity_date,
    bool rollover) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->customer_id,customer_id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->principal_minor=principal_minor;
    value->start_date=start_date;
    value->maturity_date=maturity_date;
    value->rollover=rollover;
    return umi_banking_term_deposit_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_term_deposit_valid(const UmiBankingTermDeposit *value) {
    return value!=NULL && (umi_financial_id_is_valid(&value->customer_id) && value->principal_minor>0 && umi_financial_date_is_valid(value->start_date) && umi_financial_date_is_valid(value->maturity_date) && umi_financial_date_compare(value->start_date,value->maturity_date)<0);
}

bool umi_banking_term_deposit_auto_rollover(const UmiBankingTermDeposit *value) {
    if(value==NULL) return (bool)0;
    return value->rollover;
}
