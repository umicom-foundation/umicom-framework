/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance_operations/input.h
 *
 * PURPOSE:
 *   Parse beginner-facing command fields once in Framework, with exact numbers and explicit failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_OPERATIONS_INPUT_H
#define UMICOM_FINANCE_OPERATIONS_INPUT_H
#include "umicom/finance_operations/operations.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Accept a signed base-ten integer, optionally surrounded by ASCII spaces.
 * No decimals, exponent notation, grouping commas or overflow are accepted. */
UmiStatus UmiFinanceOperationsParseInteger(const char *text, int64_t *out);
/** Accept the exact YYYY-MM-DD format and a real Gregorian calendar date. */
UmiStatus UmiFinanceOperationsParseDate(const char *text, UmiFinancialDate *out);
/** Parse 2..8 lines: account-id,debit-minor,credit-minor. Blank lines are
 * ignored. IDs cannot contain commas. The complete command remains subject to
 * account, currency, period and balance validation when Apply is called. */
UmiStatus UmiFinanceOperationsParseLines(const char *text,
    UmiFinanceOperationLine *lines, size_t capacity, size_t *outCount);
/** Explain an operation result without implying authenticated or live access. */
const char *UmiFinanceOperationsExplainStatus(UmiStatus status);
#ifdef __cplusplus
}
#endif
#endif
