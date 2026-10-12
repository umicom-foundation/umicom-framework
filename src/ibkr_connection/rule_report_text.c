/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/rule_report_text.c
 * PURPOSE: Present saved rules as evidence while retaining original provider notation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "rule_report_private.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
UmiStatus UmiIbkrRuleReportDescribe(const UmiIbkrRuleReport *report, size_t route, char **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (report == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (route >= report->identity.routeCount)
    return UMI_STATUS_NOT_FOUND;
  const RuleRow *row = &report->rows[route];
  const UmiIbkrRuleReportIdentity *identity = &report->identity;
  /* Every string was bounded at import. Reserve fixed explanation space plus
   * the exact lists; no raw entry is shortened to fit a fixed display buffer. */
  size_t capacity = 4096U;
  const RuleStrings *lists[] = {&row->times, &row->orders};
  for (size_t list = 0U; list < 2U; ++list)
    for (size_t i = 0U; i < lists[list]->count; ++i)
      capacity += strlen(lists[list]->values[i]) + 4U;
  char *text = malloc(capacity);
  if (text == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  int count = snprintf(
      text, capacity,
      "Imported Client Portal metadata; not a current TWS capability check.\n"
      "Report timestamp (as supplied): %s\n"
      "Contract %" PRIu32 " | %s | %s | currency %s | %s | requested route %s\n"
      "%s\n"
      "Limit in order-type list: %s\nFOK entry explicitly names LIMIT/LMT: %s\n"
      "Price magnifier: %s | size increment: %s | price increment: %s\n"
      "These numbers are raw metadata, not minimum-fill guarantees or an order-price conversion.\n"
      "Unknown suffixes remain uninterpreted. AON and minimum execution size remain unconfirmed.\n"
      "This file does not establish freshness, session, account permission, acceptance, exchange "
      "enforcement or commission treatment. An FOK fill may contain several executions.\n"
      "No order is submitted, previewed, changed or cancelled by this review.\n",
      identity->generatedUtc, identity->contractId, identity->symbol, identity->localSymbol,
      identity->currency, identity->side == UMI_SIDE_BUY ? "BUY" : "SELL", row->value.exchange,
      UmiIbkrRuleFindingText(row->value.finding),
      row->value.finding == UMI_IBKR_RULE_INCONCLUSIVE ? "inconclusive"
      : row->value.limitOrderAdvertised                ? "yes"
                                                       : "not listed",
      row->value.finding == UMI_IBKR_RULE_INCONCLUSIVE ? "inconclusive"
      : row->value.fokExplicitlyListsLimit             ? "yes; further validation required"
                                                       : "not established",
      row->value.priceMagnifier[0] ? row->value.priceMagnifier : "not supplied",
      row->value.sizeIncrement[0] ? row->value.sizeIncrement : "not supplied",
      row->value.priceIncrement[0] ? row->value.priceIncrement : "not supplied");
  if (count < 0 || (size_t)count >= capacity) {
    free(text);
    return UMI_STATUS_CAPACITY_EXCEEDED;
  }
  size_t at = (size_t)count;
  for (size_t list = 0U; list < 2U; ++list) {
    const char *heading =
        list == 0U ? "\nOriginal time-in-force entries:\n" : "\nOriginal order types:\n";
    size_t length = strlen(heading);
    memcpy(text + at, heading, length);
    at += length;
    for (size_t i = 0U; i < lists[list]->count; ++i) {
      length = strlen(lists[list]->values[i]);
      text[at++] = ' ';
      text[at++] = ' ';
      memcpy(text + at, lists[list]->values[i], length);
      at += length;
      text[at++] = '\n';
    }
  }
  text[at] = '\0';
  *out = text;
  return UMI_STATUS_OK;
}
void UmiIbkrRuleReportFreeText(char *text) { free(text); }
