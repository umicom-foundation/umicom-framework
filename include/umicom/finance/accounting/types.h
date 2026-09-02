/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/types.h
 *
 * PURPOSE:
 *   Define bounded accounting account, journal and period types shared by Framework financial-control services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_TYPES_H
#define UMICOM_FINANCE_ACCOUNTING_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_ACCOUNTING_ID_CAPACITY UMI_FINANCE_ID_CAPACITY
#define UMI_ACCOUNTING_NAME_CAPACITY UMI_FINANCIAL_CORE_NAME_CAPACITY
#define UMI_ACCOUNTING_MAX_ITEMS 64U
#define UMI_ACCOUNTING_MAX_LINES 32U
/**
 * List the named accounting account class values accepted by this public contract.
 */
typedef enum UmiAccountingAccountClass { UMI_ACCOUNTING_ASSET=0, UMI_ACCOUNTING_LIABILITY=1, UMI_ACCOUNTING_EQUITY=2, UMI_ACCOUNTING_REVENUE=3, UMI_ACCOUNTING_EXPENSE=4 } UmiAccountingAccountClass;
/**
 * List the named accounting normal side values accepted by this public contract.
 */
typedef enum UmiAccountingNormalSide { UMI_ACCOUNTING_NORMAL_DEBIT=0, UMI_ACCOUNTING_NORMAL_CREDIT=1 } UmiAccountingNormalSide;
/**
 * List the named accounting journal status values accepted by this public contract.
 */
typedef enum UmiAccountingJournalStatus { UMI_ACCOUNTING_JOURNAL_DRAFT=0, UMI_ACCOUNTING_JOURNAL_VALIDATED=1, UMI_ACCOUNTING_JOURNAL_POSTED=2, UMI_ACCOUNTING_JOURNAL_REVERSED=3 } UmiAccountingJournalStatus;
/**
 * List the named accounting period status values accepted by this public contract.
 */
typedef enum UmiAccountingPeriodStatus { UMI_ACCOUNTING_PERIOD_OPEN=0, UMI_ACCOUNTING_PERIOD_SOFT_CLOSED=1, UMI_ACCOUNTING_PERIOD_CLOSED=2 } UmiAccountingPeriodStatus;
/* Assign a canonical Framework financial identifier from text. */
UmiStatus umi_accounting_id_assign(UmiFinancialId *id,const char *text);
/* Build and validate a three-letter ISO-style currency code. */
UmiStatus umi_accounting_currency_from_code(const char *code,UmiCurrency *out_currency);
/* Compare canonical currency values without locale-dependent behaviour. */
bool umi_accounting_currency_equal(UmiCurrency left,UmiCurrency right);
/* Return the magnitude of a signed minor-unit amount, saturating INT64_MIN. */
int64_t umi_accounting_abs_i64(int64_t value);
#ifdef __cplusplus
}
#endif
#endif
