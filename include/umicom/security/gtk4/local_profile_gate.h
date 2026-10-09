/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/gtk4/local_profile_gate.h
 * PURPOSE: Present a reusable local-profile start screen with an explicit simulator entry.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SECURITY_GTK4_LOCAL_PROFILE_GATE_H
#define UMICOM_SECURITY_GTK4_LOCAL_PROFILE_GATE_H
#include <gtk/gtk.h>
#include "umicom/security/local_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiLocalProfileGate UmiLocalProfileGate;
typedef struct UmiLocalProfileGateConfig {
    const char *application_id;
    const char *title;
    /* Optional retained store for an OS adapter or isolated test fixture.
     * NULL selects the platform credential vault. No insecure fallback. */
    UmiLocalProfileStore *store;
    void *user_data;
    /* Called on the GTK thread. The name is canonical, or empty for the
     * simulator without a profile. It is borrowed for the callback only.
     * May destroy the gate. A failure leaves the start screen available. */
    UmiStatus (*open_workspace)(void *, const char *profile_name);
    void (*open_broker)(void *);
} UmiLocalProfileGateConfig;
UmiStatus UmiLocalProfileGateCreate(const UmiLocalProfileGateConfig *config, UmiLocalProfileGate **out);
GtkWidget *UmiLocalProfileGateWidget(UmiLocalProfileGate *gate);
/**
 * @brief Bind the optional Paper/Live launch choices on the GTK thread.
 * @param gate A live, idle local-profile gate.
 * @param action Callback receiving config.user_data and zero for Paper or one for Live.
 * This only requests a connection screen. It cannot authenticate or authorise
 * orders. The callback may destroy the gate. NULL disables the optional route.
 */
void UmiLocalProfileGateSetBrokerEnvironmentAction(UmiLocalProfileGate *gate,
    void (*action)(void *, int));

/* GTK thread only. Stops completion callbacks immediately, clears password
 * fields and releases widgets. A running vault operation may finish, but can
 * never open a workspace after this call. Retained buttons become inert. */
void UmiLocalProfileGateDestroy(UmiLocalProfileGate *gate);
#ifdef __cplusplus
}
#endif
#endif
