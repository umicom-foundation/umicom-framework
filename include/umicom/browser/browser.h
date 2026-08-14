/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/browser/browser.h
 *
 * PURPOSE:
 *   Define the stable embedded-browser function-table contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Browser contracts describe navigation and messages without exposing WebKit, Edge, GTK or Qt objects.
 */

#ifndef UMICOM_BROWSER_BROWSER_H
#define UMICOM_BROWSER_BROWSER_H
#include "umicom/browser/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBrowser {
    void *instance;
    UmiStatus (*navigate)(void *instance,const char *url);
    UmiStatus (*back)(void *instance);
    UmiStatus (*forward)(void *instance);
    UmiStatus (*reload)(void *instance);
    UmiStatus (*post_message)(void *instance,const char *message);
    const char *(*current_url)(const void *instance);
    void (*destroy)(void *instance);
} UmiBrowser;
UmiStatus umi_browser_validate(const UmiBrowser *browser);
#ifdef __cplusplus
}
#endif
#endif
