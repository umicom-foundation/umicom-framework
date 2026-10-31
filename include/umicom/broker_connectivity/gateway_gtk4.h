/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/gateway_gtk4.h
 *
 * PURPOSE:
 *   Own asynchronous Gateway discovery and an explicit local launch action.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_GATEWAY_GTK4_H
#define UMICOM_BROKER_GATEWAY_GTK4_H
#include <gtk/gtk.h>
typedef struct UmiIbkrGatewayGtk UmiIbkrGatewayGtk;
/* Main-thread API. The owner retains this panel until Destroy; its Widget is
 * borrowed. Detection runs in a worker and never opens a socket or a process.
 * Optional NULL-terminated roots (at most eight) override standard locations.
 * Roots are copied; an empty list performs no default-location search. */
UmiIbkrGatewayGtk *UmiIbkrGatewayGtkCreate(const char *const *roots);
GtkWidget *UmiIbkrGatewayGtkWidget(UmiIbkrGatewayGtk *owner);
/* Retire before destroying the containing window. Pending work is cancelled;
 * an already explicitly launched broker process remains the user's process. */
void UmiIbkrGatewayGtkDestroy(UmiIbkrGatewayGtk *owner);
#endif
