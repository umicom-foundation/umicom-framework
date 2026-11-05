/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/workspace_menu_commands.inc
 *
 * PURPOSE:
 *   Exercise Media menu proxies against the shared editor and its lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static bool MediaMenuCase(const char *mode)
{
    return strcmp(mode, "media-menu-history") == 0 ||
        strcmp(mode, "media-menu-availability") == 0 ||
        strcmp(mode, "media-menu-navigation") == 0 ||
        strcmp(mode, "media-menu-retirement") == 0;
}
static void MediaMenuCommands(const char *mode, GtkWidget *host, GtkWidget *root,
                              UmiVideoSequencePanel *panel)
{
    if (!MediaMenuCase(mode)) return;
    GtkWidget *undo = Find(host, "media.menu.undo");
    GtkWidget *redo = Find(host, "media.menu.redo");
    CHECK(gtk_widget_is_sensitive(undo));
    CHECK(!gtk_widget_is_sensitive(redo));
    if (strcmp(mode, "media-menu-history") == 0) {
        Click(host, "media.menu.undo");
        CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
        CHECK(gtk_widget_is_sensitive(redo));
        Click(host, "media.menu.redo");
        CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 2U);
        CHECK(!gtk_widget_is_sensitive(redo));
        /* Exhaust history, then branch from an undone edit. Menu state must
         * follow the original controls without silently changing the recipe. */
        Click(host, "media.menu.undo");
        Click(host, "media.menu.undo");
        CHECK(!gtk_widget_is_sensitive(undo));
        CHECK(gtk_widget_is_sensitive(redo));
        Click(host, "media.menu.undo");
        CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 0U);
        Add(root, "branch", 2000.0);
        CHECK(gtk_widget_is_sensitive(undo));
        CHECK(!gtk_widget_is_sensitive(redo));
        Click(host, "media.menu.redo");
        CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
    } else if (strcmp(mode, "media-menu-availability") == 0) {
        GtkWidget *command = Find(root, "video.undo");
        UmiVideoSequence before = *UmiVideoSequenceEditorRead(panel->editor);
        gtk_widget_set_sensitive(command, FALSE);
        CHECK(!gtk_widget_is_sensitive(undo));
        Click(host, "media.menu.undo"); /* Synthetic activation also stays inert. */
        CHECK(UmiVideoSequenceEqual(&before, UmiVideoSequenceEditorRead(panel->editor)));
        gtk_widget_set_sensitive(command, TRUE);
        CHECK(gtk_widget_is_sensitive(undo));
        gtk_widget_set_sensitive(root, FALSE);
        CHECK(!gtk_widget_is_sensitive(undo));
        gtk_widget_set_sensitive(root, TRUE);
        CHECK(gtk_widget_is_sensitive(undo));
        const char *proxies[] = {"media.menu.project", "media.menu.save", "media.menu.export",
            "media.menu.undo", "media.menu.redo", "media.export"};
        const char *targets[] = {"video.navigation.project", "video.save", "video.navigation.export",
            "video.undo", "video.redo", "video.navigation.export"};
        for (size_t i = 0U; i < G_N_ELEMENTS(proxies); ++i) {
            GtkWidget *target = Find(root, targets[i]);
            gboolean available = gtk_widget_get_sensitive(target);
            gtk_widget_set_sensitive(target, FALSE);
            CHECK(!gtk_widget_is_sensitive(Find(host, proxies[i])));
            Click(host, proxies[i]);
            CHECK(!panel->busy);
            CHECK(UmiVideoSequenceEqual(&before, UmiVideoSequenceEditorRead(panel->editor)));
            gtk_widget_set_sensitive(target, available);
        }
    } else if (strcmp(mode, "media-menu-navigation") == 0) {
        GtkStack *pages = GTK_STACK(Find(host, "creative.surface.pages"));
        UmiVideoSequence before = *UmiVideoSequenceEditorRead(panel->editor);
        Click(host, "media.menu.library");
        CHECK(strcmp(gtk_stack_get_visible_child_name(pages), "creative-workspace") == 0);
        GtkWidget *library = Find(host, "creative.tools.library");
        GtkWidget *notebook = gtk_widget_get_ancestor(library, GTK_TYPE_NOTEBOOK);
        CHECK(GTK_IS_NOTEBOOK(notebook));
        GtkWidget *selected = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook),
            gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)));
        CHECK(selected == library || gtk_widget_is_ancestor(library, selected));
        Click(host, "media.menu.layouts");
        CHECK(strcmp(gtk_stack_get_visible_child_name(pages), "layouts") == 0);
        Click(host, "media.menu.video");
        CHECK(strcmp(gtk_stack_get_visible_child_name(pages), "video-editing") == 0);
        Click(host, "media.menu.project");
        CHECK(strcmp(gtk_stack_get_visible_child_name(pages), "video-editing") == 0);
        Click(host, "media.menu.export");
        CHECK(strcmp(gtk_stack_get_visible_child_name(pages), "video-editing") == 0);
        CHECK(UmiVideoSequenceEqual(&before, UmiVideoSequenceEditorRead(panel->editor)));
    } else {
        UmiVideoSequence before = *UmiVideoSequenceEditorRead(panel->editor);
        UmiVideoSequenceGtk4Retire(root);
        CHECK(panel->closed);
        CHECK(!gtk_widget_is_sensitive(undo));
        Click(host, "media.menu.undo");
        Click(host, "media.menu.redo");
        CHECK(UmiVideoSequenceEqual(&before, UmiVideoSequenceEditorRead(panel->editor)));
    }
}
