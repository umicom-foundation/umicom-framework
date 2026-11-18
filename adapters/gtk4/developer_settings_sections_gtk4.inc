/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_settings_sections_gtk4.inc
 *
 * PURPOSE:
 *   Keep build settings controls in persistent, resizable task sections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static const char *const DeveloperSettingsNames[] = {
    "build", "tools", "presets", "run", "targets", "sdk", "intelligence", "configurations"
};
static const char *const DeveloperSettingsTitles[] = {
    "Build", "Tools", "Presets", "Run and Debug", "Configured targets",
    "SDK and Install", "Code intelligence", "Saved configurations"
};

/* Each section owns a scroll position and retains its controls. The outer form
 * keeps Apply/Cancel and feedback visible while the resizable navigation pane
 * selects a small, task-specific set of inputs. No application model is copied
 * or reconstructed when the user changes sections. */
static void DeveloperSettingsSectionsBegin(UmiGtk4DeveloperDialog *dialog)
{
    GtkWidget *host = dialog->body;
    dialog->settingsStack = gtk_stack_new();
    gtk_stack_set_hhomogeneous(GTK_STACK(dialog->settingsStack), FALSE);
    gtk_stack_set_vhomogeneous(GTK_STACK(dialog->settingsStack), FALSE);
    gtk_stack_set_transition_type(GTK_STACK(dialog->settingsStack), GTK_STACK_TRANSITION_TYPE_NONE);
    (void)umi_gtk4_automation_tag_widget(dialog->settingsStack, "developer.settings.sections");
    for (size_t i = 0U; i < UMI_GTK4_BUILD_SETTINGS_SECTION_COUNT; ++i) {
        GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        GtkWidget *scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_widget_set_hexpand(scroll, TRUE);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_widget_set_margin_start(page, 12);
        gtk_widget_set_margin_end(page, 12);
        gtk_widget_set_margin_top(page, 8);
        gtk_widget_set_margin_bottom(page, 12);
        char tag[96];
        g_snprintf(tag, sizeof(tag), "developer.settings.section.%s", DeveloperSettingsNames[i]);
        (void)umi_gtk4_automation_tag_widget(page, tag);
        GtkWidget *heading = gtk_label_new(DeveloperSettingsTitles[i]);
        gtk_label_set_xalign(GTK_LABEL(heading), 0.0F);
        gtk_widget_add_css_class(heading, "title-3");
        gtk_box_append(GTK_BOX(page), heading);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), page);
        /* Keep keyboard and explicit review focus inside the visible page.
         * GtkScrolledWindow inserts this viewport for the non-scrollable box.
         * This changes navigation only; it never changes any field value. */
        GtkWidget *viewport = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scroll));
        if (GTK_IS_VIEWPORT(viewport))
            gtk_viewport_set_scroll_to_focus(GTK_VIEWPORT(viewport), TRUE);
        gtk_stack_add_titled(GTK_STACK(dialog->settingsStack), scroll,
            DeveloperSettingsNames[i], DeveloperSettingsTitles[i]);
        dialog->settingsSections[i] = page;
    }
    GtkWidget *sidebar = gtk_stack_sidebar_new();
    gtk_stack_sidebar_set_stack(GTK_STACK_SIDEBAR(sidebar), GTK_STACK(dialog->settingsStack));
    (void)umi_gtk4_automation_tag_widget(sidebar, "developer.settings.navigation");
    GtkWidget *navigation = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(navigation),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(navigation), sidebar);
    GtkWidget *split = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_start_child(GTK_PANED(split), navigation);
    gtk_paned_set_end_child(GTK_PANED(split), dialog->settingsStack);
    gtk_paned_set_resize_start_child(GTK_PANED(split), FALSE);
    gtk_paned_set_resize_end_child(GTK_PANED(split), TRUE);
    gtk_paned_set_shrink_start_child(GTK_PANED(split), TRUE);
    gtk_paned_set_shrink_end_child(GTK_PANED(split), TRUE);
    gtk_paned_set_position(GTK_PANED(split), 185);
    gtk_widget_set_hexpand(split, TRUE);
    gtk_widget_set_vexpand(split, TRUE);
    gtk_widget_set_vexpand(host, TRUE);
    (void)umi_gtk4_automation_tag_widget(split, "developer.settings.split");
    gtk_box_append(GTK_BOX(host), split);
    /* The existing shell's scroll window now holds a viewport, not the entire
     * settings inventory. Inner pages provide scrolling independently. */
    GtkWidget *outer = gtk_widget_get_ancestor(host, GTK_TYPE_SCROLLED_WINDOW);
    if (GTK_IS_SCROLLED_WINDOW(outer))
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(outer),
            GTK_POLICY_NEVER, GTK_POLICY_NEVER);
    gtk_window_set_default_size(dialog->window, 920, 700);
}
static void DeveloperSettingsSectionBody(UmiGtk4DeveloperDialog *dialog,
    UmiGtk4BuildSettingsSection section)
{
    dialog->body = dialog->settingsSections[section];
}
UmiStatus UmiGtk4BuildSettingsCurrentSection(const UmiGtk4DeveloperDialog *dialog,
    UmiGtk4BuildSettingsSection *outSection)
{
    if (dialog == NULL || outSection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!DeveloperAlive(dialog) || dialog->settingsStack == NULL) return UMI_STATUS_INVALID_STATE;
    const char *name = gtk_stack_get_visible_child_name(GTK_STACK(dialog->settingsStack));
    for (size_t i = 0U; name != NULL && i < UMI_GTK4_BUILD_SETTINGS_SECTION_COUNT; ++i)
        if (strcmp(name, DeveloperSettingsNames[i]) == 0) {
            *outSection = (UmiGtk4BuildSettingsSection)i;
            return UMI_STATUS_OK;
        }
    return UMI_STATUS_NOT_FOUND;
}
UmiStatus UmiGtk4BuildSettingsSelectSection(UmiGtk4DeveloperDialog *dialog,
    UmiGtk4BuildSettingsSection section)
{
    if (dialog == NULL || section < UMI_GTK4_BUILD_SETTINGS_BUILD ||
        section >= UMI_GTK4_BUILD_SETTINGS_SECTION_COUNT) return UMI_STATUS_INVALID_ARGUMENT;
    if (!DeveloperAlive(dialog) || dialog->settingsStack == NULL) return UMI_STATUS_INVALID_STATE;
    if (dialog->settingsSelecting) return UMI_STATUS_BUSY;
    ++dialog->references;
    dialog->settingsSelecting = 1;
    gtk_stack_set_visible_child_name(GTK_STACK(dialog->settingsStack), DeveloperSettingsNames[section]);
    UmiStatus status = DeveloperAlive(dialog) ? UMI_STATUS_OK : UMI_STATUS_CANCELLED;
    dialog->settingsSelecting = 0;
    DeveloperRelease(dialog);
    return status;
}

