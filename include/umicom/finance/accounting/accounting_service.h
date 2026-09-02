/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/accounting_service.h
 *
 * PURPOSE:
 *   Represent Framework accounting-service readiness and ledger control state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_ACCOUNTING_SERVICE_H
#define UMICOM_FINANCE_ACCOUNTING_ACCOUNTING_SERVICE_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting accounting service data shared with callers of this public
 * contract.
 */
typedef struct UmiAccountingAccountingService {
    UmiFinancialId id;
    bool running;
    size_t account_count;
    size_t posted_journals;
    size_t open_periods;
    size_t control_breaches;
} UmiAccountingAccountingService;
/**
 * Initialise accounting accounting service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_accounting_service_init(UmiAccountingAccountingService *value,
    const char *id,
    bool running,
    size_t account_count,
    size_t posted_journals,
    size_t open_periods,
    size_t control_breaches);
/**
 * Check that accounting accounting service satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_accounting_service_valid(const UmiAccountingAccountingService *value);
/**
 * Provide the accounting accounting service ready operation used by this module and its
 * client applications.
 */
bool umi_accounting_accounting_service_ready(const UmiAccountingAccountingService *value);
#ifdef __cplusplus
}
#endif
#endif
