/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_messaging/clearing_preview.c
 *
 * PURPOSE:
 *   Aggregate reviewed obligations without crossing account, scheme or currency boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/financial_messaging/clearing_preview.h"
#include "umicom/finance/currency.h"
#include <string.h>
static int ScopeValid(const UmiUfmpClearingScope *scope) {
  return scope != NULL && scope->value_day >= 0 && scope->scale <= 18U &&
         umi_currency_valid(&scope->currency) &&
         UmiUfmpIdentifierValid(scope->owner, sizeof scope->owner) &&
         UmiUfmpIdentifierValid(scope->counterparty, sizeof scope->counterparty) &&
         UmiUfmpIdentifierValid(scope->settlement_account, sizeof scope->settlement_account) &&
         UmiUfmpIdentifierValid(scope->scheme, sizeof scope->scheme) &&
         strcmp(scope->owner, scope->counterparty) != 0;
}
static int SameScope(const UmiUfmpClearingScope *left, const UmiUfmpClearingScope *right) {
  return ScopeValid(right) && left->value_day == right->value_day && left->scale == right->scale &&
         memcmp(left->currency.code, right->currency.code, sizeof left->currency.code) == 0 &&
         strcmp(left->owner, right->owner) == 0 &&
         strcmp(left->counterparty, right->counterparty) == 0 &&
         strcmp(left->settlement_account, right->settlement_account) == 0 &&
         strcmp(left->scheme, right->scheme) == 0;
}
UmiStatus UmiUfmpClearingPreviewBuild(const UmiUfmpClearingScope *scope,
                                      const UmiUfmpClearingObligation *rows, size_t count,
                                      UmiUfmpClearingPreview *out) {
  if (out == NULL || !ScopeValid(scope) || (count != 0U && rows == NULL))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (count > UMI_UFMP_CLEARING_MAX_OBLIGATIONS)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiUfmpClearingPreview result = {0};
  result.scope = *scope;
  result.obligation_count = count;
  result.gross_pay.currency = scope->currency;
  result.gross_pay.scale = scope->scale;
  result.gross_receive = result.gross_pay;
  for (size_t i = 0U; i < count; ++i) {
    const UmiUfmpClearingObligation *row = &rows[i];
    if (!UmiUfmpIdentifierValid(row->id, sizeof row->id) || !SameScope(scope, &row->scope) ||
        row->amount.minor_units <= 0 ||
        (row->direction != UMI_UFMP_CLEARING_PAY && row->direction != UMI_UFMP_CLEARING_RECEIVE))
      return UMI_STATUS_INVALID_ARGUMENT;
    /* An import must reconcile repeated IDs before computing obligations.
     * Silently adding a duplicate would double the proposed exposure. */
    for (size_t previous = 0U; previous < i; ++previous)
      if (strcmp(rows[previous].id, row->id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    UmiMoney *total =
        row->direction == UMI_UFMP_CLEARING_PAY ? &result.gross_pay : &result.gross_receive;
    UmiStatus status = umi_money_add(total, &row->amount, total);
    if (status != UMI_STATUS_OK)
      return status;
  }
  UmiStatus status =
      umi_money_subtract(&result.gross_receive, &result.gross_pay, &result.net_receive);
  if (status == UMI_STATUS_OK)
    *out = result;
  return status;
}
