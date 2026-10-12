/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/video_sequence.h
 * PURPOSE: Compose reusable video editing, local preview and explicit render actions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_VIDEO_SEQUENCE_H
#define UMICOM_UI_GTK4_VIDEO_SEQUENCE_H
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/** @brief Create a floating video sequence editor without reading or launching anything.
 * All calls belong on the GTK thread. The host must retire before releasing its
 * window or panel, including when controls are retained by another owner. */
GtkWidget *UmiVideoSequenceGtk4Create(void);
/** @brief Disable callbacks, stop preview and request cancellation of accepted work.
 * Idempotent. Workers own their inputs and may leave a partial new output file. */
void UmiVideoSequenceGtk4Retire(GtkWidget *root);
#ifdef __cplusplus
}
#endif
#endif
