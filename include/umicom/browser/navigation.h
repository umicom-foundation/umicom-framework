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

/*
 * Browser contracts describe navigation and messages without exposing WebKit, Edge, GTK or Qt objects.
 */

#ifndef UMICOM_BROWSER_NAVIGATION_H
#define UMICOM_BROWSER_NAVIGATION_H
#include "umicom/browser/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the browser history data shared with callers of this public contract.
 */
typedef struct UmiBrowserHistory { char entries[UMI_BROWSER_HISTORY_CAPACITY][UMI_BROWSER_URL_CAPACITY]; size_t count; size_t current; } UmiBrowserHistory;
/**
 * Initialise browser history from caller-provided values so later operations receive a
 * known state.
 */
void umi_browser_history_init(UmiBrowserHistory *history);
/**
 * Provide the browser history visit operation used by this module and its client
 * applications.
 */
UmiStatus umi_browser_history_visit(UmiBrowserHistory *history,const char *url);
/**
 * Provide the browser history back operation used by this module and its client
 * applications.
 */
const char *umi_browser_history_back(UmiBrowserHistory *history);
/**
 * Provide the browser history forward operation used by this module and its client
 * applications.
 */
const char *umi_browser_history_forward(UmiBrowserHistory *history);
/**
 * Provide the browser history current operation used by this module and its client
 * applications.
 */
const char *umi_browser_history_current(const UmiBrowserHistory *history);
#ifdef __cplusplus
}
#endif
#endif
