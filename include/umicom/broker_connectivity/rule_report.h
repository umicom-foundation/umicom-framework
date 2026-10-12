/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/rule_report.h
 * PURPOSE: Review saved Client Portal route metadata without granting trading authority.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_RULE_REPORT_H
#define UMICOM_BROKER_CONNECTIVITY_RULE_REPORT_H
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_IBKR_RULE_REPORT_MAX_BYTES (4U * 1024U * 1024U)
#define UMI_IBKR_RULE_REPORT_MAX_ROUTES 100U
#define UMI_IBKR_RULE_REPORT_MAX_ENTRIES 64U
#define UMI_IBKR_RULE_REPORT_ENTRY_BYTES 4096U
typedef struct UmiIbkrRuleReport UmiIbkrRuleReport;
/** A local report describes one stock and one side. It carries no account,
 * authenticated session, freshness claim or permission to submit an order. */
typedef struct UmiIbkrRuleReportIdentity {
  uint32_t contractId;
  UmiSide side;
  char generatedUtc[40], symbol[128], localSymbol[128], currency[16];
  size_t routeCount;
} UmiIbkrRuleReportIdentity;
typedef enum UmiIbkrRuleFinding {
  UMI_IBKR_RULE_INCONCLUSIVE = 0,
  UMI_IBKR_RULE_FOK_NOT_ADVERTISED,
  UMI_IBKR_RULE_FOK_ADVERTISED
} UmiIbkrRuleFinding;
/** Raw number spellings are display evidence. Never use these fields to infer
 * minimum fills or convert an order's price between pounds and pence. Empty
 * numeric text means the report omitted that value or supplied JSON null. */
typedef struct UmiIbkrRuleRoute {
  char exchange[64];
  UmiIbkrRuleFinding finding;
  bool limitOrderAdvertised, fokExplicitlyListsLimit;
  size_t timeInForceCount, orderTypeCount;
  char priceMagnifier[64], sizeIncrement[64], priceIncrement[64];
} UmiIbkrRuleRoute;
typedef enum UmiIbkrRuleBinding {
  UMI_IBKR_RULE_BINDING_MATCH = 0,
  UMI_IBKR_RULE_BINDING_CONTRACT,
  UMI_IBKR_RULE_BINDING_SIDE,
  UMI_IBKR_RULE_BINDING_ROUTE
} UmiIbkrRuleBinding;
/**
 * @brief Import the saved report format produced by an IBKR route-rule checker.
 * @param bytes Borrowed UTF-8 JSON; never executed or used for network access.
 * @param length Exact byte count; at most UMI_IBKR_RULE_REPORT_MAX_BYTES.
 * @param out Receives an owned immutable report, or NULL on failure.
 * @return OK only for a complete supported document.
 *
 * Require a positive contract ID, STK instrument, BUY/SELL, UTC generation
 * timestamp and 1..100 distinct routes. Each list holds at most 64 nonempty
 * strings of at most 4096 bytes. Duplicate supported fields, control characters,
 * malformed UTF-8/JSON and invalid field types are rejected, never truncated.
 * Missing rule lists and explicitly inconclusive routes stay inconclusive.
 * Recompute findings from the raw lists; imported booleans and commentary do
 * not certify support. Unknown fields are validated as JSON, then discarded.
 * The parser uses at most 65,536 JSON nodes and 16 nested containers.
 */
UmiStatus UmiIbkrRuleReportCreate(const void *bytes, size_t length, UmiIbkrRuleReport **out);
/** Release the copied metadata. NULL is accepted; no provider work is performed. */
void UmiIbkrRuleReportDestroy(UmiIbkrRuleReport *report);
/** Copy identity without lending a pointer to report-owned memory. */
UmiStatus UmiIbkrRuleReportRead(const UmiIbkrRuleReport *report, UmiIbkrRuleReportIdentity *out);
/** Copy one route. Invalid indices leave output unchanged. */
UmiStatus UmiIbkrRuleReportRouteAt(const UmiIbkrRuleReport *report, size_t route,
                                   UmiIbkrRuleRoute *out);
/** Copy a decoded original TIF or order-type entry, preserving unknown suffixes.
 * False selects order types; true selects TIF. Output is unchanged on failure. */
UmiStatus UmiIbkrRuleReportEntryAt(const UmiIbkrRuleReport *report, size_t route, bool timeInForce,
                                   size_t entry, char *out, size_t capacity);
/** Compare exact contract, side and route, in that order. MATCH only establishes
 * identity equality with a saved report; it does not authorize an instruction.
 * Exchange matching is case-sensitive and never substitutes SMART for a venue. */
UmiStatus UmiIbkrRuleReportCompare(const UmiIbkrRuleReport *report, size_t route,
                                   uint32_t contractId, UmiSide side, const char *exchange,
                                   UmiIbkrRuleBinding *out);
/** Return a fixed explanation; unknown enum values receive an inconclusive label. */
const char *UmiIbkrRuleFindingText(UmiIbkrRuleFinding finding);
/** Return a fixed explanation for an identity comparison. */
const char *UmiIbkrRuleBindingText(UmiIbkrRuleBinding binding);
/** Render one route as plain UTF-8 text, including original entries and limits.
 * Caller frees a successful allocation with UmiIbkrRuleReportFreeText.
 * Failure clears out. Account fields and arbitrary imported commentary are not
 * retained or displayed. The output is text, never HTML or executable content. */
UmiStatus UmiIbkrRuleReportDescribe(const UmiIbkrRuleReport *report, size_t route, char **out);
/** Release text returned by Describe. NULL is accepted. */
void UmiIbkrRuleReportFreeText(char *text);
#ifdef __cplusplus
}
#endif
#endif
