/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/desk.h
 *
 * PURPOSE:
 *   Expose the reusable GTK4 Umicom Desk shell without leaking GTK object types
 *   through product-neutral headers. The thin desktop module supplies runtime
 *   composition; this adapter renders Framework-owned semantic state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_DESK_H
#define UMICOM_UI_GTK4_DESK_H

#include "umicom/desktop/desk_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the gtk4 desk data shared with callers of this public contract.
 */
typedef struct UmiGtk4Desk UmiGtk4Desk;

/**
 * Initialise gtk4 desk from caller-provided values so later operations receive a known
 * state. Builds and refreshes the native window without presenting it. The
 * runtime is borrowed and must outlive Desk; native widgets are Desk-owned.
 * Callers may install Framework titlebar/context composition before present.
 */
UmiStatus umi_gtk4_desk_create(
    void *native_gtk_application,
    UmiDeskRuntime *runtime,
    UmiGtk4Desk **out_desk);
/**
 * Release or reset state held by gtk4 desk so the same storage can be reused safely.
 * Disconnect Desk callbacks, destroy its native window and release retained
 * original content before freeing the controller. External widget references
 * may remain, but cannot dispatch into this released Desk. GTK thread only.
 */
void umi_gtk4_desk_destroy(UmiGtk4Desk *desk);
/** Hide the duplicate in-content product label only after a host has installed
 * topmost identity. Other Desk controls stay intact; the default is visible. */
UmiStatus umi_gtk4_desk_set_content_identity_visible(UmiGtk4Desk *desk, bool visible);
/**
 * Provide the gtk4 desk refresh operation used by this module and its client applications.
 */
UmiStatus umi_gtk4_desk_refresh(UmiGtk4Desk *desk);
/**
 * Provide the gtk4 desk present operation used by this module and its client applications.
 */
UmiStatus umi_gtk4_desk_present(UmiGtk4Desk *desk);
/**
 * Provide the gtk4 desk native window operation used by this module and its client
 * applications.
 */
void *umi_gtk4_desk_native_window(UmiGtk4Desk *desk);

#ifdef __cplusplus
}
#endif

#endif
