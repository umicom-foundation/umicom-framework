/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/event.h
 *
 * PURPOSE:
 *   Represent toolkit-neutral component events that product controllers can handle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_EVENT_H
#define UMICOM_UI_COMPONENTS_EVENT_H
#include <stdint.h>
#include "umicom/ui/components/component.h"
typedef enum UmiUiComponentEventKind { UMI_UI_COMPONENT_EVENT_ACTIVATE=1, UMI_UI_COMPONENT_EVENT_CHANGED=2, UMI_UI_COMPONENT_EVENT_SELECTED=3, UMI_UI_COMPONENT_EVENT_CLOSED=4 } UmiUiComponentEventKind;
typedef struct UmiUiComponentEvent { UmiUiComponentEventKind kind; char component_id[UMI_UI_COMPONENT_ID_CAPACITY]; int64_t integer_value; double real_value; } UmiUiComponentEvent;
#endif
