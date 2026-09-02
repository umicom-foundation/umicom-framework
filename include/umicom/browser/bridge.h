/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/browser/bridge.h
 *
 * PURPOSE:
 *   Queue bounded messages between native code and embedded web content.
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

#ifndef UMICOM_BROWSER_BRIDGE_H
#define UMICOM_BROWSER_BRIDGE_H
#include "umicom/browser/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BROWSER_BRIDGE_CAPACITY 32U
/**
 * Represent the browser bridge data shared with callers of this public contract.
 */
typedef struct UmiBrowserBridge UmiBrowserBridge;
/**
 * Initialise browser bridge from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_browser_bridge_create(UmiBrowserBridge **out_bridge);
/**
 * Release or reset state held by browser bridge so the same storage can be reused safely.
 */
void umi_browser_bridge_destroy(UmiBrowserBridge *bridge);
/**
 * Provide the browser bridge push operation used by this module and its client
 * applications.
 */
UmiStatus umi_browser_bridge_push(UmiBrowserBridge *bridge,const char *message);
/**
 * Provide the browser bridge pop operation used by this module and its client
 * applications.
 */
UmiStatus umi_browser_bridge_pop(UmiBrowserBridge *bridge,char *out_message,size_t capacity);
/**
 * Return the number of records represented by browser bridge without changing their state.
 */
size_t umi_browser_bridge_count(const UmiBrowserBridge *bridge);
#ifdef __cplusplus
}
#endif
#endif
