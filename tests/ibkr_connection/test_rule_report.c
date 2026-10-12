/*-----------------------------------------------------------------------------
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 *
 * LICENCE: MIT
 *
 * Umicom Framework
 * File: tests/ibkr_connection/test_rule_report.c
 * PURPOSE: Exercise raw-evidence interpretation, bounded import and identity comparisons without a
 * broker. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "rule_report_fixture.h"
#include "umicom/broker_connectivity/rule_report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(expression)                                                                          \
  do {                                                                                             \
    if (!(expression)) {                                                                           \
      fprintf(stderr, "%d: %s\n", __LINE__, #expression);                                          \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
/* Replace one fixture fragment, preserving JSON elsewhere so each rejection
 * represents one deliberate input defect rather than an unrelated parse error. */
static char *Replace(const char *text, const char *old, const char *replacement) {
  const char *at = strstr(text, old);
  if (at == NULL)
    return NULL;
  size_t prefix = (size_t)(at - text), suffix = strlen(at + strlen(old));
  char *result = malloc(prefix + strlen(replacement) + suffix + 1U);
  if (result == NULL)
    return NULL;
  memcpy(result, text, prefix);
  memcpy(result + prefix, replacement, strlen(replacement));
  memcpy(result + prefix + strlen(replacement), at + strlen(old), suffix + 1U);
  return result;
}
static char *Repeated(const char *entry, size_t count) {
  size_t size = (strlen(entry) + 1U) * count + 3U;
  char *text = calloc(size, 1U);
  if (text == NULL)
    return NULL;
  strcat(text, "[");
  for (size_t i = 0U; i < count; ++i) {
    if (i != 0U)
      strcat(text, ",");
    strcat(text, entry);
  }
  strcat(text, "]");
  return text;
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  const char *old = NULL, *replacement = NULL;
  char *extra = NULL;
  bool reject = false;
  UmiIbkrRuleFinding finding = UMI_IBKR_RULE_FOK_ADVERTISED;
  if (!strcmp(name, "missing")) {
    old = "\"time_in_force_entries\"";
    replacement = "\"omitted_tifs\"";
    finding = UMI_IBKR_RULE_INCONCLUSIVE;
  } else if (!strcmp(name, "empty")) {
    old = "[\"limit\",\"market\"]";
    replacement = "[]";
    finding = UMI_IBKR_RULE_INCONCLUSIVE;
  } else if (!strcmp(name, "inconclusive")) {
    old = "metadata_received";
    replacement = "inconclusive";
    finding = UMI_IBKR_RULE_INCONCLUSIVE;
  } else if (!strcmp(name, "spoof")) {
    old = "\"FOK/LIMIT,a\"";
    replacement = "\"GTC/o,a\"";
    finding = UMI_IBKR_RULE_FOK_NOT_ADVERTISED;
  } else if (!strcmp(name, "flags")) {
    old = "FOK/LIMIT,a";
    replacement = "FOK/o,a";
  } else if (!strcmp(name, "tokens")) {
    old = "FOK/LIMIT,a";
    replacement = " Fok / limit ,a";
  } else if (!strcmp(name, "sell")) {
    old = "BUY";
    replacement = "SELL";
  } else if (!strcmp(name, "duplicate")) {
    old = "\"side\":\"BUY\"";
    replacement = "\"side\":\"BUY\",\"side\":\"SELL\"";
    reject = true;
  } else if (!strcmp(name, "duplicate-escaped")) {
    old = "\"side\":\"BUY\"";
    replacement = "\"side\":\"BUY\",\"s\\u0069de\":\"SELL\"";
    reject = true;
  } else if (!strcmp(name, "invalid-json")) {
    old = "\"conid\":123";
    replacement = "\"conid\":01";
    reject = true;
  } else if (!strcmp(name, "unicode")) {
    old = "\"symbol\":\"EXAMPLE\"";
    replacement = "\"symbol\":\"caf\\u00e9\"";
  } else if (!strcmp(name, "control")) {
    old = "FOK/LIMIT,a";
    replacement = "FOK/LI\\nMIT";
    reject = true;
  } else if (!strcmp(name, "route-space")) {
    old = "SMART";
    replacement = " SMART";
    reject = true;
  } else if (!strcmp(name, "stock-only")) {
    old = "STK";
    replacement = "OPT";
    reject = true;
  } else if (!strcmp(name, "bad-side")) {
    old = "BUY";
    replacement = "SHORT";
    reject = true;
  } else if (!strcmp(name, "bad-id")) {
    old = "\"conid\":123";
    replacement = "\"conid\":0";
    reject = true;
  } else if (!strcmp(name, "fractional-id")) {
    old = "\"conid\":123";
    replacement = "\"conid\":123.0";
    reject = true;
  } else if (!strcmp(name, "large-id")) {
    old = "\"conid\":123";
    replacement = "\"conid\":2147483648";
    reject = true;
  } else if (!strcmp(name, "timestamp")) {
    old = "+00:00";
    replacement = "+01:00";
    reject = true;
  } else if (!strcmp(name, "leap-date")) {
    old = "2026-10-12";
    replacement = "2024-02-29";
  } else if (!strcmp(name, "bad-date")) {
    old = "2026-10-12";
    replacement = "2026-02-29";
    reject = true;
  } else if (!strcmp(name, "bad-number")) {
    old = "\"priceMagnifier\":100";
    replacement = "\"priceMagnifier\":\"100\"";
    reject = true;
  } else if (!strcmp(name, "bool-number")) {
    old = "\"priceMagnifier\":100";
    replacement = "\"priceMagnifier\":true";
    reject = true;
  } else if (!strcmp(name, "raw-numbers")) {
    old = "\"increment\":0.01";
    replacement = "\"increment\":1e-2";
  } else if (!strcmp(name, "missing-identity")) {
    old = "\"contract\"";
    replacement = "\"other\"";
    reject = true;
  } else if (!strcmp(name, "wrong-list")) {
    old = "[\"limit\",\"market\"]";
    replacement = "[\"limit\",false]";
    reject = true;
  } else if (!strcmp(name, "long-entry")) {
    extra = malloc(UMI_IBKR_RULE_REPORT_ENTRY_BYTES + 2U);
    CHECK(extra);
    memset(extra, 'X', UMI_IBKR_RULE_REPORT_ENTRY_BYTES + 1U);
    extra[UMI_IBKR_RULE_REPORT_ENTRY_BYTES + 1U] = '\0';
    old = "FOK/LIMIT,a";
    replacement = extra;
    reject = true;
  } else if (!strcmp(name, "too-many-entries")) {
    extra = Repeated("\"DAY\"", 65U);
    CHECK(extra);
    old = "[\"limit\",\"market\"]";
    replacement = extra;
    reject = true;
  } else if (!strcmp(name, "depth")) {
    old = "\"increment\":0.01";
    replacement = "\"increment\":0.01,\"extension\":[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]";
    reject = true;
  } else if (!strcmp(name, "no-account-output")) {
    old = "\"sizeIncrement\":1";
    replacement = "\"sizeIncrement\":1,\"canTradeAcctIds\":[\"PRIVATE-ACCOUNT\"],\"reason\":"
                  "\"PRIVATE-SESSION\"";
  } else if (!strcmp(name, "duplicate-route") || !strcmp(name, "too-many-routes")) {
    const char *start = strstr(RULE_REPORT_FIXTURE, "[{\"requested_route\"");
    size_t length = strlen(start) - 1U;
    char *original = malloc(length + 1U);
    CHECK(original);
    memcpy(original, start, length);
    original[length] = '\0';
    const char *entry = "{\"requested_route\":\"SMART\",\"status\":\"inconclusive\"}";
    extra = Repeated(entry, !strcmp(name, "duplicate-route") ? 2U : 101U);
    CHECK(extra);
    char *json = Replace(RULE_REPORT_FIXTURE, original, extra);
    free(original);
    free(extra);
    CHECK(json);
    UmiIbkrRuleReport *report = NULL;
    CHECK(UmiIbkrRuleReportCreate(json, strlen(json), &report) != UMI_STATUS_OK && report == NULL);
    free(json);
    return 0;
  } else if (!strcmp(name, "byte-limit")) {
    UmiIbkrRuleReport *report = NULL;
    CHECK(UmiIbkrRuleReportCreate("{}", (size_t)UMI_IBKR_RULE_REPORT_MAX_BYTES + 1U, &report) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(report == NULL);
    return 0;
  }
  char *json = old != NULL ? Replace(RULE_REPORT_FIXTURE, old, replacement)
                           : Replace(RULE_REPORT_FIXTURE, "BUY", "BUY");
  free(extra);
  CHECK(json);
  /* Imported summary booleans are deliberately inconsistent with the raw TIF
   * list in this fixture. Only the latter can influence the interpretation. */
  if (!strcmp(name, "spoof")) {
    char *changed =
        Replace(json, "\"sizeIncrement\":1",
                "\"sizeIncrement\":1,\"fok_advertised\":true,\"fok_explicitly_lists_limit\":true");
    free(json);
    json = changed;
    CHECK(json);
  }
  UmiIbkrRuleReport *report = NULL;
  UmiStatus status = UmiIbkrRuleReportCreate(json, strlen(json), &report);
  if (reject) {
    CHECK(status != UMI_STATUS_OK && report == NULL);
    free(json);
    return 0;
  }
  CHECK(status == UMI_STATUS_OK && report);
  if (!strcmp(name, "ownership"))
    memset(json, 'X', strlen(json));
  free(json);
  UmiIbkrRuleReportIdentity identity;
  UmiIbkrRuleRoute route;
  CHECK(UmiIbkrRuleReportRead(report, &identity) == UMI_STATUS_OK);
  CHECK(UmiIbkrRuleReportRouteAt(report, 0U, &route) == UMI_STATUS_OK);
  CHECK(identity.contractId == 123U && identity.routeCount == 1U &&
        !strcmp(route.exchange, "SMART"));
  CHECK(route.finding == finding);
  if (!strcmp(name, "flags") || !strcmp(name, "spoof"))
    CHECK(!route.fokExplicitlyListsLimit);
  if (!strcmp(name, "tokens"))
    CHECK(route.fokExplicitlyListsLimit);
  if (!strcmp(name, "sell"))
    CHECK(identity.side == UMI_SIDE_SELL);
  if (!strcmp(name, "unicode"))
    CHECK(!strcmp(identity.symbol, "caf\xc3\xa9"));
  if (!strcmp(name, "raw-numbers"))
    CHECK(!strcmp(route.priceIncrement, "1e-2"));
  if (!strcmp(name, "values"))
    CHECK(route.fokExplicitlyListsLimit && route.limitOrderAdvertised &&
          !strcmp(route.priceMagnifier, "100"));
  if (!strcmp(name, "entry-bounds") || !strcmp(name, "output-unchanged")) {
    char text[5] = "keep";
    CHECK(UmiIbkrRuleReportEntryAt(report, 0U, true, 1U, text, sizeof text) ==
              UMI_STATUS_CAPACITY_EXCEEDED &&
          !strcmp(text, "keep"));
    CHECK(UmiIbkrRuleReportEntryAt(report, 0U, true, 64U, text, sizeof text) ==
              UMI_STATUS_NOT_FOUND &&
          !strcmp(text, "keep"));
    UmiIbkrRuleRoute previous = route;
    CHECK(UmiIbkrRuleReportRouteAt(report, 1U, &route) == UMI_STATUS_NOT_FOUND);
    CHECK(!memcmp(&route, &previous, sizeof route));
  }
  UmiIbkrRuleBinding binding = UMI_IBKR_RULE_BINDING_ROUTE;
  uint32_t contract = !strcmp(name, "contract-mismatch") ? 124U : 123U;
  UmiSide side = !strcmp(name, "side-mismatch") ? UMI_SIDE_SELL : identity.side;
  const char *exchange = !strcmp(name, "route-mismatch") ? "LSE"
                         : !strcmp(name, "route-case")   ? "smart"
                                                         : "SMART";
  CHECK(UmiIbkrRuleReportCompare(report, 0U, contract, side, exchange, &binding) == UMI_STATUS_OK);
  CHECK(binding == (!strcmp(name, "contract-mismatch") ? UMI_IBKR_RULE_BINDING_CONTRACT
                    : !strcmp(name, "side-mismatch")   ? UMI_IBKR_RULE_BINDING_SIDE
                    : (!strcmp(name, "route-mismatch") || !strcmp(name, "route-case"))
                        ? UMI_IBKR_RULE_BINDING_ROUTE
                        : UMI_IBKR_RULE_BINDING_MATCH));
  char *text = NULL;
  CHECK(UmiIbkrRuleReportDescribe(report, 0U, &text) == UMI_STATUS_OK && text);
  CHECK(strstr(text, "not a current TWS") && strstr(text, "AON") && strstr(text, "No order"));
  CHECK(!strstr(text, "PRIVATE-ACCOUNT") && !strstr(text, "PRIVATE-SESSION"));
  UmiIbkrRuleReportFreeText(text);
  UmiIbkrRuleReportDestroy(report);
  return 0;
}
