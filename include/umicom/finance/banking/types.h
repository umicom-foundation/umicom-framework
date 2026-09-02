/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/types.h
 *
 * PURPOSE:
 *   Define bounded banking, customer, account and lending types shared by Framework banking services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_TYPES_H
#define UMICOM_FINANCE_BANKING_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BANKING_ID_CAPACITY UMI_FINANCE_ID_CAPACITY
#define UMI_BANKING_NAME_CAPACITY UMI_FINANCIAL_CORE_NAME_CAPACITY
#define UMI_BANKING_MAX_ITEMS 64U
#define UMI_BANKING_MAX_LINES 32U
/**
 * List the named banking customer status values accepted by this public contract.
 */
typedef enum UmiBankingCustomerStatus { UMI_BANKING_CUSTOMER_PROSPECT=0, UMI_BANKING_CUSTOMER_ACTIVE=1, UMI_BANKING_CUSTOMER_SUSPENDED=2, UMI_BANKING_CUSTOMER_CLOSED=3 } UmiBankingCustomerStatus;
/**
 * List the named banking segment values accepted by this public contract.
 */
typedef enum UmiBankingSegment { UMI_BANKING_SEGMENT_RETAIL=0, UMI_BANKING_SEGMENT_SME=1, UMI_BANKING_SEGMENT_CORPORATE=2, UMI_BANKING_SEGMENT_INSTITUTIONAL=3 } UmiBankingSegment;
/**
 * List the named banking product kind values accepted by this public contract.
 */
typedef enum UmiBankingProductKind { UMI_BANKING_PRODUCT_DEPOSIT=0, UMI_BANKING_PRODUCT_TERM_DEPOSIT=1, UMI_BANKING_PRODUCT_LOAN=2, UMI_BANKING_PRODUCT_CREDIT=3, UMI_BANKING_PRODUCT_OVERDRAFT=4 } UmiBankingProductKind;
/**
 * List the named banking account status values accepted by this public contract.
 */
typedef enum UmiBankingAccountStatus { UMI_BANKING_ACCOUNT_PENDING=0, UMI_BANKING_ACCOUNT_OPEN=1, UMI_BANKING_ACCOUNT_BLOCKED=2, UMI_BANKING_ACCOUNT_CLOSED=3 } UmiBankingAccountStatus;
/**
 * List the named banking loan status values accepted by this public contract.
 */
typedef enum UmiBankingLoanStatus { UMI_BANKING_LOAN_APPLICATION=0, UMI_BANKING_LOAN_ACTIVE=1, UMI_BANKING_LOAN_ARREARS=2, UMI_BANKING_LOAN_REPAID=3, UMI_BANKING_LOAN_DEFAULTED=4 } UmiBankingLoanStatus;
/* Assign a canonical Framework financial identifier from text. */
UmiStatus umi_banking_id_assign(UmiFinancialId *id,const char *text);
/* Build and validate a three-letter ISO-style currency code. */
UmiStatus umi_banking_currency_from_code(const char *code,UmiCurrency *out_currency);
/* Compare canonical currency values without locale-dependent behaviour. */
bool umi_banking_currency_equal(UmiCurrency left,UmiCurrency right);
/* Return the magnitude of a signed minor-unit amount, saturating INT64_MIN. */
int64_t umi_banking_abs_i64(int64_t value);
#ifdef __cplusplus
}
#endif
#endif
