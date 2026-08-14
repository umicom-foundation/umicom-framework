/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/browser/headless.h
 *
 * PURPOSE:
 *   Provide a deterministic headless browser for tests and server-side workflows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Browser contracts describe navigation and messages without exposing WebKit, Edge, GTK or Qt objects.
 */

#ifndef UMICOM_BROWSER_HEADLESS_H
#define UMICOM_BROWSER_HEADLESS_H
#include "umicom/browser/browser.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_browser_headless_create(UmiBrowserEventSink sink,void *user_data,UmiBrowser *out_browser);
#ifdef __cplusplus
}
#endif
#endif
