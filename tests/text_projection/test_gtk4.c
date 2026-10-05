/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/text_projection/test_gtk4.c
 * PURPOSE: Exercise native filtering without confusing display positions and source identities.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/filtered_choices.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            failed = 1;                                                                                      \
            goto done;                                                                                       \
        }                                                                                                    \
    } while (0)
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && !strcmp(tag, id))
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, id);
        if (found)
            return found;
    }
    return NULL;
}
typedef struct Probe
{
    GtkWidget *root;
    unsigned calls;
    UmiStatus status;
    size_t selected;
} Probe;
static void Reenter(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object;
    (void)property;
    Probe *probe = context;
    ++probe->calls;
    const char *replacement[] = {"unexpected"};
    probe->status = UmiGtk4FilteredChoicesSetRows(probe->root, replacement, 1U);
    probe->selected = 99U;
    if (UmiGtk4FilteredChoicesSelectedSource(probe->root, &probe->selected) != UMI_STATUS_BUSY)
        probe->selected = 0U;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    if (!gtk_init_check())
        return 77;
    const char *name = argv[1];
    int failed = 0;
    size_t selected = 99U;
    GtkDropDown *picker = GTK_DROP_DOWN(g_object_ref_sink(gtk_drop_down_new(NULL, NULL)));
    GtkWidget *root = g_object_ref_sink(UmiGtk4FilteredChoicesCreate(picker));
    GtkWidget *query = Find(root, "choices.query"), *order = Find(root, "choices.order"),
              *apply = Find(root, "choices.apply");
    Probe probe = {root, 0U, UMI_STATUS_OK, 99U};
    CHECK(query && order && apply);
    const char *rows[] = {"Zulu", "alpha", "ALPHA", "beta"};
    if (!strcmp(name, "reentrant"))
        g_signal_connect(picker, "notify::model", G_CALLBACK(Reenter), &probe);
    CHECK(UmiGtk4FilteredChoicesSetRows(root, rows, 4U) == UMI_STATUS_OK);
    CHECK(gtk_drop_down_get_selected(picker) == GTK_INVALID_LIST_POSITION);
    if (!strcmp(name, "reentrant"))
    {
        CHECK(probe.calls == 1U && probe.status == UMI_STATUS_BUSY && probe.selected == 99U);
        goto done;
    }
    gtk_drop_down_set_selected(picker, 2U);
    CHECK(UmiGtk4FilteredChoicesSelectedSource(root, &selected) == UMI_STATUS_OK && selected == 2U);
    if (!strcmp(name, "sort") || !strcmp(name, "filter") || !strcmp(name, "hidden"))
    {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(order), 1U);
        gtk_editable_set_text(GTK_EDITABLE(query), !strcmp(name, "hidden")   ? "zulu"
                                                   : !strcmp(name, "filter") ? "alpha"
                                                                             : "");
        /* Merely typing a query is not an application command. */
        CHECK(UmiGtk4FilteredChoicesSelectedSource(root, &selected) == UMI_STATUS_OK && selected == 2U);
        g_signal_emit_by_name(apply, "clicked");
        if (!strcmp(name, "hidden"))
            CHECK(UmiGtk4FilteredChoicesSelectedSource(root, &selected) == UMI_STATUS_NOT_FOUND);
        else
            CHECK(UmiGtk4FilteredChoicesSelectedSource(root, &selected) == UMI_STATUS_OK && selected == 2U &&
                  gtk_drop_down_get_selected(picker) == 1U);
    }
    else if (!strcmp(name, "replacement"))
    {
        CHECK(UmiGtk4FilteredChoicesSetRows(root, rows, 4U) == UMI_STATUS_OK);
        CHECK(UmiGtk4FilteredChoicesSelectedSource(root, &selected) == UMI_STATUS_NOT_FOUND);
    }
    else if (!strcmp(name, "clear"))
    {
        CHECK(UmiGtk4FilteredChoicesSetRows(root, NULL, 0U) == UMI_STATUS_OK);
        CHECK(gtk_drop_down_get_model(picker) == NULL);
    }
    else if (!strcmp(name, "invalid-utf8"))
    {
        const char *bad[] = {"\xff"};
        CHECK(UmiGtk4FilteredChoicesSetRows(root, bad, 1U) == UMI_STATUS_PARSE_ERROR);
        CHECK(UmiGtk4FilteredChoicesSelectedSource(root, &selected) == UMI_STATUS_OK && selected == 2U);
    }
    else if (!strcmp(name, "external-model"))
    {
        gtk_drop_down_set_model(picker, NULL);
        CHECK(UmiGtk4FilteredChoicesSelectedSource(root, &selected) == UMI_STATUS_INVALID_STATE);
    }
    else if (!strcmp(name, "retained"))
    {
        g_object_ref(apply);
        g_object_unref(root);
        root = NULL;
        g_signal_emit_by_name(apply, "clicked");
        g_object_unref(apply);
    }
    else
        failed = 2;
done:
    g_signal_handlers_disconnect_by_data(picker, &probe);
    g_clear_object(&root);
    g_object_unref(picker);
    return failed;
}
