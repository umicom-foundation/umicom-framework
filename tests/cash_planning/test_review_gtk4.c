/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/cash_planning/test_review_gtk4.c
 * PURPOSE: Exercise scenario review and closed-host refusal through real native controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#undef CHECK
#include "../build_log/fixture.h"
#include "../../adapters/gtk4/cash_plan_private.h"

static GtkWidget *Control(GtkWidget *root, const char *name)
{
    char id[128];
    (void)snprintf(id, sizeof id, "cash.plan.%s", name);
    GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(root, id);
    CHECK(widget != NULL);
    return widget;
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
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Control(root, "output")));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}
static void Has(GtkWidget *root, const char *expected)
{
    char *text = Output(root);
    CHECK(strstr(text, expected) != NULL);
    g_free(text);
}
static void Wait(CashPanel *panel)
{
    gint64 deadline = g_get_monotonic_time() + 5000000;
    while (panel->busy && g_get_monotonic_time() < deadline)
    {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    CHECK(!panel->busy);
}
static void RetireOnApproval(GtkCheckButton *button, gpointer root)
{
    (void)button;
    UmiGtk4CashPlanRetire(GTK_WIDGET(root));
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    GtkWidget *root = NULL;
    GtkWindow *host = NULL, *parent = NULL;
    if (strcmp(mode, "host-close") == 0)
    {
        parent = GTK_WINDOW(gtk_window_new()); g_object_ref_sink(parent);
        GtkWidget *launcher = UmiGtk4CashPlanLauncherCreate();
        gtk_window_set_child(parent, launcher);
        g_signal_emit_by_name(launcher, "clicked");
        GListModel *windows = gtk_window_get_toplevels();
        for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i)
        {
            GtkWindow *candidate = g_list_model_get_item(windows, i);
            const char *title = gtk_window_get_title(candidate);
            if (candidate != parent && title != NULL && strcmp(title, "Cash planning") == 0)
            { host = candidate; break; }
            g_object_unref(candidate);
        }
        CHECK(host != NULL);
        CHECK(GTK_IS_SCROLLED_WINDOW(gtk_window_get_child(host)));
        root = umi_gtk4_automation_find_tagged_widget(gtk_window_get_child(host), "cash.plan.root");
        CHECK(root != NULL);
        g_object_ref(root);
    }
    else
    {
        OK(UmiGtk4CashPlanCreate(&root)); g_object_ref_sink(root);
    }
    CashPanel *panel = g_object_get_data(G_OBJECT(root), "umicom-cash-plan");
    CHECK(panel != NULL);
    Set(root, "opening", "1000.00"); Set(root, "buffer", "200.00");
    Set(root, "start", "2028-02-01"); Set(root, "end", "2028-02-29"); Click(root, "settings");
    char directory[UMI_PATH_CAPACITY], filename[UMI_PATH_CAPACITY];
    FixtureDirectory(directory); FixturePath(filename, directory, "plan.json");
    Set(root, "path", filename);
    if (strcmp(mode, "retired-actions") == 0 || strcmp(mode, "host-close") == 0 || strcmp(mode, "pickers") == 0)
    {
        UmiCashPlan *before = NULL; OK(UmiCashPlanCopy(panel->plan, &before));
        uint64_t revision = UmiCashPlanRevision(panel->plan);
        CHECK(gtk_widget_get_visible(Control(root, "browse-open")));
        CHECK(gtk_widget_get_visible(Control(root, "browse-save")));
        CHECK(gtk_widget_get_visible(Control(root, "browse-export")));
        if (host != NULL) gtk_window_destroy(host);
        else UmiGtk4CashPlanRetire(root);
        UmiGtk4CashPlanRetire(root); /* Idempotence with retained root and controls. */
        CHECK(panel->retired && !gtk_widget_get_sensitive(root));
        CHECK(!gtk_widget_get_visible(Control(root, "browse-open")));
        CHECK(!gtk_widget_get_visible(Control(root, "browse-save")));
        CHECK(!gtk_widget_get_visible(Control(root, "browse-export")));
        Set(root, "opening", "999.00"); Set(root, "id", "late"); Set(root, "label", "Late entry");
        Set(root, "amount", "5.00");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Control(root, "approval")), TRUE);
        const char *actions[] = {"settings", "add", "update", "remove", "undo", "reset", "load", "save", "export", "apply"};
        for (size_t i = 0U; i < sizeof actions / sizeof actions[0]; ++i) Click(root, actions[i]);
        CHECK(!panel->busy && panel->pending == NULL && UmiCashPlanRevision(panel->plan) == revision);
        CHECK(!g_file_test(filename, G_FILE_TEST_EXISTS)); Same(before, panel->plan); UmiCashPlanDestroy(before);
    }
    else if (strcmp(mode, "retire-before-save") == 0 || strcmp(mode, "retire-before-edit") == 0)
    {
        UmiCashPlan *before = NULL; OK(UmiCashPlanCopy(panel->plan, &before));
        if (strcmp(mode, "retire-before-edit") == 0) Set(root, "opening", "900.00");
        GtkWidget *approval = Control(root, "approval");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(approval), TRUE);
        g_signal_connect_object(approval, "toggled", G_CALLBACK(RetireOnApproval), root, 0);
        Click(root, strcmp(mode, "retire-before-save") == 0 ? "save" : "settings");
        CHECK(panel->retired && !panel->busy);
        CHECK(!g_file_test(filename, G_FILE_TEST_EXISTS)); Same(before, panel->plan); UmiCashPlanDestroy(before);
    }
    else
    {
        UmiCashPlan *loaded = Plan();
        UmiCashPlanConfig config = Config();
        if (strcmp(mode, "currency") == 0) memcpy(config.opening.currency.code, "USD", 4U);
        else if (strcmp(mode, "scale") == 0) config.opening.scale = 3U;
        else if (strcmp(mode, "range") == 0) config.end.day = 28U;
        OK(UmiCashPlanSetConfig(loaded, &config));
        if (strcmp(mode, "currency") != 0 && strcmp(mode, "scale") != 0)
            Add(loaded, "rent", 3, strcmp(mode, "overflow") == 0 ? INT64_MAX : 85000,
                strcmp(mode, "overflow") == 0);
        OK(UmiCashPlanSaveNew(filename, loaded, NULL)); UmiCashPlanDestroy(loaded);
        Click(root, "load"); CHECK(panel->busy);
        if (strcmp(mode, "retire-loading") == 0)
        {
            char *before = Output(root);
            UmiGtk4CashPlanRetire(root); Wait(panel);
            char *after = Output(root); CHECK(strcmp(before, after) == 0); g_free(before); g_free(after);
            CHECK(panel->pending == NULL && panel->retired);
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(panel->note)), "closed") != NULL);
        }
        else
        {
            Wait(panel); CHECK(panel->pending != NULL);
            Has(root, "LOADED PLAN COMPARISON (not applied)"); Has(root, "CURRENT STORED ASSUMPTIONS");
            Has(root, "LOADED ASSUMPTIONS (not applied)"); CHECK(UmiCashPlanCount(panel->plan) == 0U);
            if (strcmp(mode, "currency") == 0) Has(root, "Currency differs: current GBP | loaded USD.");
            else if (strcmp(mode, "scale") == 0) Has(root, "Decimal places differ: current 2 | loaded 3.");
            else if (strcmp(mode, "range") == 0) Has(root, "Date range differs:");
            else if (strcmp(mode, "overflow") == 0) Has(root, "No partial differences are shown.");
            else if (strcmp(mode, "review") == 0 || strcmp(mode, "apply-undo") == 0)
            {
                Has(root, "Closing: current 1000.00 | loaded 150.00 | change -850.00");
                Has(root, "First buffer shortfall: 2028-02-03");
                Has(root, "Entry IDs: added 1 | removed 0 | changed 0 | unchanged 0");
                Click(root, "apply"); CHECK(UmiCashPlanCount(panel->plan) == 0U);
                if (strcmp(mode, "apply-undo") == 0)
                {
                    gtk_check_button_set_active(GTK_CHECK_BUTTON(Control(root, "approval")), TRUE);
                    Click(root, "apply"); Has(root, "Closing: 150.00");
                    Click(root, "undo"); Has(root, "Closing: 1000.00");
                }
            }
            else return 2;
        }
    }
    if (host != NULL) { gtk_window_destroy(host); g_object_unref(host); }
    if (parent != NULL) { gtk_window_destroy(parent); g_object_unref(parent); }
    g_object_unref(root);
    return 0;
}
