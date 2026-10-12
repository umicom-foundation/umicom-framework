/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/rule_report_private.h
 * PURPOSE: Keep owned route strings private to the immutable report.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_RULE_REPORT_PRIVATE_H
#define UMICOM_RULE_REPORT_PRIVATE_H
#include "umicom/broker_connectivity/rule_report.h"
typedef struct RuleStrings {
  size_t count;
  char *values[UMI_IBKR_RULE_REPORT_MAX_ENTRIES];
} RuleStrings;
typedef struct RuleRow {
  UmiIbkrRuleRoute value;
  RuleStrings times, orders;
} RuleRow;
struct UmiIbkrRuleReport {
  UmiIbkrRuleReportIdentity identity;
  RuleRow rows[UMI_IBKR_RULE_REPORT_MAX_ROUTES];
};
/* Analyse whole ASCII tokens only. Unknown provider flags remain raw evidence. */
bool RuleTokenEqual(const char *begin, size_t length, const char *expected);
void RuleRowAnalyse(RuleRow *row, bool metadataReceived);
#endif
