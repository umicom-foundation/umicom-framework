/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/media_workspace_style.h
 *
 * PURPOSE:
 *   Share scoped Media styling and canonical vector branding between native media surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MEDIA_WORKSPACE_STYLE_H
#define UMICOM_MEDIA_WORKSPACE_STYLE_H
#include <gtk/gtk.h>
#include <stdbool.h>
void UmiMediaWorkspaceStyle(GtkWidget *root);
GtkWidget *UmiMediaWorkspaceBrand(bool wordmark);
#endif
