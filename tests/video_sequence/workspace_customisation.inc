/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/workspace_customisation.inc
 *
 * PURPOSE:
 *   Exercise library navigation, panel customisation and branding without changing media recipes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static bool CustomisationCase(const char *mode)
{
    const char *cases[] = {
        "library-search", "library-selection", "library-empty", "library-reentry",
        "timeline-zoom", "layout-resize", "layout-visibility", "layout-recall",
        "layout-reset", "layout-retirement", "layout-reentry", "layout-retire-notify",
        "workspace-navigation", "media-brand",
        "media-mode-retention"
    };
    for (size_t i = 0U; i < G_N_ELEMENTS(cases); ++i)
        if (strcmp(mode, cases[i]) == 0) return true;
    return false;
}
static bool SurfaceCase(const char *mode)
{
    return strcmp(mode, "media-brand") == 0 || strcmp(mode, "media-mode-retention") == 0;
}
static bool CardVisible(UmiVideoSequencePanel *panel, size_t index)
{
    return gtk_widget_get_visible(gtk_widget_get_parent(panel->libraryButtons[index]));
}
static void EditDuringLayout(GObject *widget, GParamSpec *property, gpointer root)
{
    (void)widget; (void)property;
    Click(GTK_WIDGET(root), "video.remove");
}
static void RetireDuringLayout(GObject *widget, GParamSpec *property, gpointer root)
{
    (void)widget; (void)property;
    UmiVideoSequenceGtk4Retire(GTK_WIDGET(root));
}
static void Customisation(const char *mode, GtkWidget *window, GtkWidget *host,
                          GtkWidget *root, UmiVideoSequencePanel *panel)
{
    if (!CustomisationCase(mode)) return;
    UmiVideoSequence before = *UmiVideoSequenceEditorRead(panel->editor);
    bool dirty = panel->dirty;
    if (strcmp(mode, "library-search") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(panel->search), "SECOND");
        CHECK(!CardVisible(panel, 0U) && CardVisible(panel, 1U));
        gtk_editable_set_text(GTK_EDITABLE(panel->search), "INPUT.MP4");
        CHECK(CardVisible(panel, 0U) && CardVisible(panel, 1U));
        gtk_editable_set_text(GTK_EDITABLE(panel->search), "");
        CHECK(CardVisible(panel, 0U) && CardVisible(panel, 1U));
    } else if (strcmp(mode, "library-selection") == 0) {
        Click(root, "video.library.clip.0");
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector)) == 0U);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(panel->id)), "first") == 0);
        CHECK(gtk_widget_has_css_class(panel->clipButtons[0], "suggested-action"));
        CHECK(gtk_widget_has_css_class(panel->libraryButtons[0], "suggested-action"));
        CHECK(!gtk_widget_has_css_class(panel->libraryButtons[1], "suggested-action"));
    } else if (strcmp(mode, "library-empty") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(panel->search), "not-a-clip");
        CHECK(!CardVisible(panel, 0U) && !CardVisible(panel, 1U));
        CHECK(gtk_widget_get_visible(panel->emptyLibrary));
        CHECK(gtk_widget_get_visible(panel->clipButtons[0]));
        gtk_editable_set_text(GTK_EDITABLE(panel->search), "");
        CHECK(!gtk_widget_get_visible(panel->emptyLibrary));
    } else if (strcmp(mode, "library-reentry") == 0) {
        gulong handler = g_signal_connect(panel->clipButtons[0], "notify::width-request",
            G_CALLBACK(EditDuringLayout), root);
        gtk_range_set_value(GTK_RANGE(panel->zoom), 40.0);
        g_signal_handler_disconnect(panel->clipButtons[0], handler);
    } else if (strcmp(mode, "timeline-zoom") == 0) {
        int oldWidth = 0, newWidth = 0;
        gtk_widget_get_size_request(panel->clipButtons[0], &oldWidth, NULL);
        gtk_range_set_value(GTK_RANGE(panel->zoom), 40.0);
        gtk_widget_get_size_request(panel->clipButtons[0], &newWidth, NULL);
        CHECK(newWidth > oldWidth);
        CHECK(strstr(gtk_button_get_label(GTK_BUTTON(panel->clipButtons[0])), "0.00 - 5.00 s") != NULL);
    } else if (strcmp(mode, "layout-resize") == 0) {
        int old = gtk_paned_get_position(GTK_PANED(panel->layoutPanes[0]));
        gtk_paned_set_position(GTK_PANED(panel->layoutPanes[0]), old + 40);
        Settle();
        CHECK(gtk_paned_get_position(GTK_PANED(panel->layoutPanes[0])) > old);
        for (size_t i = 0U; i < 3U; ++i) {
            CHECK(gtk_paned_get_wide_handle(GTK_PANED(panel->layoutPanes[i])));
            CHECK(gtk_widget_get_focusable(panel->layoutPanes[i]));
        }
        Geometry(root); Capture(window, host);
        gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768); Settle();
        Geometry(root); Capture(window, host);
    } else if (strcmp(mode, "layout-visibility") == 0) {
        for (size_t i = 0U; i < 3U; ++i) {
            gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->layoutChecks[i]), FALSE);
            CHECK(!gtk_widget_get_visible(panel->layoutRegions[i]));
            gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->layoutChecks[i]), TRUE);
            CHECK(gtk_widget_get_visible(panel->layoutRegions[i]));
        }
        Click(root, "video.layout.focus"); Settle();
        for (size_t i = 0U; i < 3U; ++i) CHECK(!gtk_widget_get_visible(panel->layoutRegions[i]));
        CHECK(gtk_widget_get_mapped(panel->video));
    } else if (strcmp(mode, "layout-recall") == 0) {
        gtk_paned_set_position(GTK_PANED(panel->layoutPanes[0]), 300); Settle();
        int position = gtk_paned_get_position(GTK_PANED(panel->layoutPanes[0]));
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->layoutChecks[1]), FALSE);
        Click(root, "video.layout.remember");
        CHECK(panel->rememberedLayout);
        Click(root, "video.layout.reset"); Settle();
        Click(root, "video.layout.recall"); Settle();
        CHECK(gtk_widget_get_visible(panel->layoutRegions[0]));
        CHECK(!gtk_widget_get_visible(panel->layoutRegions[1]));
        CHECK(gtk_widget_get_visible(panel->layoutRegions[2]));
        CHECK(abs(gtk_paned_get_position(GTK_PANED(panel->layoutPanes[0])) - position) <= 2);
    } else if (strcmp(mode, "layout-reset") == 0) {
        Click(root, "video.layout.recall");
        CHECK(!panel->rememberedLayout);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(panel->status)), "Remember layout first") != NULL);
        Click(root, "video.layout.focus");
        Click(root, "video.layout.reset"); Settle();
        for (size_t i = 0U; i < 3U; ++i) CHECK(gtk_widget_get_visible(panel->layoutRegions[i]));
        Geometry(root);
    } else if (strcmp(mode, "layout-reentry") == 0) {
        gulong handler = g_signal_connect(panel->sourceBook, "notify::visible",
            G_CALLBACK(EditDuringLayout), root);
        Click(root, "video.layout.focus");
        Click(root, "video.layout.reset");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->layoutChecks[0]), FALSE);
        Click(root, "video.navigation.project");
        g_signal_handler_disconnect(panel->sourceBook, handler);
        CHECK(!panel->arranging && !panel->refreshing);
    } else if (strcmp(mode, "layout-retire-notify") == 0) {
        gulong handler = g_signal_connect(panel->sourceBook, "notify::visible",
            G_CALLBACK(RetireDuringLayout), root);
        Click(root, "video.layout.focus");
        g_signal_handler_disconnect(panel->sourceBook, handler);
        CHECK(panel->closed && !panel->arranging && !panel->refreshing);
        CHECK(gtk_widget_get_visible(panel->inspectorBook));
        CHECK(gtk_widget_get_visible(panel->layoutRegions[2]));
    } else if (strcmp(mode, "layout-retirement") == 0) {
        UmiVideoSequenceGtk4Retire(root);
        Click(root, "video.layout.focus");
        Click(root, "video.layout.remember");
        gtk_editable_set_text(GTK_EDITABLE(panel->search), "second");
        gtk_range_set_value(GTK_RANGE(panel->zoom), 40.0);
        Click(root, "video.library.clip.0");
        CHECK(!panel->rememberedLayout);
        for (size_t i = 0U; i < 3U; ++i) CHECK(gtk_widget_get_visible(panel->layoutRegions[i]));
    } else if (strcmp(mode, "workspace-navigation") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(panel->rightId), "unsubmitted-cut");
        Click(root, "video.layout.focus");
        Click(root, "video.navigation.project");
        CHECK(gtk_widget_get_visible(panel->sourceBook));
        CHECK(gtk_notebook_get_current_page(GTK_NOTEBOOK(panel->sourceBook)) == 1);
        Click(root, "video.navigation.export");
        CHECK(gtk_widget_get_visible(panel->inspectorBook));
        CHECK(gtk_notebook_get_current_page(GTK_NOTEBOOK(panel->inspectorBook)) == 1);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(panel->rightId)), "unsubmitted-cut") == 0);
    } else if (strcmp(mode, "media-brand") == 0) {
        GtkWidget *logo = Find(host, "media.brand.logo");
        GtkWidget *icon = Find(host, "media.brand.icon");
        CHECK(GTK_IS_PICTURE(logo) && GTK_IS_PICTURE(icon));
        CHECK(gtk_picture_get_paintable(GTK_PICTURE(logo)) != NULL);
        CHECK(gtk_picture_get_paintable(GTK_PICTURE(icon)) != NULL);
        CHECK(gtk_widget_get_mapped(logo) && gtk_widget_get_mapped(icon));
        Capture(window, host);
    } else if (strcmp(mode, "media-mode-retention") == 0) {
        GtkWidget *originalClip = panel->clipButtons[0];
        gtk_editable_set_text(GTK_EDITABLE(panel->rightId), "unsubmitted-cut");
        const char *routes[] = {"ai-video", "ai-image", "audio", "canvas"};
        const char *tags[] = {"creative.tools.ai-video", "creative.tools.ai-image",
                              "creative.tools.audio", "creative.tools.canvas"};
        GtkStack *pages = GTK_STACK(Find(host, "creative.surface.pages"));
        for (size_t i = 0U; i < G_N_ELEMENTS(routes); ++i) {
            char id[64]; (void)g_snprintf(id, sizeof id, "media.mode.%s", routes[i]);
            Click(host, id); Settle();
            CHECK(strcmp(gtk_stack_get_visible_child_name(pages), "creative-workspace") == 0);
            CHECK(gtk_widget_get_mapped(Find(host, tags[i])));
            CHECK(gtk_widget_has_css_class(Find(host, id), "suggested-action"));
            Click(host, "media.mode.video"); Settle();
            CHECK(gtk_stack_get_visible_child(pages) == root);
            CHECK(gtk_widget_has_css_class(Find(host, "media.mode.video"), "suggested-action"));
            CHECK(!gtk_widget_has_css_class(Find(host, id), "suggested-action"));
            CHECK(panel->clipButtons[0] == originalClip);
            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(panel->rightId)), "unsubmitted-cut") == 0);
        }
        Capture(window, host);
    }
    CHECK(UmiVideoSequenceEqual(&before, UmiVideoSequenceEditorRead(panel->editor)));
    CHECK(panel->dirty == dirty);
    if (!panel->closed) {
        /* Viewing, resizing and zooming must not consume a recipe undo slot. */
        Click(root, "video.undo");
        CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
    }
}
