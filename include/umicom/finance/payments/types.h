/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/types.h
 *
 * PURPOSE:
 *   Define bounded payment lifecycle, rail and financial-message types shared by Framework payment services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_TYPES_H
#define UMICOM_FINANCE_PAYMENTS_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PAYMENTS_ID_CAPACITY UMI_FINANCE_ID_CAPACITY
#define UMI_PAYMENTS_NAME_CAPACITY UMI_FINANCIAL_CORE_NAME_CAPACITY
#define UMI_PAYMENTS_MAX_ITEMS 64U
#define UMI_PAYMENTS_MAX_LINES 32U
typedef enum UmiPaymentsStatus { UMI_PAYMENTS_CREATED=0, UMI_PAYMENTS_VALIDATED=1, UMI_PAYMENTS_APPROVED=2, UMI_PAYMENTS_RELEASED=3, UMI_PAYMENTS_CLEARED=4, UMI_PAYMENTS_SETTLED=5, UMI_PAYMENTS_RETURNED=6, UMI_PAYMENTS_REJECTED=7 } UmiPaymentsStatus;
typedef enum UmiPaymentsRailKind { UMI_PAYMENTS_RAIL_INTERNAL=0, UMI_PAYMENTS_RAIL_DOMESTIC=1, UMI_PAYMENTS_RAIL_HIGH_VALUE=2, UMI_PAYMENTS_RAIL_INSTANT=3, UMI_PAYMENTS_RAIL_CORRESPONDENT=4 } UmiPaymentsRailKind;
typedef enum UmiPaymentsMessageDirection { UMI_PAYMENTS_MESSAGE_OUTBOUND=0, UMI_PAYMENTS_MESSAGE_INBOUND=1 } UmiPaymentsMessageDirection;
/* Assign a canonical Framework financial identifier from text. */
UmiStatus umi_payments_id_assign(UmiFinancialId *id,const char *text);
/* Build and validate a three-letter ISO-style currency code. */
UmiStatus umi_payments_currency_from_code(const char *code,UmiCurrency *out_currency);
/* Compare canonical currency values without locale-dependent behaviour. */
bool umi_payments_currency_equal(UmiCurrency left,UmiCurrency right);
/* Return the magnitude of a signed minor-unit amount, saturating INT64_MIN. */
int64_t umi_payments_abs_i64(int64_t value);
#ifdef __cplusplus
}
#endif
#endif