/* Focusing before GTK allocates a newly shown page does not necessarily scroll
 * it. A bounded frame callback reveals only that explicitly focused control.
 * Weak window references avoid retaining the form or its application owner. */
typedef struct DeveloperReviewReveal {
    GWeakRef window;
    GWeakRef parent;
    unsigned frames;
} DeveloperReviewReveal;
static void DeveloperReviewRevealFree(gpointer data)
{
    DeveloperReviewReveal *request = data;
    g_weak_ref_clear(&request->window);
    g_weak_ref_clear(&request->parent);
    g_free(request);
}
static gboolean DeveloperReviewRevealTick(GtkWidget *target, GdkFrameClock *clock, gpointer data)
{
    (void)clock;
    DeveloperReviewReveal *request = data;
    GtkWindow *window = g_weak_ref_get(&request->window);
    GtkWindow *parent = g_weak_ref_get(&request->parent);
    gboolean owned = window != NULL && parent != NULL &&
        UmiGtk4WindowIsOpen(window) && UmiGtk4WindowIsOpen(parent) &&
        gtk_widget_get_root(target) == GTK_ROOT(window) && gtk_window_get_focus(window) == target;
    g_clear_object(&window); g_clear_object(&parent);
    if (!owned) goto finished;
    if (++request->frames == 1U) return G_SOURCE_CONTINUE;
    GtkWidget *viewport = gtk_widget_get_ancestor(target, GTK_TYPE_VIEWPORT);
    /* Measure in viewport coordinates so its content margins are included. */
    GtkAdjustment *adjustment = viewport != NULL
        ? gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(viewport)) : NULL;
    graphene_rect_t bounds;
    double page = adjustment != NULL ? gtk_adjustment_get_page_size(adjustment) : 0.0;
    if (viewport == NULL || page <= 0.0 || gtk_widget_get_height(target) <= 0 ||
        !gtk_widget_compute_bounds(target, viewport, &bounds)) {
        if (request->frames < 16U) return G_SOURCE_CONTINUE;
        goto finished;
    }
    double value = gtk_adjustment_get_value(adjustment);
    /* Retain the earlier content-relative range for review: it omitted the
     * page margin and could leave the last pixels below the visible edge. */
