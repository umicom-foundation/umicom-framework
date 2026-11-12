/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/workspace_minimum.inc
 *
 * PURPOSE:
 *   Exercise native pane measurement at the minimum size before and after timeline changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static bool MinimumCase(const char *mode)
{
    return strcmp(mode, "minimum-empty") == 0 ||
           strcmp(mode, "minimum-populated") == 0 ||
           strcmp(mode, "minimum-expanded") == 0 ||
           strcmp(mode, "minimum-visibility") == 0;
}

/* Ask GTK for its real requests, then measure the opposite axis at the exact
 * boundary and adjacent sizes. A large initial window can hide a bad minimum
 * contract. Fatal GTK warnings remain enabled throughout these measurements. */
static void MinimumRequests(GtkWidget *root)
{
    GtkWidget *pane = Find(root, "video.workspace.vertical");
    GtkWidget *start = gtk_paned_get_start_child(GTK_PANED(pane));
    GtkWidget *end = gtk_paned_get_end_child(GTK_PANED(pane));
    int startMin = 0, endMin = 0, minimum = 0, natural = 0;
    CHECK(start != NULL && end != NULL);
    if (gtk_widget_get_visible(start))
        gtk_widget_measure(start, GTK_ORIENTATION_VERTICAL, -1, &startMin, NULL, NULL, NULL);
    if (gtk_widget_get_visible(end))
        gtk_widget_measure(end, GTK_ORIENTATION_VERTICAL, -1, &endMin, NULL, NULL, NULL);
    gtk_widget_measure(pane, GTK_ORIENTATION_VERTICAL, -1, &minimum, &natural, NULL, NULL);
    CHECK(minimum > 0 && natural >= minimum);
    CHECK(minimum >= startMin + endMin);
    if (gtk_widget_get_visible(start) && gtk_widget_get_visible(end)) {
        CHECK(startMin > 0 && endMin > 0);
        /* The separator also needs height; neither visible region may donate
         * its last pixel to satisfy the divider's initial position. */
        CHECK(minimum > startMin + endMin);
    }
    for (int extra = 0; extra <= 4; ++extra) {
        int widthMin = 0, widthNatural = 0;
        gtk_widget_measure(pane, GTK_ORIENTATION_HORIZONTAL, minimum + extra,
                           &widthMin, &widthNatural, NULL, NULL);
        CHECK(widthMin > 0 && widthNatural >= widthMin);
    }
}

static void MinimumAfterEdits(const char *mode, GtkWidget *root, UmiVideoSequencePanel *panel)
{
    if (!MinimumCase(mode) || strcmp(mode, "minimum-empty") == 0) return;
    UmiVideoSequence before = *UmiVideoSequenceEditorRead(panel->editor);
    bool dirty = panel->dirty;
    MinimumRequests(root);
    if (strcmp(mode, "minimum-expanded") == 0) {
        /* A collapsed body is retained by its expander but is not parented.
         * Locate the named owner rather than requiring a hidden child ancestry.
         * Preserve the earlier assumption so the regression remains reviewable. */
#if 0
        GtkWidget *expander = gtk_widget_get_ancestor(panel->summary, GTK_TYPE_EXPANDER);
#endif
        GtkWidget *expander = Find(root, "video.section.summary");
        CHECK(expander != NULL);
        CHECK(!gtk_expander_get_expanded(GTK_EXPANDER(expander)));
        GtkWidget *body = gtk_expander_get_child(GTK_EXPANDER(expander));
        CHECK(body != NULL);
        CHECK(gtk_widget_is_ancestor(panel->summary, body));
        gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE);
        MinimumRequests(root); Settle();
        CHECK(gtk_widget_get_mapped(panel->summary));
        gtk_expander_set_expanded(GTK_EXPANDER(expander), FALSE);
        MinimumRequests(root);
        CHECK(gtk_expander_get_child(GTK_EXPANDER(expander)) == body);
        CHECK(!gtk_widget_get_mapped(panel->summary));
    } else if (strcmp(mode, "minimum-visibility") == 0) {
        GtkWidget *pane = Find(root, "video.workspace.vertical");
        GtkWidget *start = gtk_paned_get_start_child(GTK_PANED(pane));
        GtkWidget *end = gtk_paned_get_end_child(GTK_PANED(pane));
        gtk_widget_set_visible(start, FALSE); MinimumRequests(root);
        gtk_widget_set_visible(start, TRUE); MinimumRequests(root);
        gtk_widget_set_visible(end, FALSE); MinimumRequests(root);
        gtk_widget_set_visible(end, TRUE); MinimumRequests(root);
        Settle(); Geometry(root);
    }
    CHECK(UmiVideoSequenceEqual(&before, UmiVideoSequenceEditorRead(panel->editor)));
    CHECK(panel->dirty == dirty);
}
