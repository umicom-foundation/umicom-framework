/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/browser/types.h
 *
 * PURPOSE:
 *   Define toolkit-neutral embedded-browser events and capacities.
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

#ifndef UMICOM_BROWSER_TYPES_H
#define UMICOM_BROWSER_TYPES_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BROWSER_URL_CAPACITY 1024U
#define UMI_BROWSER_MESSAGE_CAPACITY 4096U
#define UMI_BROWSER_HISTORY_CAPACITY 64U
typedef enum UmiBrowserEventKind { UMI_BROWSER_NAVIGATED=1, UMI_BROWSER_TITLE_CHANGED=2, UMI_BROWSER_MESSAGE=3, UMI_BROWSER_CLOSED=4 } UmiBrowserEventKind;
typedef struct UmiBrowserEvent { UmiBrowserEventKind kind; char text[UMI_BROWSER_MESSAGE_CAPACITY]; } UmiBrowserEvent;
typedef void (*UmiBrowserEventSink)(const UmiBrowserEvent *event,void *user_data);
#ifdef __cplusplus
}
#endif
#endif
