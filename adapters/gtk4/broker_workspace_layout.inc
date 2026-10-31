/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_layout.inc
 *
 * PURPOSE:
 *   Keep broker pages identifiable and resizable while preserving their observations and native ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static const char *const WorkspacePageNames[UMI_BROKER_PAGE_COUNT + 1] = {
    "Account and positions", "Market data", "Instrument", "Options",
    "Open orders", "Completed orders", "Trades", "Chart", "Market depth",
    "Time & Sales", "Profit and loss", "Scanner", NULL
};
const char *UmiBrokerWorkspacePageName(UmiBrokerWorkspacePage page)
{
    return (unsigned)page < UMI_BROKER_PAGE_COUNT ? WorkspacePageNames[page] : NULL;
}
static int WorkspacePagePane(UmiBrokerWorkspaceGtk *owner, UmiBrokerWorkspacePage page)
{
    for (unsigned pane = 0U; pane < 2U; ++pane)
        if (gtk_notebook_page_num(GTK_NOTEBOOK(owner->panes[pane]),
                owner->pageWidgets[page]) >= 0) return (int)pane;
    return -1;
}
static UmiStatus WorkspaceLayoutEnter(UmiBrokerWorkspaceGtk *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->closed) return UMI_STATUS_CANCELLED;
    if (owner->layoutBusy) return UMI_STATUS_BUSY;
    owner->layoutBusy = TRUE;
    ++owner->references;
    return UMI_STATUS_OK;
}
static UmiStatus WorkspaceLayoutLeave(UmiBrokerWorkspaceGtk *owner)
{
    UmiStatus result = owner->closed ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
    owner->layoutBusy = FALSE;
    Release(owner);
    return result;
}
static void WorkspacePaneVisibility(UmiBrokerWorkspaceGtk *owner)
{
    for (unsigned pane = 0U; pane < 2U && !owner->closed; ++pane)
        gtk_widget_set_visible(owner->panes[pane],
            gtk_notebook_get_n_pages(GTK_NOTEBOOK(owner->panes[pane])) != 0);
}
/* Each page has a controller reference as well as its notebook's reference.
 * Reparenting therefore preserves inputs, reports and signal connections even
 * while a native page-removed observer runs. Retirement still stops the command. */
