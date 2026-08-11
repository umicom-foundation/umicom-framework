/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/browser/bridge.h
 *
 * PURPOSE:
 *   Queue bounded messages between native code and embedded web content.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Browser contracts describe navigation and messages without exposing WebKit, Edge, GTK or Qt objects.
 */

#ifndef UMICOM_BROWSER_BRIDGE_H
#define UMICOM_BROWSER_BRIDGE_H
#include "umicom/browser/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BROWSER_BRIDGE_CAPACITY 32U
typedef struct UmiBrowserBridge UmiBrowserBridge;
UmiStatus umi_browser_bridge_create(UmiBrowserBridge **out_bridge);
void umi_browser_bridge_destroy(UmiBrowserBridge *bridge);
UmiStatus umi_browser_bridge_push(UmiBrowserBridge *bridge,const char *message);
UmiStatus umi_browser_bridge_pop(UmiBrowserBridge *bridge,char *out_message,size_t capacity);
size_t umi_browser_bridge_count(const UmiBrowserBridge *bridge);
#ifdef __cplusplus
}
#endif
#endif