#if 0
    double first = (double)bounds.origin.y, last = first + (double)bounds.size.height;
#endif
    double first = value + (double)bounds.origin.y, last = first + (double)bounds.size.height;
    double next = value;
    if (first < value || last - first > page) next = first;
    else if (last > value + page) next = last - page;
    double lower = gtk_adjustment_get_lower(adjustment);
    next = CLAMP(next, lower, MAX(lower, gtk_adjustment_get_upper(adjustment) - page));
    /* Clear ownership before notifying: value-changed can destroy or replace
     * the form. The retained adjustment is the only object used afterwards. */
    g_object_set_data(G_OBJECT(target), "umicom-review-reveal", NULL);
    g_object_ref(adjustment);
    if (value != next) gtk_adjustment_set_value(adjustment, next);
    g_object_unref(adjustment);
    return G_SOURCE_REMOVE;
finished:
    g_object_set_data(G_OBJECT(target), "umicom-review-reveal", NULL);
    return G_SOURCE_REMOVE;
}
static void DeveloperReviewRevealSchedule(UmiGtk4DeveloperDialog *dialog)
{
    GtkWidget *target = dialog->trust;
    guint previous = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(target), "umicom-review-reveal"));
    if (previous != 0U) gtk_widget_remove_tick_callback(target, previous);
    DeveloperReviewReveal *request = g_new0(DeveloperReviewReveal, 1);
    GObject *parent = g_weak_ref_get(&dialog->parentWindow);
    g_weak_ref_init(&request->window, dialog->window);
    g_weak_ref_init(&request->parent, parent);
    g_clear_object(&parent);
    guint tick = gtk_widget_add_tick_callback(target, DeveloperReviewRevealTick, request, DeveloperReviewRevealFree);
    g_object_set_data(G_OBJECT(target), "umicom-review-reveal", GUINT_TO_POINTER(tick));
}

/* A refused command needs a readable recovery path. Keep the explanation in
 * the existing pinned status area and focus the unchecked trust control.
 * Native observers can retire the form at each GTK call, so one internal lease
 * covers the sequence and no child is used after ownership has been revoked. */
UmiStatus UmiGtk4BuildSettingsReviewExecution(UmiGtk4DeveloperDialog *dialog)
{
    if (dialog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!DeveloperAlive(dialog) || dialog->settingsStack == NULL || dialog->trust == NULL)
        return UMI_STATUS_INVALID_STATE;
    if (dialog->settingsSelecting || dialog->accepting) return UMI_STATUS_BUSY;
    ++dialog->references;
    dialog->settingsSelecting = 1;
    gtk_stack_set_visible_child_name(GTK_STACK(dialog->settingsStack), "build");
    if (DeveloperAlive(dialog))
        ShowStatus(dialog, UMI_STATUS_PERMISSION_DENIED,
            "No build or program was started. Review this project's code and build scripts. "
            "If you trust them, select the trust checkbox, choose Apply Settings, then retry your command. "
            "Cancel leaves tools disabled.");
    if (DeveloperAlive(dialog))
        (void)gtk_widget_grab_focus(dialog->trust);
    if (DeveloperAlive(dialog)) DeveloperReviewRevealSchedule(dialog);
    UmiStatus status = DeveloperAlive(dialog) ? UMI_STATUS_OK : UMI_STATUS_CANCELLED;
    dialog->settingsSelecting = 0;
    DeveloperRelease(dialog);
    return status;
}
