/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_workspace_layout.c
 *
 * PURPOSE:
 *   Verify broker page identity, pane movement, layout restoration and retirement without network access.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/workspace_gtk4.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    if (strcmp(gtk_widget_get_name(root), name) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, name);
        if (found != NULL) return found;
    }
    return NULL;
}
static int Same(const UmiBrokerWorkspaceLayout *a, const UmiBrokerWorkspaceLayout *b)
{
    if (a->orientation != b->orientation || a->divider != b->divider) return 0;
    for (unsigned i = 0U; i < UMI_BROKER_PAGE_COUNT; ++i)
        if (a->pane[i] != b->pane[i] || a->order[i] != b->order[i]) return 0;
    return a->selected[0] == b->selected[0] && a->selected[1] == b->selected[1];
}
typedef struct Observation {
    UmiBrokerWorkspaceGtk *workspace;
    GtkWindow *parent;
    UmiStatus nested, copied;
    unsigned calls;
    int close;
    UmiBrokerWorkspaceLayout requested;
} Observation;
static void DuringCommand(Observation *state)
{
    ++state->calls;
    if (state->close == 2) {
        UmiBrokerWorkspaceGtkDestroy(state->workspace);
        state->workspace = NULL;
    } else if (state->close) gtk_window_destroy(state->parent);
    else {
        UmiBrokerWorkspaceLayout capture;
        state->nested = UmiBrokerWorkspaceGtkMovePage(state->workspace, UMI_BROKER_PAGE_MARKET, 1U);
        state->copied = UmiBrokerWorkspaceGtkCopyLayout(state->workspace, &capture);
        /* Restore must retain the caller's original intent before callbacks. */
        state->requested.pane[UMI_BROKER_PAGE_ACCOUNT] = 0U;
    }
}
static void PageChanged(GtkNotebook *book, GtkWidget *child, guint index, gpointer data)
{
    (void)book; (void)child; (void)index;
    DuringCommand(data);
}
static void PropertyChanged(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    DuringCommand(data);
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    const char *test = argv[1];
    GtkWindow *parent = GTK_WINDOW(gtk_window_new());
    g_object_ref(parent);
    UmiBrokerAccessPlan plan;
    CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_PAPER, &plan) == UMI_STATUS_OK);
    UmiBrokerWorkspaceGtk *workspace = NULL;
    CHECK(UmiBrokerWorkspaceGtkCreate(parent, &plan, &workspace) == UMI_STATUS_OK);
    GtkWidget *root = UmiBrokerWorkspaceGtkWidget(workspace);
    g_object_ref(root);
    gtk_window_set_child(parent, root);
    GtkNotebook *first = GTK_NOTEBOOK(Find(root, "broker-workspace-pages"));
    GtkNotebook *second = GTK_NOTEBOOK(Find(root, "broker-workspace-comparison"));
    GtkWidget *split = Find(root, "broker-workspace-split");
    CHECK(first != NULL && second != NULL && split != NULL);
    UmiBrokerWorkspaceLayout initial, changed, after;
    CHECK(UmiBrokerWorkspaceGtkCopyLayout(workspace, &initial) == UMI_STATUS_OK);
    changed = initial;
    Observation observer = {.workspace = workspace, .parent = parent, .requested = initial};
    guint windowCount = g_list_model_get_n_items(gtk_window_get_toplevels());
    if (!strcmp(test, "names")) {
        for (unsigned i = 0U; i < UMI_BROKER_PAGE_COUNT; ++i)
            CHECK(UmiBrokerWorkspacePageName((UmiBrokerWorkspacePage)i) != NULL);
        CHECK(UmiBrokerWorkspacePageName(UMI_BROKER_PAGE_COUNT) == NULL);
        CHECK(UmiBrokerWorkspacePageName((UmiBrokerWorkspacePage)-1) == NULL);
    } else if (!strcmp(test, "defaults")) {
        CHECK(gtk_notebook_get_n_pages(first) == UMI_BROKER_PAGE_COUNT - 1);
        CHECK(gtk_notebook_get_n_pages(second) == 1);
        CHECK(initial.selected[0] == UMI_BROKER_PAGE_ACCOUNT);
        CHECK(initial.selected[1] == UMI_BROKER_PAGE_CHART);
        CHECK(initial.orientation == GTK_ORIENTATION_HORIZONTAL);
    } else if (!strcmp(test, "show-chart") || !strcmp(test, "show-after-reorder")) {
        if (!strcmp(test, "show-after-reorder"))
            gtk_notebook_reorder_child(first, gtk_notebook_get_nth_page(first, 1), 7);
        CHECK(UmiBrokerWorkspaceGtkSelectPage(workspace,
            !strcmp(test, "show-chart") ? UMI_BROKER_PAGE_CHART : UMI_BROKER_PAGE_MARKET) == UMI_STATUS_OK);
        CHECK(UmiBrokerWorkspaceGtkCopyLayout(workspace, &after) == UMI_STATUS_OK);
        CHECK(!strcmp(test, "show-chart") ? after.selected[1] == UMI_BROKER_PAGE_CHART :
            after.selected[0] == UMI_BROKER_PAGE_MARKET);
    } else if (!strcmp(test, "move-account") || !strcmp(test, "move-chart")) {
        UmiBrokerWorkspacePage page = !strcmp(test, "move-account") ?
            UMI_BROKER_PAGE_ACCOUNT : UMI_BROKER_PAGE_CHART;
        unsigned target = page == UMI_BROKER_PAGE_ACCOUNT ? 1U : 0U;
        GtkWidget *original = gtk_notebook_get_nth_page(page == UMI_BROKER_PAGE_ACCOUNT ? first : second, 0);
        CHECK(UmiBrokerWorkspaceGtkMovePage(workspace, page, target) == UMI_STATUS_OK);
        CHECK(UmiBrokerWorkspaceGtkCopyLayout(workspace, &after) == UMI_STATUS_OK);
        CHECK(after.pane[page] == target && after.selected[target] == page);
        CHECK(gtk_notebook_page_num(target == 0U ? first : second, original) >= 0);
        CHECK(UmiBrokerWorkspaceGtkMovePage(workspace, page, target) == UMI_STATUS_OK);
    } else if (!strcmp(test, "move-all")) {
        for (unsigned page = 0U; page < UMI_BROKER_PAGE_COUNT; ++page)
            CHECK(UmiBrokerWorkspaceGtkMovePage(workspace, (UmiBrokerWorkspacePage)page, 1U) == UMI_STATUS_OK);
        CHECK(!gtk_widget_get_visible(GTK_WIDGET(first)) && gtk_widget_get_visible(GTK_WIDGET(second)));
        CHECK(UmiBrokerWorkspaceGtkCopyLayout(workspace, &after) == UMI_STATUS_OK);
        CHECK(after.selected[0] == UMI_BROKER_PAGE_COUNT);
        CHECK(UmiBrokerWorkspaceGtkMovePage(workspace, UMI_BROKER_PAGE_ACCOUNT, 0U) == UMI_STATUS_OK);
        CHECK(gtk_widget_get_visible(GTK_WIDGET(first)));
    } else if (!strcmp(test, "restore") || !strcmp(test, "restore-reordered") ||
               !strcmp(test, "restore-snapshot")) {
        CHECK(UmiBrokerWorkspaceGtkMovePage(workspace, UMI_BROKER_PAGE_MARKET, 1U) == UMI_STATUS_OK);
        CHECK(UmiBrokerWorkspaceGtkSetSplit(workspace, GTK_ORIENTATION_VERTICAL) == UMI_STATUS_OK);
        gtk_paned_set_position(GTK_PANED(split), 317);
        if (!strcmp(test, "restore-reordered"))
            gtk_notebook_reorder_child(first, gtk_notebook_get_nth_page(first, 0), 5);
        CHECK(UmiBrokerWorkspaceGtkCopyLayout(workspace, &changed) == UMI_STATUS_OK);
        CHECK(UmiBrokerWorkspaceGtkApplyLayout(workspace, &initial) == UMI_STATUS_OK);
        CHECK(UmiBrokerWorkspaceGtkCopyLayout(workspace, &after) == UMI_STATUS_OK && Same(&initial, &after));
        if (!strcmp(test, "restore-snapshot")) {
            CHECK(UmiBrokerWorkspaceGtkApplyLayout(workspace, &changed) == UMI_STATUS_OK);
            CHECK(UmiBrokerWorkspaceGtkCopyLayout(workspace, &after) == UMI_STATUS_OK && Same(&changed, &after));
        }
    } else if (!strcmp(test, "split")) {
        CHECK(UmiBrokerWorkspaceGtkSetSplit(workspace, GTK_ORIENTATION_VERTICAL) == UMI_STATUS_OK);
        CHECK(gtk_orientable_get_orientation(GTK_ORIENTABLE(split)) == GTK_ORIENTATION_VERTICAL);
        CHECK(UmiBrokerWorkspaceGtkSetSplit(workspace, GTK_ORIENTATION_HORIZONTAL) == UMI_STATUS_OK);
    } else if (!strcmp(test, "toolbar-show") || !strcmp(test, "toolbar-move")) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "broker-workspace-page-choice")), UMI_BROKER_PAGE_MARKET);
        g_signal_emit_by_name(Find(root, !strcmp(test, "toolbar-show") ?
            "broker-workspace-page-show" : "broker-workspace-page-move"), "clicked");
        CHECK(UmiBrokerWorkspaceGtkCopyLayout(workspace, &after) == UMI_STATUS_OK);
        CHECK(after.selected[!strcmp(test, "toolbar-show") ? 0 : 1] == UMI_BROKER_PAGE_MARKET);
    } else if (!strcmp(test, "toolbar-split")) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "broker-workspace-split-choice")), 1U);
        CHECK(gtk_orientable_get_orientation(GTK_ORIENTABLE(split)) == GTK_ORIENTATION_VERTICAL);
    } else if (!strcmp(test, "retained")) {
        UmiBrokerWorkspaceGtkDestroy(workspace); workspace = NULL;
        g_signal_emit_by_name(Find(root, "broker-workspace-page-show"), "clicked");
        g_signal_emit_by_name(Find(root, "broker-workspace-page-move"), "clicked");
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "broker-workspace-split-choice")), 1U);
    } else if (!strcmp(test, "move-close") || !strcmp(test, "apply-close") ||
               !strcmp(test, "select-close") || !strcmp(test, "split-close") ||
               !strcmp(test, "move-reenter") || !strcmp(test, "apply-reenter") ||
               !strcmp(test, "move-release") || !strcmp(test, "apply-release")) {
        observer.close = strstr(test, "release") != NULL ? 2 : strstr(test, "close") != NULL;
        gboolean applying = g_str_has_prefix(test, "apply");
        observer.requested.pane[UMI_BROKER_PAGE_ACCOUNT] = 1U;
        observer.requested.selected[0] = UMI_BROKER_PAGE_MARKET;
        observer.requested.selected[1] = UMI_BROKER_PAGE_ACCOUNT;
        UmiStatus result;
        if (g_str_has_prefix(test, "select")) {
            g_signal_connect(first, "switch-page", G_CALLBACK(PageChanged), &observer);
            result = UmiBrokerWorkspaceGtkSelectPage(workspace, UMI_BROKER_PAGE_MARKET);
        } else if (g_str_has_prefix(test, "split")) {
            g_signal_connect(split, "notify::orientation", G_CALLBACK(PropertyChanged), &observer);
            result = UmiBrokerWorkspaceGtkSetSplit(workspace, GTK_ORIENTATION_VERTICAL);
        } else {
            g_signal_connect(first, "page-removed", G_CALLBACK(PageChanged), &observer);
            result = applying ? UmiBrokerWorkspaceGtkApplyLayout(workspace, &observer.requested) :
                UmiBrokerWorkspaceGtkMovePage(workspace, UMI_BROKER_PAGE_ACCOUNT, 1U);
        }
        CHECK(observer.calls != 0U);
        CHECK(result == (observer.close ? UMI_STATUS_CANCELLED : UMI_STATUS_OK));
        if (!observer.close) {
            CHECK(observer.nested == UMI_STATUS_BUSY && observer.copied == UMI_STATUS_BUSY);
            CHECK(UmiBrokerWorkspaceGtkCopyLayout(workspace, &after) == UMI_STATUS_OK);
            CHECK(after.pane[UMI_BROKER_PAGE_ACCOUNT] == 1U);
        } else if (observer.close == 2) {
            workspace = NULL; /* The command retained its owner through retirement. */
        } else {
            after = initial;
            CHECK(UmiBrokerWorkspaceGtkCopyLayout(workspace, &after) == UMI_STATUS_CANCELLED);
            CHECK(Same(&initial, &after));
        }
        g_signal_handlers_disconnect_by_data(first, &observer);
        g_signal_handlers_disconnect_by_data(split, &observer);
    } else {
        UmiStatus result = UMI_STATUS_OK;
        if (!strcmp(test, "invalid-page"))
            result = UmiBrokerWorkspaceGtkSelectPage(workspace, (UmiBrokerWorkspacePage)-1);
        else if (!strcmp(test, "invalid-pane"))
            result = UmiBrokerWorkspaceGtkMovePage(workspace, UMI_BROKER_PAGE_ACCOUNT, 2U);
        else if (!strcmp(test, "invalid-orientation"))
            result = UmiBrokerWorkspaceGtkSetSplit(workspace, (GtkOrientation)99);
        else if (!strcmp(test, "null-output"))
            result = UmiBrokerWorkspaceGtkCopyLayout(workspace, NULL);
        else if (!strcmp(test, "invalid-plan-null"))
            result = UmiBrokerWorkspaceGtkApplyLayout(workspace, NULL);
        else {
            if (!strcmp(test, "invalid-duplicate")) changed.order[1] = changed.order[0];
            else if (!strcmp(test, "invalid-selection")) changed.selected[0] = UMI_BROKER_PAGE_CHART;
            else if (!strcmp(test, "invalid-empty-selection")) changed.selected[1] = UMI_BROKER_PAGE_COUNT;
            else if (!strcmp(test, "invalid-divider")) changed.divider = -1;
            else if (!strcmp(test, "invalid-enum")) changed.orientation = (GtkOrientation)99;
            else if (!strcmp(test, "invalid-order")) changed.order[0] = UMI_BROKER_PAGE_COUNT;
            else return 2;
            result = UmiBrokerWorkspaceGtkApplyLayout(workspace, &changed);
        }
        CHECK(result == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrokerWorkspaceGtkCopyLayout(workspace, &after) == UMI_STATUS_OK && Same(&initial, &after));
    }
    if (workspace != NULL) UmiBrokerWorkspaceGtkDestroy(workspace);
    CHECK(observer.close || g_list_model_get_n_items(gtk_window_get_toplevels()) == windowCount);
    gtk_window_destroy(parent);
    g_object_unref(root); g_object_unref(parent);
    return 0;
}
