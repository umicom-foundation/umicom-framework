/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/rule_report_decode.c
 * PURPOSE: Read only supported metadata from a complete, bounded JSON document.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "rule_report_private.h"
#include "umicom/language_runtime/json_tree.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* Object lookups reject duplicate names through the shared JSON owner. Optional
 * means absent or null, not a permissive conversion from another JSON type. */
static UmiStatus RuleText(const UmiJsonTree *tree, int object, const char *key, char *out,
                          size_t capacity, bool optional) {
  int node;
  UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
  if (optional && status == UMI_STATUS_NOT_FOUND)
    return UMI_STATUS_OK;
  if (status != UMI_STATUS_OK)
    return status;
  if (optional && UmiJsonTreeIsNull(tree, node))
    return UMI_STATUS_OK;
  if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
    return UMI_STATUS_PARSE_ERROR;
  status = UmiJsonTreeText(tree, node, out, capacity);
  if (status != UMI_STATUS_OK)
    return status;
  bool visible = false;
  for (const unsigned char *at = (const unsigned char *)out; *at != 0U; ++at) {
    if (*at < 32U || *at == 127U)
      return UMI_STATUS_PARSE_ERROR;
    if (*at != ' ')
      visible = true;
  }
  return visible || optional ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
static bool RuleUtc(const char *text) {
  size_t length = strlen(text);
  if (length < 20U || text[4] != '-' || text[7] != '-' || text[10] != 'T' || text[13] != ':' ||
      text[16] != ':')
    return false;
  unsigned values[6] = {0};
  const size_t starts[6] = {0U, 5U, 8U, 11U, 14U, 17U}, widths[6] = {4U, 2U, 2U, 2U, 2U, 2U};
  for (size_t i = 0U; i < 6U; ++i)
    for (size_t j = 0U; j < widths[i]; ++j) {
      char c = text[starts[i] + j];
      if (c < '0' || c > '9')
        return false;
      values[i] = values[i] * 10U + (unsigned)(c - '0');
    }
  const unsigned days[] = {31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};
  unsigned year = values[0], month = values[1], day = values[2];
  if (year == 0U || month == 0U || month > 12U || day == 0U || values[3] > 23U || values[4] > 59U ||
      values[5] > 59U)
    return false;
  unsigned maximum = days[month - 1U];
  if (month == 2U && year % 4U == 0U && (year % 100U != 0U || year % 400U == 0U))
    ++maximum;
  if (day > maximum)
    return false;
  size_t at = 19U;
  if (text[at] == '.') {
    size_t begin = ++at;
    while (text[at] >= '0' && text[at] <= '9')
      ++at;
    if (at == begin || at - begin > 6U)
      return false;
  }
  return strcmp(text + at, "Z") == 0 || strcmp(text + at, "+00:00") == 0;
}
static UmiStatus RuleNumber(const UmiJsonTree *tree, int object, const char *key, char out[64]) {
  int node;
  UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
  if (status == UMI_STATUS_NOT_FOUND)
    return UMI_STATUS_OK;
  if (status != UMI_STATUS_OK)
    return status;
  if (UmiJsonTreeIsNull(tree, node))
    return UMI_STATUS_OK;
  const char *bytes;
  size_t size;
  status = UmiJsonTreeSourceSpan(tree, node, &bytes, &size);
  if (status != UMI_STATUS_OK)
    return status;
  if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE || size == 0U ||
      (bytes[0] != '-' && (bytes[0] < '0' || bytes[0] > '9')))
    return UMI_STATUS_PARSE_ERROR;
  if (size >= 64U)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  memcpy(out, bytes, size);
  out[size] = '\0';
  return UMI_STATUS_OK;
}
static UmiStatus RuleList(const UmiJsonTree *tree, int object, const char *key, RuleStrings *out) {
  int array;
  UmiStatus status = UmiJsonTreeMember(tree, object, key, &array);
  if (status == UMI_STATUS_NOT_FOUND)
    return UMI_STATUS_OK;
  if (status != UMI_STATUS_OK)
    return status;
  if (UmiJsonTreeIsNull(tree, array))
    return UMI_STATUS_OK;
  if (UmiJsonTreeKind(tree, array) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
    return UMI_STATUS_PARSE_ERROR;
  if (UmiJsonTreeCount(tree, array) > UMI_IBKR_RULE_REPORT_MAX_ENTRIES)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  for (int node = UmiJsonTreeFirst(tree, array); node >= 0; node = UmiJsonTreeNext(tree, node)) {
    char text[UMI_IBKR_RULE_REPORT_ENTRY_BYTES + 1U];
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
      return UMI_STATUS_PARSE_ERROR;
    status = UmiJsonTreeText(tree, node, text, sizeof text);
    if (status != UMI_STATUS_OK)
      return status;
    bool visible = false;
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p) {
      if (*p < 32U || *p == 127U)
        return UMI_STATUS_PARSE_ERROR;
      if (*p != ' ')
        visible = true;
    }
    if (!visible)
      return UMI_STATUS_PARSE_ERROR;
    size_t length = strlen(text) + 1U;
    char *copy = malloc(length);
    if (copy == NULL)
      return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, text, length);
    out->values[out->count++] = copy;
  }
  return UMI_STATUS_OK;
}
static UmiStatus RuleDecodeRow(const UmiJsonTree *tree, int object, RuleRow *row) {
  if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    return UMI_STATUS_PARSE_ERROR;
  UmiStatus status = RuleText(tree, object, "requested_route", row->value.exchange,
                              sizeof row->value.exchange, false);
  if (status != UMI_STATUS_OK)
    return status;
  for (const unsigned char *p = (const unsigned char *)row->value.exchange; *p != 0U; ++p)
    if (*p < 33U || *p > 126U)
      return UMI_STATUS_PARSE_ERROR;
  char state[32] = {0};
  status = RuleText(tree, object, "status", state, sizeof state, false);
  if (status != UMI_STATUS_OK)
    return status;
  bool received = strcmp(state, "metadata_received") == 0;
  if (!received && strcmp(state, "inconclusive") != 0)
    return UMI_STATUS_PARSE_ERROR;
  status = RuleList(tree, object, "time_in_force_entries", &row->times);
  if (status == UMI_STATUS_OK)
    status = RuleList(tree, object, "order_types", &row->orders);
  if (status == UMI_STATUS_OK)
    status = RuleNumber(tree, object, "priceMagnifier", row->value.priceMagnifier);
  if (status == UMI_STATUS_OK)
    status = RuleNumber(tree, object, "sizeIncrement", row->value.sizeIncrement);
  if (status == UMI_STATUS_OK)
    status = RuleNumber(tree, object, "increment", row->value.priceIncrement);
  if (status == UMI_STATUS_OK)
    RuleRowAnalyse(row, received);
  return status;
}
static UmiStatus RuleDecode(const UmiJsonTree *tree, UmiIbkrRuleReport *report) {
  if (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    return UMI_STATUS_PARSE_ERROR;
  UmiIbkrRuleReportIdentity *identity = &report->identity;
  UmiStatus status = RuleText(tree, 0, "generated_utc", identity->generatedUtc,
                              sizeof identity->generatedUtc, false);
  if (status != UMI_STATUS_OK)
    return status;
  if (!RuleUtc(identity->generatedUtc))
    return UMI_STATUS_PARSE_ERROR;
  char side[8] = {0};
  status = RuleText(tree, 0, "side", side, sizeof side, false);
  if (status != UMI_STATUS_OK)
    return status;
  if (strcmp(side, "BUY") == 0)
    identity->side = UMI_SIDE_BUY;
  else if (strcmp(side, "SELL") == 0)
    identity->side = UMI_SIDE_SELL;
  else
    return UMI_STATUS_PARSE_ERROR;
  int contract, node;
  status = UmiJsonTreeMember(tree, 0, "contract", &contract);
  if (status != UMI_STATUS_OK)
    return status;
  if (UmiJsonTreeKind(tree, contract) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    return UMI_STATUS_PARSE_ERROR;
  status = UmiJsonTreeMember(tree, contract, "conid", &node);
  if (status != UMI_STATUS_OK)
    return status;
  int64_t id;
  status = UmiJsonTreeInteger(tree, node, &id);
  if (status != UMI_STATUS_OK)
    return status;
  if (id <= 0 || id > INT_MAX)
    return UMI_STATUS_INVALID_ARGUMENT;
  identity->contractId = (uint32_t)id;
  char instrument[16] = {0};
  status = RuleText(tree, contract, "instrument_type", instrument, sizeof instrument, false);
  if (status != UMI_STATUS_OK)
    return status;
  if (strcmp(instrument, "STK") != 0)
    return UMI_STATUS_NOT_IMPLEMENTED;
  status = RuleText(tree, contract, "symbol", identity->symbol, sizeof identity->symbol, true);
  if (status == UMI_STATUS_OK)
    status = RuleText(tree, contract, "local_symbol", identity->localSymbol,
                      sizeof identity->localSymbol, true);
  if (status == UMI_STATUS_OK)
    status =
        RuleText(tree, contract, "currency", identity->currency, sizeof identity->currency, true);
  if (status != UMI_STATUS_OK)
    return status;
  int array;
  status = UmiJsonTreeMember(tree, 0, "routes", &array);
  if (status != UMI_STATUS_OK)
    return status;
  if (UmiJsonTreeKind(tree, array) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
    return UMI_STATUS_PARSE_ERROR;
  size_t count = UmiJsonTreeCount(tree, array);
  if (count == 0U)
    return UMI_STATUS_PARSE_ERROR;
  if (count > UMI_IBKR_RULE_REPORT_MAX_ROUTES)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  for (int row = UmiJsonTreeFirst(tree, array); row >= 0; row = UmiJsonTreeNext(tree, row)) {
    RuleRow *current = &report->rows[identity->routeCount];
    status = RuleDecodeRow(tree, row, current);
    if (status != UMI_STATUS_OK)
      return status;
    for (size_t previous = 0U; previous < identity->routeCount; ++previous)
      if (strcmp(report->rows[previous].value.exchange, current->value.exchange) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    ++identity->routeCount;
  }
  return UMI_STATUS_OK;
}
UmiStatus UmiIbkrRuleReportCreate(const void *bytes, size_t length, UmiIbkrRuleReport **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (bytes == NULL || length == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (length > UMI_IBKR_RULE_REPORT_MAX_BYTES)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiJsonTreeLimits limits = {UMI_IBKR_RULE_REPORT_MAX_BYTES, 65536U, 16U};
  UmiJsonTree *tree = NULL;
  UmiStatus status = UmiJsonTreeCreate(bytes, length, &limits, NULL, &tree);
  if (status != UMI_STATUS_OK)
    return status;
  UmiIbkrRuleReport *report = calloc(1U, sizeof *report);
  if (report == NULL)
    status = UMI_STATUS_OUT_OF_MEMORY;
  else
    status = RuleDecode(tree, report);
  /* Discard the complete input tree before returning. Only selected contract
   * metadata survives, so account IDs or commentary in extensions cannot leak
   * through a later report view or export. */
  UmiJsonTreeDestroy(tree);
  if (status != UMI_STATUS_OK)
    UmiIbkrRuleReportDestroy(report);
  else
    *out = report;
  return status;
}
