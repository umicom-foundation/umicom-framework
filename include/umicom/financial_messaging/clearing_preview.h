/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_messaging/clearing_preview.h
 *
 * PURPOSE:
 *   Define exact, scoped clearing previews without authorizing transfers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCIAL_MESSAGING_CLEARING_PREVIEW_H
#define UMICOM_FINANCIAL_MESSAGING_CLEARING_PREVIEW_H
#include "umicom/finance/money.h"
#include "umicom/financial_messaging/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_UFMP_CLEARING_MAX_OBLIGATIONS 4096U
/* One preview describes a bilateral, single-currency/account/scheme/day scope.
 * value_day is a nonnegative count of UTC calendar days since 1970-01-01.
 * This groups reviewed obligations; it does not establish legal netting rights. */
typedef struct UmiUfmpClearingScope {
  char owner[UMI_UFMP_ID_CAPACITY];
  char counterparty[UMI_UFMP_ID_CAPACITY];
  char settlement_account[UMI_UFMP_ID_CAPACITY];
  char scheme[UMI_UFMP_ID_CAPACITY];
  int32_t value_day;
  UmiCurrency currency;
  uint8_t scale;
} UmiUfmpClearingScope;
typedef enum UmiUfmpClearingDirection {
  UMI_UFMP_CLEARING_PAY = 1,
  UMI_UFMP_CLEARING_RECEIVE = 2
} UmiUfmpClearingDirection;
typedef struct UmiUfmpClearingObligation {
  char id[UMI_UFMP_ID_CAPACITY];
  UmiUfmpClearingScope scope;
  UmiUfmpClearingDirection direction;
  UmiMoney amount;
} UmiUfmpClearingObligation;
typedef struct UmiUfmpClearingPreview {
  UmiUfmpClearingScope scope;
  size_t obligation_count;
  UmiMoney gross_pay;
  UmiMoney gross_receive;
  UmiMoney net_receive; /* Negative means the scoped owner would pay. */
} UmiUfmpClearingPreview;
/* Pure all-or-nothing calculation using the existing checked money service.
 * Every row must match the explicit scope, amount currency and scale. Positive
 * amounts only; duplicate obligation IDs are refused even when identical.
 * Cross-currency arithmetic, FX conversion and automatic account combining
 * are deliberately absent. Gross totals are bounded by INT64_MAX; a small
 * net does not make an overflowing gross total acceptable.
 * Count zero accepts NULL rows and returns a zero preview for a valid scope.
 * Output is unchanged on failure. No allocation, database or network calls;
 * caller serializes inputs. A preview never authorizes payment or settlement. */
UmiStatus UmiUfmpClearingPreviewBuild(const UmiUfmpClearingScope *scope,
                                      const UmiUfmpClearingObligation *rows, size_t count,
                                      UmiUfmpClearingPreview *out);
/* Shared synthetic report for thin product consoles. required excludes the
 * terminating zero. NULL/zero measures; short outputs are left unchanged. */
UmiStatus UmiUfmpClearingExampleFormat(char *output, size_t capacity, size_t *required);
#ifdef __cplusplus
}
#endif
#endif
