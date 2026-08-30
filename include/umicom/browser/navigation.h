/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/browser/navigation.h
 *
 * PURPOSE:
 *   Maintain deterministic browser navigation history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Browser contracts describe navigation and messages without exposing WebKit, Edge, GTK or Qt objects.
 */

#ifndef UMICOM_BROWSER_NAVIGATION_H
#define UMICOM_BROWSER_NAVIGATION_H
#include "umicom/browser/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBrowserHistory { char entries[UMI_BROWSER_HISTORY_CAPACITY][UMI_BROWSER_URL_CAPACITY]; size_t count; size_t current; } UmiBrowserHistory;
void umi_browser_history_init(UmiBrowserHistory *history);
UmiStatus umi_browser_history_visit(UmiBrowserHistory *history,const char *url);
const char *umi_browser_history_back(UmiBrowserHistory *history);
const char *umi_browser_history_forward(UmiBrowserHistory *history);
const char *umi_browser_history_current(const UmiBrowserHistory *history);
#ifdef __cplusplus
}
#endif
#endif
