/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/captions.h
 * PURPOSE: Expose one shared native caption editor for creative applications.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_CAPTIONS_H
#define UMICOM_UI_GTK4_CAPTIONS_H
#include "umicom/creative_workspace/captions.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C"
{
#endif
    /**
 * Create a floating GTK widget with an owned, initially empty SRT draft.
 * Parent or sink/unref it on the GTK thread. File reads/writes run on workers
 * using explicit absolute paths. Destruction requests cancellation; a worker
 * owns its inputs and never borrows widgets. Retained controls become inert.
 * Captions are independent of saved scene projects until explicitly exported.
 */
    UmiStatus UmiCaptionsGtkCreate(GtkWidget **out);
#ifdef __cplusplus
}
#endif
#endif
