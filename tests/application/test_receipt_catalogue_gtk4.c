/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_receipt_catalogue_gtk4.c
 * PURPOSE: Verify shared catalogue composition and delegated authority for every native product.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/workstation/shell_header.h"
#include "umicom/application/portfolio.h"
#include <stdio.h>
#include <string.h>
#define CHECK(expression)                                                                                    \
    do                                                                                                       \
    {                                                                                                        \
        if (!(expression))                                                                                   \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #expression);                                              \
            result = 1;                                                                                      \
            goto done;                                                                                       \
        }                                                                                                    \
    } while (0)
/* The rendered-only search missed controls in collapsed review panels. The Framework logical-tree helper replaces it; retain the former traversal for review. */
#if 0
static GtkWidget *find(GtkWidget *widget, const char *id)
{
    if (g_strcmp0(g_object_get_data(G_OBJECT(widget), "umicom-automation-id"), id) == 0)
        return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = find(child, id);
        if (found != NULL)
            return found;
    }
    return NULL;
}
#endif
/* Inspect logical ownership as well as rendered children. Finding a control
 * does not grant permission to edit it or make a collapsed panel visible. */
static GtkWidget *find(GtkWidget *widget, const char *id)
{
    return umi_gtk4_automation_find_tagged_widget(widget, id);
}
/* Capture the delegated action so an independent request cannot silently
 * fall back to the standard launch path. */
static UmiGtk4WorkstationApplicationOpenMode last_mode;
static UmiStatus host(const char *application_id, UmiGtk4WorkstationApplicationOpenMode mode, void *data)
{
    size_t *calls = data;
    (void)application_id;
    last_mode = mode;
    ++*calls;
    return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    UmiGtk4WorkstationShellHeader *header = NULL;
    UmiApplicationLaunchReceipt receipt;
    GtkWidget *root = NULL, *open = NULL, *activity = NULL;
    bool open_retained = false;
    GtkTextIter begin, end;
    char *text = NULL;
    size_t calls = 0U;
    int result = 0;
    if (!gtk_init_check())
        return 77;
    CHECK(argc == 2);
    const UmiApplicationDefinition *product = umi_application_portfolio_find(argv[1]);
    CHECK(product != NULL);
    UmiGtk4WorkstationShellHeaderConfig config =
        umi_gtk4_ws_shell_header_config_default(product->application_id, product->display_name);
    CHECK(umi_gtk4_ws_shell_header_create_managed(&config, &header) == UMI_STATUS_OK);
    root = umi_gtk4_ws_shell_header_widget(header);
    g_object_ref(root);
    activity = find(root, "workstation.application.activity");
    CHECK(GTK_IS_TEXT_VIEW(activity) && !gtk_text_view_get_editable(GTK_TEXT_VIEW(activity)));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(activity));
    gtk_text_buffer_get_bounds(buffer, &begin, &end);
    text = gtk_text_buffer_get_text(buffer, &begin, &end, FALSE);
    CHECK(strstr(text, "No native launches") != NULL);
    CHECK(UmiGtk4WorkstationApplicationReceipt(header, product->application_id, &receipt) ==
          UMI_STATUS_NOT_FOUND);
    /* A delegated request is not native process evidence. The observer must
     * never manufacture a running receipt on behalf of an external host. */
    CHECK(umi_gtk4_ws_shell_header_set_application_open_handler(header, host, &calls) == UMI_STATUS_OK);
/* Bank cannot select itself from its own catalogue. The former implementation is retained for review. */
#if 0
    open = find(root, "workstation.application.org.umicom.bank");
#endif
    /* A host is excluded from its own cross-application catalogue. Select
     * another real product so every host exercises the same delegated action. */
    const char *target_id = strcmp(product->application_id, "org.umicom.bank") == 0
                                ? "org.umicom.trader" : "org.umicom.bank";
    char open_id[128], instance_id[128];
    (void)snprintf(open_id, sizeof(open_id), "workstation.application.%s", target_id);
    (void)snprintf(instance_id, sizeof(instance_id), "workstation.application.new-window.%s", target_id);
    open = find(root, open_id);
    CHECK(GTK_IS_BUTTON(open));
    g_object_ref(open);
    open_retained = true;
    g_signal_emit_by_name(open, "clicked");
    CHECK(calls == 1U);
    CHECK(last_mode == UMI_GTK4_WORKSTATION_APPLICATION_OPEN_STANDARD);
/* Use the same eligible target for both delegated modes. The former implementation is retained for review. */
#if 0
    GtkWidget *new_instance = find(root, "workstation.application.new-window.org.umicom.bank");
#endif
    GtkWidget *new_instance = find(root, instance_id);
    CHECK(GTK_IS_BUTTON(new_instance));
    g_signal_emit_by_name(new_instance, "clicked");
    CHECK(calls == 2U);
    CHECK(last_mode == UMI_GTK4_WORKSTATION_APPLICATION_OPEN_NEW_WINDOW);

/* Delegation must not manufacture a receipt for the selected product. The former implementation is retained for review. */
#if 0
    CHECK(UmiGtk4WorkstationApplicationReceipt(header, "org.umicom.bank", &receipt) == UMI_STATUS_NOT_FOUND);
#endif
    CHECK(UmiGtk4WorkstationApplicationReceipt(header, target_id, &receipt) == UMI_STATUS_NOT_FOUND);
    umi_gtk4_ws_shell_header_destroy(header);
    header = NULL;
    g_signal_emit_by_name(open, "clicked");
    CHECK(calls == 2U);
done:
    umi_gtk4_ws_shell_header_destroy(header);
    if (open_retained)
        g_object_unref(open);
    if (root != NULL)
        g_object_unref(root);
    g_free(text);
    return result;
}