static void WorkspaceMoveOwned(UmiBrokerWorkspaceGtk *owner,
    UmiBrokerWorkspacePage page, unsigned pane)
{
    int previous = WorkspacePagePane(owner, page);
    if (previous == (int)pane) return;
    GtkWidget *widget = owner->pageWidgets[page];
    if (previous >= 0) {
        GtkNotebook *from = GTK_NOTEBOOK(owner->panes[previous]);
        int index = gtk_notebook_page_num(from, widget);
        gtk_notebook_remove_page(from, index);
        if (owner->closed) return;
    }
    GtkNotebook *to = GTK_NOTEBOOK(owner->panes[pane]);
    gtk_notebook_append_page(to, widget, gtk_label_new(WorkspacePageNames[page]));
    if (owner->closed) return;
    gtk_notebook_set_tab_reorderable(to, widget, TRUE);
}
UmiStatus UmiBrokerWorkspaceGtkSelectPage(UmiBrokerWorkspaceGtk *owner,
    UmiBrokerWorkspacePage page)
{
    if ((unsigned)page >= UMI_BROKER_PAGE_COUNT) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = WorkspaceLayoutEnter(owner);
    if (status != UMI_STATUS_OK) return status;
    int pane = WorkspacePagePane(owner, page);
    if (pane >= 0) {
        GtkNotebook *book = GTK_NOTEBOOK(owner->panes[pane]);
        gtk_notebook_set_current_page(book, gtk_notebook_page_num(book, owner->pageWidgets[page]));
        if (!owner->closed) gtk_widget_grab_focus(GTK_WIDGET(book));
    }
    status = WorkspaceLayoutLeave(owner);
    return status != UMI_STATUS_OK ? status : pane < 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
}
UmiStatus UmiBrokerWorkspaceGtkMovePage(UmiBrokerWorkspaceGtk *owner,
    UmiBrokerWorkspacePage page, unsigned pane)
{
    if ((unsigned)page >= UMI_BROKER_PAGE_COUNT || pane > 1U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = WorkspaceLayoutEnter(owner);
    if (status != UMI_STATUS_OK) return status;
    WorkspaceMoveOwned(owner, page, pane);
    if (!owner->closed) WorkspacePaneVisibility(owner);
    if (!owner->closed) {
        GtkNotebook *book = GTK_NOTEBOOK(owner->panes[pane]);
        gtk_notebook_set_current_page(book, gtk_notebook_page_num(book, owner->pageWidgets[page]));
    }
    return WorkspaceLayoutLeave(owner);
}
UmiStatus UmiBrokerWorkspaceGtkSetSplit(UmiBrokerWorkspaceGtk *owner,
    GtkOrientation orientation)
{
    if (orientation != GTK_ORIENTATION_HORIZONTAL && orientation != GTK_ORIENTATION_VERTICAL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = WorkspaceLayoutEnter(owner);
    if (status != UMI_STATUS_OK) return status;
    gtk_orientable_set_orientation(GTK_ORIENTABLE(owner->split), orientation);
    if (!owner->closed) gtk_drop_down_set_selected(GTK_DROP_DOWN(owner->splitChoice),
        orientation == GTK_ORIENTATION_HORIZONTAL ? 0U : 1U);
    return WorkspaceLayoutLeave(owner);
}
UmiStatus UmiBrokerWorkspaceGtkCopyLayout(UmiBrokerWorkspaceGtk *owner,
    UmiBrokerWorkspaceLayout *out)
{
    if (owner == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->closed) return UMI_STATUS_CANCELLED;
    if (owner->layoutBusy) return UMI_STATUS_BUSY;
    UmiBrokerWorkspaceLayout result = {0};
    size_t used = 0U;
    result.orientation = gtk_orientable_get_orientation(GTK_ORIENTABLE(owner->split));
    result.divider = gtk_paned_get_position(GTK_PANED(owner->split));
    for (unsigned pane = 0U; pane < 2U; ++pane) {
        GtkNotebook *book = GTK_NOTEBOOK(owner->panes[pane]);
        result.selected[pane] = UMI_BROKER_PAGE_COUNT;
        int current = gtk_notebook_get_current_page(book);
        for (int index = 0; index < gtk_notebook_get_n_pages(book); ++index) {
            GtkWidget *widget = gtk_notebook_get_nth_page(book, index);
            unsigned page = 0U;
            while (page < UMI_BROKER_PAGE_COUNT && owner->pageWidgets[page] != widget) ++page;
            if (page == UMI_BROKER_PAGE_COUNT || used == UMI_BROKER_PAGE_COUNT)
                return UMI_STATUS_INVALID_STATE;
            result.pane[page] = pane;
            result.order[used++] = (UmiBrokerWorkspacePage)page;
            if (index == current) result.selected[pane] = (UmiBrokerWorkspacePage)page;
        }
    }
    if (used != UMI_BROKER_PAGE_COUNT) return UMI_STATUS_INVALID_STATE;
    *out = result;
    return UMI_STATUS_OK;
}
static gboolean WorkspaceLayoutValid(const UmiBrokerWorkspaceLayout *layout)
{
    if (layout == NULL || layout->divider < 0 ||
        (layout->orientation != GTK_ORIENTATION_HORIZONTAL &&
         layout->orientation != GTK_ORIENTATION_VERTICAL)) return FALSE;
    gboolean seen[UMI_BROKER_PAGE_COUNT] = {FALSE};
    unsigned count[2] = {0U, 0U};
    for (unsigned i = 0U; i < UMI_BROKER_PAGE_COUNT; ++i) {
        unsigned page = (unsigned)layout->order[i];
        if (page >= UMI_BROKER_PAGE_COUNT || seen[page] || layout->pane[page] > 1U) return FALSE;
        seen[page] = TRUE;
        ++count[layout->pane[page]];
    }
    for (unsigned pane = 0U; pane < 2U; ++pane) {
        unsigned page = (unsigned)layout->selected[pane];
        if (count[pane] == 0U) {
            if (page != UMI_BROKER_PAGE_COUNT) return FALSE;
        } else if (page >= UMI_BROKER_PAGE_COUNT || layout->pane[page] != pane) return FALSE;
    }
    return TRUE;
}
UmiStatus UmiBrokerWorkspaceGtkApplyLayout(UmiBrokerWorkspaceGtk *owner,
    const UmiBrokerWorkspaceLayout *layout)
{
    if (!WorkspaceLayoutValid(layout)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Copy before publishing: callbacks may edit or release the caller's plan. */
    UmiBrokerWorkspaceLayout requested = *layout;
    UmiStatus status = WorkspaceLayoutEnter(owner);
    if (status != UMI_STATUS_OK) return status;
    int position[2] = {0, 0};
    for (unsigned i = 0U; i < UMI_BROKER_PAGE_COUNT && !owner->closed; ++i) {
        UmiBrokerWorkspacePage page = requested.order[i];
        unsigned pane = requested.pane[page];
        WorkspaceMoveOwned(owner, page, pane);
        if (!owner->closed)
            gtk_notebook_reorder_child(GTK_NOTEBOOK(owner->panes[pane]),
                owner->pageWidgets[page], position[pane]++);
    }
    if (!owner->closed)
        gtk_orientable_set_orientation(GTK_ORIENTABLE(owner->split), requested.orientation);
    if (!owner->closed)
        gtk_drop_down_set_selected(GTK_DROP_DOWN(owner->splitChoice),
            requested.orientation == GTK_ORIENTATION_HORIZONTAL ? 0U : 1U);
    if (!owner->closed) gtk_paned_set_position(GTK_PANED(owner->split), requested.divider);
    if (!owner->closed) WorkspacePaneVisibility(owner);
    for (unsigned pane = 0U; pane < 2U && !owner->closed; ++pane) {
        if (requested.selected[pane] == UMI_BROKER_PAGE_COUNT) continue;
        GtkNotebook *book = GTK_NOTEBOOK(owner->panes[pane]);
        gtk_notebook_set_current_page(book,
            gtk_notebook_page_num(book, owner->pageWidgets[requested.selected[pane]]));
    }
    return WorkspaceLayoutLeave(owner);
}
static void WorkspaceShowPage(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    UmiBrokerWorkspacePage page = (UmiBrokerWorkspacePage)
        gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->pageChoice));
    (void)UmiBrokerWorkspaceGtkSelectPage(owner, page);
}
static void WorkspaceMovePage(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    UmiBrokerWorkspacePage page = (UmiBrokerWorkspacePage)
        gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->pageChoice));
    if ((unsigned)page >= UMI_BROKER_PAGE_COUNT) return;
    int pane = WorkspacePagePane(owner, page);
    if (pane >= 0) (void)UmiBrokerWorkspaceGtkMovePage(owner, page, pane == 0 ? 1U : 0U);
}
static void WorkspaceSplitChanged(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)spec;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->layoutBusy) return;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    if (selected <= 1U) (void)UmiBrokerWorkspaceGtkSetSplit(owner,
        selected == 0U ? GTK_ORIENTATION_HORIZONTAL : GTK_ORIENTATION_VERTICAL);
}
static void WorkspaceLayoutCreate(UmiBrokerWorkspaceGtk *owner, GtkWidget *account)
{
    GtkWidget *controls = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    owner->pageChoice = gtk_drop_down_new_from_strings(WorkspacePageNames);
    gtk_widget_set_name(owner->pageChoice, "broker-workspace-page-choice");
    gtk_widget_set_tooltip_text(owner->pageChoice, "Choose a broker page");
    owner->pageShow = gtk_button_new_with_label("Show");
    gtk_widget_set_name(owner->pageShow, "broker-workspace-page-show");
    owner->pageMove = gtk_button_new_with_label("Move to other pane");
    gtk_widget_set_name(owner->pageMove, "broker-workspace-page-move");
    const char *splits[] = {"Side by side", "Stacked", NULL};
    owner->splitChoice = gtk_drop_down_new_from_strings(splits);
    gtk_widget_set_name(owner->splitChoice, "broker-workspace-split-choice");
    gtk_widget_set_tooltip_text(owner->splitChoice, "Arrange the two resizable panes");
    gtk_box_append(GTK_BOX(controls), owner->pageChoice);
    gtk_box_append(GTK_BOX(controls), owner->pageShow);
    gtk_box_append(GTK_BOX(controls), owner->pageMove);
    gtk_box_append(GTK_BOX(controls), owner->splitChoice);
    GtkWidget *toolbar = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(toolbar),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(toolbar), controls);
    gtk_box_append(GTK_BOX(owner->root), toolbar);
    owner->split = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_name(owner->split, "broker-workspace-split");
    gtk_widget_set_vexpand(owner->split, TRUE);
    for (unsigned pane = 0U; pane < 2U; ++pane) {
        owner->panes[pane] = gtk_notebook_new();
        gtk_widget_set_name(owner->panes[pane],
            pane == 0U ? "broker-workspace-pages" : "broker-workspace-comparison");
        gtk_notebook_set_scrollable(GTK_NOTEBOOK(owner->panes[pane]), TRUE);
    }
    gtk_paned_set_start_child(GTK_PANED(owner->split), owner->panes[0]);
    gtk_paned_set_end_child(GTK_PANED(owner->split), owner->panes[1]);
    gtk_paned_set_resize_start_child(GTK_PANED(owner->split), TRUE);
    gtk_paned_set_resize_end_child(GTK_PANED(owner->split), TRUE);
    gtk_paned_set_position(GTK_PANED(owner->split), 520);
    /* Construct in dependency order: the shared instrument controls exist
     * before pages that use their selection. C initializer evaluation order
     * must not decide the order in which callbacks become available. */
    GtkWidget *content[UMI_BROKER_PAGE_COUNT];
    content[0] = account;
    content[1] = WorkspaceQuotePage(owner);
    content[2] = WorkspaceDetailsPage(owner);
    content[3] = WorkspaceOptionsPage(owner);
    content[4] = WorkspaceOrdersPage(owner);
    content[5] = WorkspaceCompletedPage(owner);
    content[6] = WorkspaceTradesPage(owner);
    /* The original finite chart remains inside the chart mode container.
     * Both modes retain their own queries and observations when switched. */
