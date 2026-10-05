/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_action_menu_gtk4.c
 * PURPOSE: Exercise searchable action groups, Unicode matching, callback identity and retained GTK ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/action_menu.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
static void Pump(void)
{
    for (unsigned step = 0U; step < 512U && g_main_context_pending(NULL); ++step)
        (void)g_main_context_iteration(NULL, FALSE);
}
static void Activated(GtkButton *button, gpointer data)
{
    (void)button;
    ++*(unsigned *)data;
}
static void Finalized(gpointer data, GObject *object)
{
    (void)object;
    *(int *)data = 1;
}
static void ReplaceQuery(GtkEditable *entry, gpointer data)
{
    int *changed = data;
    if (*changed || strcmp(gtk_editable_get_text(entry), "build") != 0)
        return;
    *changed = 1;
    gtk_editable_set_text(entry, "missing");
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    const char *cases[] = {
        "create",          "empty",           "append",           "parented",      "non-button",
        "keyword",         "casefold",        "canonical",        "compatibility", "clear",
        "no-match",        "disabled",        "hidden",           "shown",         "capacity",
        "query-limit",     "query-over",      "query-characters", "invalid-query", "invalid-keywords",
        "invalid-label",   "label-over",      "retained-entry",   "retained-list", "retained-button",
        "retired-popover", "reentrant-query", "activation",       "coalesced",     "independent",
        "append-filtered", "invalid-owner",   "null-output"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0, finalized = 0;
    unsigned invoked = 0U;
    GtkWidget *menu = NULL, *button = NULL, *entry = NULL, *list = NULL, *other = NULL, *extra = NULL;
    gchar *large = NULL;
    gulong hook = 0U;
    int replaced = 0;
    if (strcmp(mode, "null-output") == 0)
    {
        CHECK(UmiGtk4ActionMenuCreate("Tools", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    if (strcmp(mode, "invalid-label") == 0)
    {
        CHECK(UmiGtk4ActionMenuCreate("", &menu) == UMI_STATUS_INVALID_ARGUMENT && menu == NULL);
        CHECK(UmiGtk4ActionMenuCreate("\xff", &menu) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    CHECK(UmiGtk4ActionMenuCreate("Tools", &menu) == UMI_STATUS_OK);
    g_object_ref_sink(menu);
    g_object_weak_ref(G_OBJECT(menu), Finalized, &finalized);
    entry = g_object_ref(UmiGtk4ActionMenuFilterEntry(menu));
    CHECK(GTK_IS_ENTRY(entry));
    if (strcmp(mode, "empty") == 0 || strcmp(mode, "create") == 0)
    {
        if (strcmp(mode, "create") == 0)
        {
            CHECK(strcmp(gtk_menu_button_get_label(GTK_MENU_BUTTON(menu)), "Tools") == 0);
            CHECK(GTK_IS_POPOVER(gtk_menu_button_get_popover(GTK_MENU_BUTTON(menu))));
            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), "") == 0);
        }
        else
        {
            CHECK(UmiGtk4ActionMenuSetFilter(menu, "anything") == UMI_STATUS_OK);
            Pump();
        }
        CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 0U);
        goto cleanup;
    }
    if (strcmp(mode, "invalid-owner") == 0)
    {
        CHECK(UmiGtk4ActionMenuSetFilter(NULL, "x") == UMI_STATUS_INVALID_ARGUMENT &&
              UmiGtk4ActionMenuVisibleCount(NULL) == 0U && UmiGtk4ActionMenuFilterEntry(NULL) == NULL);
        goto cleanup;
    }
    const char *label = "Run Build";
    if (strcmp(mode, "canonical") == 0)
        label = "R\xc3\xa9sum\xc3\xa9";
    if (strcmp(mode, "compatibility") == 0)
        label = "\xf0\x9d\x94\xb8"
                "CTION";
    button = g_object_ref_sink(gtk_button_new_with_label(label));
    g_signal_connect(button, "clicked", G_CALLBACK(Activated), &invoked);
    if (strcmp(mode, "non-button") == 0)
    {
        extra = g_object_ref_sink(gtk_label_new("Text"));
        CHECK(UmiGtk4ActionMenuAppend(menu, extra, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    if (strcmp(mode, "invalid-keywords") == 0)
    {
        CHECK(UmiGtk4ActionMenuAppend(menu, button, "\xff") == UMI_STATUS_INVALID_ARGUMENT &&
              gtk_widget_get_parent(button) == NULL);
        goto cleanup;
    }
    if (strcmp(mode, "label-over") == 0)
    {
        large = g_strnfill(513U, 'a');
        gtk_button_set_label(GTK_BUTTON(button), large);
        CHECK(UmiGtk4ActionMenuAppend(menu, button, NULL) == UMI_STATUS_INVALID_ARGUMENT &&
              gtk_widget_get_parent(button) == NULL);
        goto cleanup;
    }
    CHECK(UmiGtk4ActionMenuAppend(menu, button, "compile project") == UMI_STATUS_OK);
    Pump();
    CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 1U && invoked == 0U);
    if (strcmp(mode, "parented") == 0)
        CHECK(UmiGtk4ActionMenuAppend(menu, button, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    if (strcmp(mode, "keyword") == 0)
    {
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "compile") == UMI_STATUS_OK);
        Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 1U);
    }
    if (strcmp(mode, "casefold") == 0)
    {
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "rUN bUILD") == UMI_STATUS_OK);
        Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 1U);
    }
    if (strcmp(mode, "canonical") == 0)
    {
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "re\xcc\x81sume\xcc\x81") == UMI_STATUS_OK);
        Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 1U);
    }
    if (strcmp(mode, "compatibility") == 0)
    {
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "action") == UMI_STATUS_OK);
        Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 1U);
    }
    if (strcmp(mode, "clear") == 0 || strcmp(mode, "no-match") == 0)
    {
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "missing") == UMI_STATUS_OK);
        Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 0U);
        if (strcmp(mode, "clear") == 0)
        {
            CHECK(UmiGtk4ActionMenuSetFilter(menu, "") == UMI_STATUS_OK);
            Pump();
            CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 1U);
        }
    }
    if (strcmp(mode, "disabled") == 0)
    {
        gtk_widget_set_sensitive(button, FALSE);
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "build") == UMI_STATUS_OK);
        Pump();
        CHECK(!gtk_widget_get_sensitive(button) && UmiGtk4ActionMenuVisibleCount(menu) == 1U);
    }
    if (strcmp(mode, "hidden") == 0 || strcmp(mode, "shown") == 0)
    {
        gtk_widget_set_visible(button, FALSE);
        Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 0U && !gtk_widget_get_visible(button));
        if (strcmp(mode, "shown") == 0)
        {
            gtk_widget_set_visible(button, TRUE);
            Pump();
            CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 1U);
        }
    }
    if (strcmp(mode, "capacity") == 0)
    {
        for (unsigned i = 1U; i < 256U; ++i)
            CHECK(UmiGtk4ActionMenuAppend(menu, gtk_button_new_with_label("Action"), NULL) == UMI_STATUS_OK);
        extra = g_object_ref_sink(gtk_button_new_with_label("Excess"));
        CHECK(UmiGtk4ActionMenuAppend(menu, extra, NULL) == UMI_STATUS_CAPACITY_EXCEEDED &&
              gtk_widget_get_parent(extra) == NULL);
        Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 256U);
    }
    if (strcmp(mode, "query-limit") == 0)
    {
        large = g_malloc(1025U);
        for (size_t i = 0U; i < 256U; ++i)
            memcpy(large + i * 4U, "\xf0\x9f\x98\x80", 4U);
        large[1024] = '\0';
        CHECK(UmiGtk4ActionMenuSetFilter(menu, large) == UMI_STATUS_OK);
        Pump();
        CHECK(strlen(gtk_editable_get_text(GTK_EDITABLE(entry))) == 1024U &&
              UmiGtk4ActionMenuVisibleCount(menu) == 0U);
    }
    if (strcmp(mode, "query-over") == 0 || strcmp(mode, "query-characters") == 0)
    {
        large = g_strnfill(strcmp(mode, "query-over") == 0 ? 1025U : 257U, 'a');
        CHECK(UmiGtk4ActionMenuSetFilter(menu, large) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), "") == 0 &&
              UmiGtk4ActionMenuVisibleCount(menu) == 1U);
    }
    if (strcmp(mode, "invalid-query") == 0)
    {
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "\xff") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4ActionMenuSetFilter(menu, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    if (strcmp(mode, "reentrant-query") == 0)
    {
        hook = g_signal_connect(entry, "changed", G_CALLBACK(ReplaceQuery), &replaced);
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "build") == UMI_STATUS_INVALID_STATE);
        g_signal_handler_disconnect(entry, hook);
        hook = 0U;
        Pump();
        CHECK(replaced && strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), "missing") == 0 &&
              UmiGtk4ActionMenuVisibleCount(menu) == 0U);
    }
    if (strcmp(mode, "coalesced") == 0)
    {
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "missing") == UMI_STATUS_OK);
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "compile") == UMI_STATUS_OK);
        Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 1U);
    }
    if (strcmp(mode, "independent") == 0)
    {
        CHECK(UmiGtk4ActionMenuCreate("Other", &other) == UMI_STATUS_OK);
        g_object_ref_sink(other);
        CHECK(UmiGtk4ActionMenuAppend(other, gtk_button_new_with_label("Other Action"), NULL) ==
              UMI_STATUS_OK);
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "missing") == UMI_STATUS_OK);
        Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 0U && UmiGtk4ActionMenuVisibleCount(other) == 1U);
    }
    if (strcmp(mode, "append-filtered") == 0)
    {
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "debug") == UMI_STATUS_OK);
        Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 0U);
        CHECK(UmiGtk4ActionMenuAppend(menu, gtk_button_new_with_label("Debug"), NULL) == UMI_STATUS_OK);
        Pump();
        CHECK(UmiGtk4ActionMenuVisibleCount(menu) == 1U);
    }
    if (strcmp(mode, "retired-popover") == 0)
    {
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "build") == UMI_STATUS_OK);
        gtk_menu_button_set_popover(GTK_MENU_BUTTON(menu), NULL);
        Pump();
        CHECK(UmiGtk4ActionMenuFilterEntry(menu) == NULL && UmiGtk4ActionMenuVisibleCount(menu) == 0U);
    }
    if (strcmp(mode, "retained-entry") == 0 || strcmp(mode, "retained-list") == 0 ||
        strcmp(mode, "retained-button") == 0)
    {
        if (strcmp(mode, "retained-list") == 0)
        {
            list = gtk_widget_get_parent(gtk_widget_get_parent(button));
            CHECK(GTK_IS_LIST_BOX(list));
            g_object_ref(list);
        }
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "build") == UMI_STATUS_OK);
        g_clear_object(&menu);
        CHECK(finalized);
        gtk_editable_set_text(GTK_EDITABLE(entry), "late query");
        Pump();
        if (list != NULL)
            gtk_list_box_invalidate_filter(GTK_LIST_BOX(list));
        if (strcmp(mode, "retained-button") == 0)
        {
            g_signal_emit_by_name(button, "clicked");
            CHECK(invoked == 1U);
        }
    }
    if (strcmp(mode, "activation") == 0)
    {
        CHECK(UmiGtk4ActionMenuSetFilter(menu, "build") == UMI_STATUS_OK);
        Pump();
        CHECK(invoked == 0U);
        g_signal_emit_by_name(button, "clicked");
        CHECK(invoked == 1U);
    }
cleanup:
    if (hook != 0U)
        g_signal_handler_disconnect(entry, hook);
    g_clear_object(&menu);
    g_clear_object(&other);
    g_clear_object(&button);
    g_clear_object(&extra);
    g_clear_object(&entry);
    g_clear_object(&list);
    g_free(large);
    Pump();
    return failed;
}
