/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/rule_report_gtk4.h
 * PURPOSE: Offer a local metadata report panel independent of broker credentials and transport.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_RULE_REPORT_GTK4_H
#define UMICOM_BROKER_CONNECTIVITY_RULE_REPORT_GTK4_H
#include "umicom/broker_connectivity/rule_report.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/** Create a floating native report panel. Import reads a chosen absolute regular
 * file on a worker. The panel owns its immutable report and never opens a broker
 * connection. It discards late results after the path changes or it is unrooted.
 * Call on the GTK thread and append the widget to a live window. */
GtkWidget *UmiIbkrRuleReportGtk4Create(void);
/** Capture a policy identity for comparison. Copies exchange; it is never used
 * as a request destination. A successful comparison grants no trading authority.
 * Call on the GTK thread; output publication may invoke application handlers. */
UmiStatus UmiIbkrRuleReportGtk4SetExpectation(GtkWidget *panel, uint32_t contractId, UmiSide side,
                                              const char *exchange);
/** Retire a captured comparison when policy, quote identity or connection changes.
 * The imported file remains visible as historical, unverified metadata. */
void UmiIbkrRuleReportGtk4ClearExpectation(GtkWidget *panel);
#ifdef __cplusplus
}
#endif
#endif