#if 0
    content[7] = WorkspaceHistoryPage(owner);
#endif
    content[7] = WorkspaceChartPage(owner);
    content[8] = WorkspaceDepthPage(owner);
    content[9] = WorkspaceTapePage(owner);
    content[10] = WorkspacePnlPage(owner);
    content[11] = WorkspaceScannerPage(owner);
    for (unsigned page = 0U; page < UMI_BROKER_PAGE_COUNT; ++page) {
        /* The page viewport permits narrow panels without clipping their
         * controls. Its stable identity survives tab reordering and relocation. */
        GtkWidget *viewport = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(viewport),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(viewport), content[page]);
        owner->pageWidgets[page] = g_object_ref_sink(viewport);
        WorkspaceMoveOwned(owner, (UmiBrokerWorkspacePage)page,
            page == UMI_BROKER_PAGE_CHART ? 1U : 0U);
    }
    gtk_box_append(GTK_BOX(owner->root), owner->split);
    g_signal_connect(owner->pageShow, "clicked", G_CALLBACK(WorkspaceShowPage), owner);
    g_signal_connect(owner->pageMove, "clicked", G_CALLBACK(WorkspaceMovePage), owner);
    g_signal_connect(owner->splitChoice, "notify::selected", G_CALLBACK(WorkspaceSplitChanged), owner);
}
