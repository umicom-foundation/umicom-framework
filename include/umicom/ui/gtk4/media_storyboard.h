/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/media_storyboard.h
 *
 * PURPOSE:
 *   Compose native scene authoring with existing asset-library and creation-brief owners.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_MEDIA_STORYBOARD_H
#define UMICOM_UI_GTK4_MEDIA_STORYBOARD_H
#include <gtk/gtk.h>
/* Owner thread, floating result. Optional targets are weakly observed and not
 * reparented. Construction performs no I/O. Retire before destroying host services.
 * Scenes are session drafts; exported instructions are text, not a project archive. */
#ifdef __cplusplus
extern "C" {
#endif
GtkWidget *UmiMediaStoryboardGtkCreate(GtkWidget *asset_library, GtkWidget *creation_brief);
void UmiMediaStoryboardGtkRetire(GtkWidget *root);
#ifdef __cplusplus
}
#endif
#endif
