/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/creative_assets.h
 * PURPOSE: Present explicit local asset capture and new-file copies with worker-owned I/O and GTK lifetime handling.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_CREATIVE_ASSETS_H
#define UMICOM_UI_GTK4_CREATIVE_ASSETS_H
#include <gtk/gtk.h>
#include "umicom/creative_workspace/asset.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Floating widget; parent it or sink/unref it on the GTK owner thread. No I/O
 * occurs at creation. One asset lives in memory until replaced or closed;
 * it is not automatically part of a saved creative project. File actions run
 * on workers, and late completions never access a destroyed root. */
    GtkWidget *UmiCreativeAssetsGtkCreate(void);
    /* Explicit in-memory capture for hosts with their own file/drag/drop adapter.
 * This copies synchronously; use the panel's file action for worker I/O.
 * A failed capture leaves the previous asset intact. Busy panels refuse it. */
    UmiStatus UmiCreativeAssetsGtkCapture(GtkWidget *root, const char *label, UmiCreativeAssetKind kind,
                                          const void *bytes, size_t size);
    /* Transfer a worker-prepared capture without copying large payloads on the
     * GTK thread. Success sets *asset=NULL; a busy/invalid panel retains it.
     * The previous capture is replaced only after ownership can be accepted. */
    UmiStatus UmiCreativeAssetsGtkAdopt(GtkWidget *root, UmiCreativeAsset **asset);
    UmiStatus UmiCreativeAssetsGtkInspect(GtkWidget *root, UmiCreativeAssetInfo *out);
#ifdef __cplusplus
}
#endif
#endif
