/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/creation_wait.h
 *
 * PURPOSE:
 *   Wait for native project form completion without assuming a click performs synchronous file I/O.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_PROJECT_CREATION_WAIT_H
#define UMICOM_TEST_PROJECT_CREATION_WAIT_H
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
/* The caller owns a native form reference across this wait. No controller
 * pointer is borrowed after an adoption callback could have destroyed it. */
static int UmiTestProjectCreationWait(GtkWindow *form)
{
    const gint64 deadline = g_get_monotonic_time() + 20 * G_TIME_SPAN_SECOND;
    while (UmiGtk4WindowIsOpen(form) && gtk_widget_get_visible(GTK_WIDGET(form))) {
        GtkWidget *accept = umi_gtk4_automation_find_tagged_widget(form, "developer.dialog.accept");
        if (accept == NULL || gtk_widget_get_sensitive(accept)) return 1;
        if (g_get_monotonic_time() >= deadline) return 0;
        (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return 1;
}
#endif
