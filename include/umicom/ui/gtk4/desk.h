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

typedef struct UmiGtk4Desk UmiGtk4Desk;

UmiStatus umi_gtk4_desk_create(
    void *native_gtk_application,
    UmiDeskRuntime *runtime,
    UmiGtk4Desk **out_desk);
void umi_gtk4_desk_destroy(UmiGtk4Desk *desk);
UmiStatus umi_gtk4_desk_refresh(UmiGtk4Desk *desk);
UmiStatus umi_gtk4_desk_present(UmiGtk4Desk *desk);
void *umi_gtk4_desk_native_window(UmiGtk4Desk *desk);

#ifdef __cplusplus
}
#endif

#endif
