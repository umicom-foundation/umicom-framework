/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/access_gtk4.h
 *
 * PURPOSE:
 *   Keep native connection controls aligned with the chosen local broker mode.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_ACCESS_GTK4_H
#define UMICOM_BROKER_ACCESS_GTK4_H
#include "umicom/broker_connectivity/access_plan.h"
#include "umicom/broker_connectivity/connection_gtk4.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Create an unpresented inspector using a validated, copied connection plan.
 * Guest has no broker connection and returns NULL. No socket is opened and
 * Live acknowledgement is never inherited. GTK main context only. */
GtkWindow *UmiIbkrGtkCreateForPlan(GtkWindow *parent, const UmiBrokerAccessPlan *plan);
/* The launcher copies plan. Removing its native parent makes retained controls
 * inert. Reopening starts disconnected with fresh, unchecked Live consent. */
GtkWidget *UmiIbkrGtkLauncherCreateForPlan(GtkWindow *parent, const UmiBrokerAccessPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
