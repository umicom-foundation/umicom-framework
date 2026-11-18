/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/automation_pointer.h
 *
 * PURPOSE:
 *   Observe whether a tagged GTK control can receive a centre-point pointer hit
 *   without injecting input, reading values or changing application state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_AUTOMATION_POINTER_H
#define UMICOM_UI_GTK4_AUTOMATION_POINTER_H
#include "umicom/ui/automation.h"
#ifdef __cplusplus
extern "C" {
#endif

/** A point sample is evidence about routing inside one GTK window. */
typedef enum UmiGtk4AutomationPointerState {
    UMI_GTK4_POINTER_UNKNOWN = 0,
    UMI_GTK4_POINTER_UNMAPPED = 1,
    UMI_GTK4_POINTER_DISABLED = 2,
    UMI_GTK4_POINTER_OUTSIDE = 3,
    UMI_GTK4_POINTER_OBSTRUCTED = 4,
    UMI_GTK4_POINTER_READY = 5
} UmiGtk4AutomationPointerState;

typedef struct UmiGtk4AutomationPointerSample {
    UmiGtk4AutomationPointerState state;
    float windowX;
    float windowY;
    char targetType[UMI_UI_ID_CAPACITY];
    char hitType[UMI_UI_ID_CAPACITY];
} UmiGtk4AutomationPointerSample;

/** Resolve one stable tag in a live, caller-owned GTK scope on its owning
 * thread. OK means the observation completed; only POINTER_READY means this
 * centre point reaches the target or a descendant. Other states explain why
 * it does not. Missing/ambiguous IDs return NOT_FOUND/ALREADY_EXISTS; lookup
 * errors clear the output. Private controls return PERMISSION_DENIED.
 *
 * Coordinates are GTK logical pixels relative to the containing GtkWindow,
 * not screen pixels. No event pumping, scrolling, focusing, activation, input
 * injection or text collection occurs. The caller waits for layout readiness
 * and reacquires a sample after changes. A narrow/partly covered control may
 * be clickable elsewhere; this API tests only its centre. Foreign windows,
 * OS dialogs, desktop occlusion and real mouse delivery are outside this
 * in-process observation. Pair it with actual input and outcome assertions.
 */
UmiStatus UmiGtk4AutomationProbePointer(void *nativeScope, const char *targetId,
    UmiGtk4AutomationPointerSample *outSample);

#ifdef __cplusplus
}
#endif
#endif
