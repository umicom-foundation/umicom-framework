/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/output_view.h
 * PURPOSE: Present bounded worker output through shared native controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_OUTPUT_VIEW_H
#define UMICOM_UI_GTK4_OUTPUT_VIEW_H
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_OUTPUT_VIEW_CAPACITY 65536U
    /* A frontend-owned display copy, never a borrowed worker buffer. The host
 * supplies plain-text context/status; the presenter does not infer job success.
 * Identity and revision must increase monotonically. Equal revisions are
 * accepted without replacing selected text unless counters_saturated is set. */
    typedef struct UmiOutputViewSnapshot
    {
        uint64_t operation_id;
        uint64_t revision;
        uint64_t total_bytes;
        size_t length;
        bool truncated;
        bool counters_saturated;
        char context[512];
        char status[512];
        char bytes[UMI_OUTPUT_VIEW_CAPACITY];
    } UmiOutputViewSnapshot;

    /* GTK-owner-thread API. The floating panel owns the latest snapshot and widgets.
 * The prefix creates semantic control tags (.panel, .text, .status, .follow,
 * .refresh, .copy). Controls have weak panel bindings and no service callbacks. */
    GtkWidget *UmiOutputViewGtk4Create(const char *automation_prefix, const char *ready_message);
    /* Pause keeps the displayed text while accepting newer evidence. Refresh shows
 * the latest copy once; Copy copies what is actually displayed. NUL and invalid
 * UTF-8 are made visible without changing the host's raw bytes. No I/O occurs. */
    UmiStatus UmiOutputViewGtk4Update(GtkWidget *panel, const UmiOutputViewSnapshot *snapshot);
#ifdef __cplusplus
}
#endif
#endif
