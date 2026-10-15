/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/healthcare/test_access_gtk4.c
 * PURPOSE: Exercise the native workflow and a retained control after owner destruction.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/healthcare/access_gtk4.h"
#include <stdio.h>
#include <string.h>

/* Find stable widget names instead of depending on translated button captions. */
static GtkWidget *find(GtkWidget *root, const char *name)
{
    GtkWidget *child;
    if (strcmp(gtk_widget_get_name(root), name) == 0) return root;
    for (child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *match = find(child, name);
        if (match != NULL) return match;
    }
    return NULL;
}

int main(void)
{
    GtkWidget *root = NULL, *retained = NULL, *details;
    static const char *const actions[] = {
        "healthcare-register", "healthcare-request", "healthcare-full",
        "healthcare-accept", "healthcare-funding", "healthcare-reserve", "healthcare-collect"
    };
    size_t index;
    int result = 1;
    if (!gtk_init_check()) return 77;
    if (UmiHealthcareAccessWorkspaceNew(&root) != UMI_STATUS_OK) return 1;
    g_object_ref_sink(root);
    for (index = 0U; index < sizeof(actions) / sizeof(actions[0]); ++index) {
        GtkWidget *button = find(root, actions[index]);
        if (button == NULL) goto cleanup;
        g_signal_emit_by_name(button, "clicked");
    }
    details = find(root, "healthcare-details");
    if (details == NULL || strstr(gtk_label_get_text(GTK_LABEL(details)), "Collected: 1") == NULL)
        goto cleanup;
    retained = find(root, "healthcare-request");
    if (retained == NULL) goto cleanup;
    g_object_ref(retained);
    result = 0;
cleanup:
    g_object_unref(root);
    /* The disconnected owner must make this inert, even if a host retains a
     * reference to the button after its original workspace is gone. */
    if (retained != NULL) {
        g_signal_emit_by_name(retained, "clicked");
        g_object_unref(retained);
    }
    if (result != 0) fputs("Healthcare native workflow failed\n", stderr);
    return result;
}
