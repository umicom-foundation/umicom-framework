/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/components/media_gtk4.c
 *
 * PURPOSE:
 *   Implement reusable GTK4 rendering for common semantic Umicom components.
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
#include "umicom/ui/gtk4/media.h"
GtkWidget *umi_gtk4_picture_new(const UmiUiComponentSpec*s){return s&&s->text[0]?gtk_picture_new_for_filename(s->text):gtk_picture_new();}
GtkWidget *umi_gtk4_video_new(const UmiUiComponentSpec*s){return s&&s->text[0]?gtk_video_new_for_filename(s->text):gtk_video_new();}
