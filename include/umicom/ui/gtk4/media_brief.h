/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/media_brief.h
 *
 * PURPOSE:
 *   Compose source excerpts, editable briefs and explicit provider handoffs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_MEDIA_BRIEF_H
#define UMICOM_UI_GTK4_MEDIA_BRIEF_H
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* GTK owner thread. The floating result owns its draft and weakly observes
 * optional generator panels. No storage, network or credential access occurs
 * during creation. A host calls Retire before releasing its service owners;
 * retained controls then become inert. The source excerpt stays in memory
 * until the panel is released. Saving is explicit and uses a chosen path. */
GtkWidget *UmiMediaBriefGtkCreate(GtkWidget *image_panel, GtkWidget *video_panel);
void UmiMediaBriefGtkRetire(GtkWidget *root);
#ifdef __cplusplus
}
#endif
#endif
