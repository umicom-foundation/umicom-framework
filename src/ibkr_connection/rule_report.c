/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/rule_report.c
 * PURPOSE: Own bounded rule reports and compare their exact contract, route and side.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "rule_report_private.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* ASCII comparisons do not depend on the user's locale. Trimming belongs only
 * to descriptive provider tokens, never contract or execution-route identity. */
bool RuleTokenEqual(const char *begin, size_t length, const char *expected) {
  while (length != 0U && *begin == ' ') {
    ++begin;
    --length;
  }
  while (length != 0U && begin[length - 1U] == ' ')
    --length;
  if (strlen(expected) != length)
    return false;
  for (size_t i = 0U; i < length; ++i) {
    unsigned char c = (unsigned char)begin[i];
    if (c >= 'a' && c <= 'z')
      c = (unsigned char)(c - 'a' + 'A');
    if (c != (unsigned char)expected[i])
      return false;
  }
  return true;
}
void RuleRowAnalyse(RuleRow *row, bool metadataReceived) {
  row->value.timeInForceCount = row->times.count;
  row->value.orderTypeCount = row->orders.count;
  if (!metadataReceived || row->times.count == 0U || row->orders.count == 0U)
    return;
  row->value.finding = UMI_IBKR_RULE_FOK_NOT_ADVERTISED;
  for (size_t i = 0U; i < row->orders.count; ++i) {
    const char *text = row->orders.values[i];
    if (RuleTokenEqual(text, strlen(text), "LIMIT") || RuleTokenEqual(text, strlen(text), "LMT"))
      row->value.limitOrderAdvertised = true;
  }
  for (size_t i = 0U; i < row->times.count; ++i) {
    const char *text = row->times.values[i];
    const char *slash = strchr(text, '/');
    size_t prefix = slash != NULL ? (size_t)(slash - text) : strlen(text);
    if (!RuleTokenEqual(text, prefix, "FOK"))
      continue;
    row->value.finding = UMI_IBKR_RULE_FOK_ADVERTISED;
    /* Seeing FOK/o,a does not establish what either flag means. Only an
     * explicitly named LIMIT or LMT token records that narrower evidence. */
    if (slash != NULL) {
      const char *at = slash + 1;
      while (*at != '\0') {
        const char *comma = strchr(at, ',');
        size_t length = comma != NULL ? (size_t)(comma - at) : strlen(at);
        if (RuleTokenEqual(at, length, "LIMIT") || RuleTokenEqual(at, length, "LMT"))
          row->value.fokExplicitlyListsLimit = true;
        if (comma == NULL)
          break;
        at = comma + 1;
      }
    }
  }
}
void UmiIbkrRuleReportDestroy(UmiIbkrRuleReport *report) {
  if (report == NULL)
    return;
  /* Walk every slot, including a partly decoded row, so parse failure has the
   * same ownership cleanup as normal destruction. calloc initializes slots. */
  for (size_t i = 0U; i < UMI_IBKR_RULE_REPORT_MAX_ROUTES; ++i) {
    for (size_t j = 0U; j < report->rows[i].times.count; ++j)
      free(report->rows[i].times.values[j]);
    for (size_t j = 0U; j < report->rows[i].orders.count; ++j)
      free(report->rows[i].orders.values[j]);
  }
  free(report);
}
UmiStatus UmiIbkrRuleReportRead(const UmiIbkrRuleReport *report, UmiIbkrRuleReportIdentity *out) {
  if (report == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = report->identity;
  return UMI_STATUS_OK;
}
UmiStatus UmiIbkrRuleReportRouteAt(const UmiIbkrRuleReport *report, size_t route,
                                   UmiIbkrRuleRoute *out) {
  if (report == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (route >= report->identity.routeCount)
    return UMI_STATUS_NOT_FOUND;
  *out = report->rows[route].value;
  return UMI_STATUS_OK;
}
UmiStatus UmiIbkrRuleReportEntryAt(const UmiIbkrRuleReport *report, size_t route, bool timeInForce,
                                   size_t entry, char *out, size_t capacity) {
  if (report == NULL || out == NULL || capacity == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (route >= report->identity.routeCount)
    return UMI_STATUS_NOT_FOUND;
  const RuleStrings *list = timeInForce ? &report->rows[route].times : &report->rows[route].orders;
  if (entry >= list->count)
    return UMI_STATUS_NOT_FOUND;
  size_t length = strlen(list->values[entry]);
  if (length >= capacity)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  memcpy(out, list->values[entry], length + 1U);
  return UMI_STATUS_OK;
}
UmiStatus UmiIbkrRuleReportCompare(const UmiIbkrRuleReport *report, size_t route,
                                   uint32_t contractId, UmiSide side, const char *exchange,
                                   UmiIbkrRuleBinding *out) {
  if (report == NULL || out == NULL || contractId == 0U || contractId > INT_MAX ||
      (side != UMI_SIDE_BUY && side != UMI_SIDE_SELL) || exchange == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = 0U;
  while (length < 64U && exchange[length] != '\0') {
    if ((unsigned char)exchange[length] < 33U || (unsigned char)exchange[length] > 126U)
      return UMI_STATUS_INVALID_ARGUMENT;
    ++length;
  }
  if (length == 0U || length == 64U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (route >= report->identity.routeCount)
    return UMI_STATUS_NOT_FOUND;
  UmiIbkrRuleBinding binding = UMI_IBKR_RULE_BINDING_MATCH;
  if (report->identity.contractId != contractId)
    binding = UMI_IBKR_RULE_BINDING_CONTRACT;
  else if (report->identity.side != side)
    binding = UMI_IBKR_RULE_BINDING_SIDE;
  else if (strcmp(report->rows[route].value.exchange, exchange) != 0)
    binding = UMI_IBKR_RULE_BINDING_ROUTE;
  *out = binding;
  return UMI_STATUS_OK;
}
const char *UmiIbkrRuleFindingText(UmiIbkrRuleFinding finding) {
  if (finding == UMI_IBKR_RULE_FOK_ADVERTISED)
    return "FOK advertised; acceptance and enforcement unconfirmed";
  if (finding == UMI_IBKR_RULE_FOK_NOT_ADVERTISED)
    return "FOK not advertised in this route's returned list";
  return "Inconclusive: usable route metadata is missing";
}
const char *UmiIbkrRuleBindingText(UmiIbkrRuleBinding binding) {
  switch (binding) {
  case UMI_IBKR_RULE_BINDING_MATCH:
    return "Contract, side and route match the captured policy; support remains unconfirmed.";
  case UMI_IBKR_RULE_BINDING_CONTRACT:
    return "Different contract: this report does not describe the captured policy.";
  case UMI_IBKR_RULE_BINDING_SIDE:
    return "Different side: Buy rules cannot stand in for Sell rules.";
  case UMI_IBKR_RULE_BINDING_ROUTE:
    return "Different route: SMART and individual venues are not interchangeable.";
  default:
    return "No valid comparison with a captured policy.";
  }
}
