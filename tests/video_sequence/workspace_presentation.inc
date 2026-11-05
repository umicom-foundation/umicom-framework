/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/workspace_presentation.inc
 *
 * PURPOSE:
 *   Exercise compact editing controls, retained drafts and proportional panel layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static bool PresentationCase(const char *mode)
{
    const char *cases[] = {"presentation-sections", "presentation-split",
        "presentation-split-edit", "presentation-reentry", "presentation-retirement",
        "presentation-wide", "presentation-history"};
    for (size_t i = 0U; i < G_N_ELEMENTS(cases); ++i)
        if (strcmp(mode, cases[i]) == 0) return true;
    return false;
}

/* GTK activation follows the button's own action, rather than calling an
 * editing service directly. Waiting allows GTK's activation animation to end. */
static void ActivatePresentation(GtkWidget *root, const char *id)
{
    GtkWidget *button = Find(root, id);
    CHECK(gtk_widget_get_mapped(button));
    CHECK(gtk_widget_is_sensitive(button));
    CHECK(gtk_widget_activate(button));
    Settle();
}

static void Presentation(const char *mode, GtkWidget *window, GtkWidget *host,
                         GtkWidget *root, UmiVideoSequencePanel *panel)
{
    if (!PresentationCase(mode)) return;
    UmiVideoSequence before = *UmiVideoSequenceEditorRead(panel->editor);
    bool dirty = panel->dirty;
    GtkExpander *split = GTK_EXPANDER(Find(root, "video.section.split"));
    CHECK(!gtk_expander_get_expanded(split));
    if (strcmp(mode, "presentation-sections") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(panel->rightId), "draft-right");
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->split), 1250.0);
        const char *ids[] = {"video.section.import", "video.section.range",
                            "video.section.split", "video.section.order"};
        for (size_t i = 0U; i < G_N_ELEMENTS(ids); ++i) {
            GtkExpander *section = GTK_EXPANDER(Find(root, ids[i]));
            GtkWidget *child = gtk_expander_get_child(section);
            CHECK(child != NULL);
            gtk_expander_set_expanded(section, TRUE);
            gtk_expander_set_expanded(section, FALSE);
            gtk_expander_set_expanded(section, TRUE);
            CHECK(gtk_expander_get_child(section) == child);
        }
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(panel->rightId)), "draft-right") == 0);
        CHECK(gtk_spin_button_get_value(GTK_SPIN_BUTTON(panel->split)) == 1250.0);
    } else if (strcmp(mode, "presentation-split") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(panel->rightId), "draft-right");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->layoutChecks[1]), FALSE);
        Settle();
        ActivatePresentation(root, "video.navigation.split");
        CHECK(gtk_expander_get_expanded(split));
        CHECK(gtk_widget_get_mapped(panel->split));
        CHECK(FocusInRegion(window, GTK_WIDGET(split)));
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(panel->rightId)), "draft-right") == 0);
    } else if (strcmp(mode, "presentation-split-edit") == 0) {
        ActivatePresentation(root, "video.navigation.split");
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->split), 1000.0);
        gtk_editable_set_text(GTK_EDITABLE(panel->rightId), "second-right");
        ActivatePresentation(root, "video.split");
        CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 3U);
        ActivatePresentation(root, "video.undo");
        CHECK(UmiVideoSequenceEqual(&before, UmiVideoSequenceEditorRead(panel->editor)));
        ActivatePresentation(root, "video.redo");
        CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 3U);
        ActivatePresentation(root, "video.undo");
    } else if (strcmp(mode, "presentation-reentry") == 0 ||
               strcmp(mode, "presentation-retirement") == 0) {
        bool retire = strcmp(mode, "presentation-retirement") == 0;
        gulong handler = g_signal_connect(split, "notify::expanded",
            retire ? G_CALLBACK(RetireDuringLayout) : G_CALLBACK(EditDuringLayout), root);
        ActivatePresentation(root, "video.navigation.split");
        g_signal_handler_disconnect(split, handler);
        CHECK(panel->closed == retire);
        CHECK(!panel->refreshing && !panel->arranging);
        if (retire) {
            gtk_expander_set_expanded(split, FALSE);
            Click(root, "video.navigation.split");
            CHECK(!gtk_expander_get_expanded(split));
        }
    } else if (strcmp(mode, "presentation-wide") == 0) {
        gtk_window_set_default_size(GTK_WINDOW(window), 1600, 1000); Settle();
        Click(root, "video.layout.reset"); Settle();
        Geometry(root);
        graphene_rect_t player = Bounds(root, "video.preview-region");
        graphene_rect_t library = Bounds(root, "video.source-pages");
        graphene_rect_t inspector = Bounds(root, "video.inspector-pages");
        CHECK(player.size.width > library.size.width && player.size.width > inspector.size.width);
        gtk_paned_set_position(GTK_PANED(panel->layoutPanes[0]), 290); Settle();
        int position = gtk_paned_get_position(GTK_PANED(panel->layoutPanes[0]));
        gtk_widget_set_visible(root, FALSE); Settle();
        gtk_widget_set_visible(root, TRUE); Settle();
        CHECK(abs(gtk_paned_get_position(GTK_PANED(panel->layoutPanes[0])) - position) <= 2);
        Capture(window, host);
    } else if (strcmp(mode, "presentation-history") == 0) {
        GtkWidget *undo = Find(root, "video.undo");
        CHECK(gtk_widget_is_ancestor(undo, Find(root, "video.timeline")));
        CHECK(GTK_IS_IMAGE(gtk_button_get_child(GTK_BUTTON(undo))));
        CHECK(strcmp(gtk_widget_get_tooltip_text(undo), "Undo video edit") == 0);
        ActivatePresentation(root, "video.undo");
        CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
        ActivatePresentation(root, "video.redo");
    }
    CHECK(UmiVideoSequenceEqual(&before, UmiVideoSequenceEditorRead(panel->editor)));
    CHECK(panel->dirty == dirty);
}
