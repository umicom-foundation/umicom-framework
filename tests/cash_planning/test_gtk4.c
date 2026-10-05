/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/cash_planning/test_gtk4.c
 * PURPOSE: Exercise real cash planning widgets, review gates and retained control lifetimes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ui/gtk4/cash_plan.h"
#undef CHECK
#include "../build_log/fixture.h"
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0)
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, id);
        if (found != NULL)
            return found;
    }
    return NULL;
}
static GtkWidget *Control(GtkWidget *root, const char *name)
{
    char id[128];
    (void)snprintf(id, sizeof id, "cash.plan.%s", name);
    GtkWidget *w = Find(root, id);
    if (w == NULL)
        abort();
    return w;
}
static void Set(GtkWidget *root, const char *name, const char *text)
{
    gtk_editable_set_text(GTK_EDITABLE(Control(root, name)), text);
}
static void Click(GtkWidget *root, const char *name)
{
    g_signal_emit_by_name(Control(root, name), "clicked");
}
static char *Output(GtkWidget *root)
{
    GtkTextBuffer *b = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Control(root, "output")));
    GtkTextIter a, z;
    gtk_text_buffer_get_bounds(b, &a, &z);
    return gtk_text_buffer_get_text(b, &a, &z, FALSE);
}
static int Contains(GtkWidget *root, const char *text)
{
    char *out = Output(root);
    int found = strstr(out, text) != NULL;
    g_free(out);
    return found;
}
static const char *Note(GtkWidget *root) { return gtk_label_get_text(GTK_LABEL(Control(root, "note"))); }
static void Setup(GtkWidget *root)
{
    Set(root, "opening", "1000.00");
    Set(root, "buffer", "200.00");
    Set(root, "start", "2028-02-01");
    Set(root, "end", "2028-02-29");
    Click(root, "settings");
}
static void AddRent(GtkWidget *root)
{
    Set(root, "id", "rent");
    Set(root, "label", "Monthly rent");
    Set(root, "date", "2028-02-03");
    Set(root, "amount", "850.00");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(Control(root, "receive")), FALSE);
    Click(root, "add");
}
static int AwaitNote(GtkWidget *root, const char *expected)
{
    gint64 until = g_get_monotonic_time() + 5000000;
    while (g_get_monotonic_time() < until)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        if (strstr(Note(root), expected) != NULL)
            return 1;
        if (strstr(Note(root), "File request refused") != NULL)
            return 0;
        g_usleep(1000U);
    }
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check())
        return 77;
    const char *mode = argv[1];
    GtkWidget *root = NULL;
    OK(UmiGtk4CashPlanCreate(&root));
    g_object_ref_sink(root);
    Setup(root);
    if (strcmp(mode, "retained") == 0)
    {
        GtkWidget *button = Control(root, "add");
        g_object_ref(button);
        g_object_unref(root);
        root = NULL;
        g_signal_emit_by_name(button, "clicked");
        g_object_unref(button);
        return 0;
    }
    if (strcmp(mode, "load") == 0 || strcmp(mode, "approve") == 0 || strcmp(mode, "invalidate") == 0 ||
        strcmp(mode, "close-loading") == 0 || strcmp(mode, "changed-loading") == 0)
    {
        char folder[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
        FixtureDirectory(folder);
        FixturePath(path, folder, "review.json");
        UmiCashPlan *p = Plan();
        Add(p, "rent", 3, 85000, 0);
        OK(UmiCashPlanSaveNew(path, p, NULL));
        UmiCashPlanDestroy(p);
        Set(root, "path", path);
        Click(root, "load");
        if (strcmp(mode, "changed-loading") == 0)
        {
            /* The worker result has not returned to the main context yet.
             * This deterministic edit must prevent publication of its review. */
            Set(root, "opening", "999.00");
            CHECK(AwaitNote(root, "Inputs changed while loading"));
            gtk_check_button_set_active(GTK_CHECK_BUTTON(Control(root, "approval")), TRUE);
            Click(root, "apply");
            CHECK(strstr(Note(root), "approve") != NULL);
            Click(root, "reset");
            CHECK(Contains(root, "Closing: 1000.00"));
            g_object_unref(root);
            return 0;
        }
        if (strcmp(mode, "close-loading") == 0)
        {
            gpointer weak = root;
            g_object_add_weak_pointer(G_OBJECT(root), &weak);
            g_object_unref(root);
            root = NULL;
            CHECK(weak == NULL);
            gint64 until = g_get_monotonic_time() + 500000;
            while (g_get_monotonic_time() < until)
            {
                while (g_main_context_iteration(NULL, FALSE))
                {
                }
                g_usleep(1000U);
            }
            return 0;
        }
        CHECK(AwaitNote(root, "Review both plans"));
        CHECK(Contains(root, "LOADED ASSUMPTIONS (not applied)"));
        if (strcmp(mode, "load") == 0)
        {
            Click(root, "apply");
            CHECK(strstr(Note(root), "approve") != NULL);
        }
        else if (strcmp(mode, "invalidate") == 0)
        {
            Set(root, "opening", "999.00");
            gtk_check_button_set_active(GTK_CHECK_BUTTON(Control(root, "approval")), TRUE);
            Click(root, "apply");
            CHECK(strstr(Note(root), "approve") != NULL);
            Click(root, "reset");
            CHECK(Contains(root, "Closing: 1000.00"));
        }
        else
        {
            gtk_check_button_set_active(GTK_CHECK_BUTTON(Control(root, "approval")), TRUE);
            Click(root, "apply");
            CHECK(Contains(root, "Closing: 150.00"));
            Click(root, "undo");
            CHECK(Contains(root, "Closing: 1000.00"));
        }
    }
    else if (strcmp(mode, "settings") == 0)
    {
        CHECK(Contains(root, "2028-02-01 to 2028-02-29") && Contains(root, "Closing: 1000.00"));
    }
    else if (strcmp(mode, "draft") == 0)
    {
        Set(root, "opening", "999.00");
        Set(root, "id", "bill");
        Set(root, "label", "Bill");
        Set(root, "amount", "1.00");
        Click(root, "add");
        CHECK(Contains(root, "Included: 0"));
        Click(root, "reset");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(Control(root, "opening"))), "1000.00") == 0);
    }
    else if (strcmp(mode, "invalid") == 0)
    {
        Set(root, "id", "bad");
        Set(root, "label", "Invalid date");
        Set(root, "date", "2028-02-30");
        Set(root, "amount", "1.00");
        Click(root, "add");
        CHECK(Contains(root, "Included: 0"));
    }
    else
    {
        AddRent(root);
        CHECK(Contains(root, "Closing: 150.00") && Contains(root, "First buffer shortfall: 2028-02-03"));
        if (strcmp(mode, "add") == 0)
            CHECK(Contains(root, "Included: 1"));
        else if (strcmp(mode, "independent") == 0)
        {
            GtkWidget *other = NULL;
            OK(UmiGtk4CashPlanCreate(&other));
            g_object_ref_sink(other);
            Setup(other);
            CHECK(Contains(other, "Closing: 1000.00"));
            g_object_unref(other);
        }
        else if (strcmp(mode, "undo") == 0)
        {
            Click(root, "undo");
            CHECK(Contains(root, "Closing: 1000.00") && Contains(root, "Included: 0"));
        }
        else if (strcmp(mode, "currency") == 0)
        {
            Set(root, "currency", "USD");
            Click(root, "settings");
            CHECK(Contains(root, "GBP") && !Contains(root, "USD"));
        }
        else
        {
            gtk_drop_down_set_selected(GTK_DROP_DOWN(Control(root, "entries")), 0U);
            if (strcmp(mode, "update") == 0)
            {
                Set(root, "amount", "900.00");
                Click(root, "update");
                CHECK(Contains(root, "Closing: 100.00"));
            }
            else if (strcmp(mode, "remove") == 0)
            {
                Click(root, "remove");
                CHECK(Contains(root, "Closing: 1000.00") && Contains(root, "Included: 0"));
            }
            else if (strcmp(mode, "disabled") == 0)
            {
                gtk_check_button_set_active(GTK_CHECK_BUTTON(Control(root, "enabled")), FALSE);
                Click(root, "update");
                CHECK(Contains(root, "Closing: 1000.00") && Contains(root, "Disabled: 1"));
            }
            else
                return 2;
        }
    }
    g_object_unref(root);
    return 0;
}
