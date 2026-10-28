/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/welcome_gtk4.h
 *
 * PURPOSE:
 *   Present explicit broker modes while the product owns workspace composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_WELCOME_GTK4_H
#define UMICOM_BROKER_WELCOME_GTK4_H
#include <gtk/gtk.h>
#include "umicom/broker_connectivity/access_plan.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiBrokerWelcomeGtk UmiBrokerWelcomeGtk;
typedef struct UmiBrokerWelcomeGtkConfig {
    void *userData;
    /* Called only on an explicit click, on the GTK thread. The plan is borrowed
     * for the duration of this call. May destroy this welcome controller. */
    UmiStatus (*openMode)(void *, const UmiBrokerAccessPlan *);
    /* Optional compatibility route for local workspace profiles. */
    UmiStatus (*openLocalProfiles)(void *);
} UmiBrokerWelcomeGtkConfig;

UmiStatus UmiBrokerWelcomeGtkCreate(const UmiBrokerWelcomeGtkConfig *config,
    UmiBrokerWelcomeGtk **outWelcome);
/* Borrowed widget. The controller retains it until Destroy, even if unparented. */
GtkWidget *UmiBrokerWelcomeGtkWidget(UmiBrokerWelcomeGtk *welcome);
/* GTK thread only. Retained controls become inert; no asynchronous callback,
 * network operation or credential handling is owned by this controller. */
void UmiBrokerWelcomeGtkDestroy(UmiBrokerWelcomeGtk *welcome);
#ifdef __cplusplus
}
#endif
#endif
