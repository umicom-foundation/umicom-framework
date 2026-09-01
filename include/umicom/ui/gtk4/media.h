/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/media.h
 *
 * PURPOSE:
 *   Expose reusable GTK4 picture and video host constructors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_GTK4_MEDIA_H
#define UMICOM_UI_GTK4_MEDIA_H
#include <gtk/gtk.h>
#include "umicom/ui/components/component.h"
GtkWidget *umi_gtk4_picture_new(const UmiUiComponentSpec *spec);
GtkWidget *umi_gtk4_video_new(const UmiUiComponentSpec *spec);
#endif
