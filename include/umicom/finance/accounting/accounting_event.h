/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/accounting_event.h
 *
 * PURPOSE:
 *   Represent canonical economic events before posting-rule transformation into journals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_ACCOUNTING_EVENT_H
#define UMICOM_FINANCE_ACCOUNTING_ACCOUNTING_EVENT_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingAccountingEvent {
    UmiFinancialId id;
    char event_type[UMI_FINANCIAL_CORE_CODE_CAPACITY];
    UmiFinancialDate accounting_date;
    int64_t amount_minor;
    UmiCurrency currency;
} UmiAccountingAccountingEvent;
UmiStatus umi_accounting_accounting_event_init(UmiAccountingAccountingEvent *value,
    const char *id,
    const char *event_type,
    UmiFinancialDate accounting_date,
    int64_t amount_minor,
    const char *currency_code);
bool umi_accounting_accounting_event_valid(const UmiAccountingAccountingEvent *value);
int64_t umi_accounting_accounting_event_absolute_minor(const UmiAccountingAccountingEvent *value);
#ifdef __cplusplus
}
#endif
#endif
