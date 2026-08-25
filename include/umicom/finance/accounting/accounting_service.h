/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/accounting_service.h
 *
 * PURPOSE:
 *   Represent Framework accounting-service readiness and ledger control state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_ACCOUNTING_SERVICE_H
#define UMICOM_FINANCE_ACCOUNTING_ACCOUNTING_SERVICE_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingAccountingService {
    UmiFinancialId id;
    bool running;
    size_t account_count;
    size_t posted_journals;
    size_t open_periods;
    size_t control_breaches;
} UmiAccountingAccountingService;
UmiStatus umi_accounting_accounting_service_init(UmiAccountingAccountingService *value,
    const char *id,
    bool running,
    size_t account_count,
    size_t posted_journals,
    size_t open_periods,
    size_t control_breaches);
bool umi_accounting_accounting_service_valid(const UmiAccountingAccountingService *value);
bool umi_accounting_accounting_service_ready(const UmiAccountingAccountingService *value);
#ifdef __cplusplus
}
#endif
#endif
