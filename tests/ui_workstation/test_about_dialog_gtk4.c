/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_about_dialog_gtk4.c
 *
 * PURPOSE:
 *   Exercise metadata copies, canonical artwork, weak About caching and synchronous window observers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/about_dialog.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%d: %s\n", __LINE__, #c); failed = 1; goto cleanup; } } while (0)

static GtkWindow *FindAbout(GtkWindow *parent)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (GTK_IS_ABOUT_DIALOG(window) &&
            gtk_window_get_transient_for(window) == parent) return window;
        g_object_unref(window);
    }
    return NULL;
}
typedef struct AboutObserver {
    GtkWindow *parent;
    GtkWindow *other;
    UmiGtk4AboutDialogConfig config;
    const char *mode;
    unsigned calls;
    UmiStatus nested;
} AboutObserver;
static void ObserveVisible(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    AboutObserver *observer = data;
    if (!gtk_widget_get_visible(GTK_WIDGET(object)) || observer->calls++ != 0U) return;
    if (!strcmp(observer->mode, "observer-owner-close")) gtk_window_destroy(observer->parent);
    else if (!strcmp(observer->mode, "observer-child-close")) gtk_window_destroy(GTK_WINDOW(object));
    else if (!strcmp(observer->mode, "observer-hide")) gtk_widget_set_visible(GTK_WIDGET(object), FALSE);
    else if (!strcmp(observer->mode, "observer-reparent"))
        gtk_window_set_transient_for(GTK_WINDOW(object), observer->other);
    else observer->nested = UmiGtk4AboutDialogPresent(observer->parent, &observer->config);
}

typedef struct AboutConstruction {
    GtkWindow *parent;
    UmiGtk4AboutDialogConfig *config;
    const char *mode;
    char *name;
    unsigned calls;
    UmiStatus nested;
} AboutConstruction;
static void ObserveConstruction(GListModel *model, guint position,
    guint removed, guint added, gpointer data)
{
    (void)removed;
    AboutConstruction *observer = data;
    if (added == 0U || observer->calls != 0U) return;
    for (guint i = 0U; i < added; ++i) {
        GObject *object = g_list_model_get_item(model, position + i);
        gboolean about = GTK_IS_ABOUT_DIALOG(object);
        g_clear_object(&object);
        if (!about) continue;
        ++observer->calls;
        if (!strcmp(observer->mode, "construction-owner-close")) gtk_window_destroy(observer->parent);
        else if (!strcmp(observer->mode, "construction-copy")) strcpy(observer->name, "Changed during creation");
        else observer->nested = UmiGtk4AboutDialogPresent(observer->parent, observer->config);
        break;
    }
}

static void ReleaseWindow(GtkWindow **window)
{
    if (*window != NULL) {
        if (UmiGtk4WindowIsOpen(*window)) gtk_window_destroy(*window);
        g_clear_object(window);
    }
}
int main(int argc, char **argv)
{
    const char *modes[] = {"metadata", "copy", "brand", "native-close", "owner-close",
        "null-parent", "invalid-parent", "closed-parent", "null-config", "null-output",
        "occupied-output", "empty-name", "invalid-utf8", "name-limit", "comment-limit",
        "invalid-license", "empty-comment", "repeat", "reopen-retained", "distinct-owners",
        "changed-config", "observer-owner-close", "observer-child-close", "observer-hide",
        "observer-reparent", "observer-reenter", "construction-owner-close",
        "construction-copy", "construction-reenter"};
    if (argc != 2) return 2;
    const char *mode = argv[1]; gboolean known = FALSE;
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (!strcmp(mode, modes[i])) known = TRUE;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0; GtkWindow *parent = GTK_WINDOW(gtk_window_new());
    GtkWindow *window = NULL, *other = NULL, *again = NULL;
    GtkWidget *box = NULL;
    gulong construction_signal = 0U;
    g_object_ref(parent);
    char name[257] = "Umicom Test", comments[4097] = "Native metadata";
    UmiGtk4AboutDialogConfig config = {
        {name, "Umicom Foundation", "umicom-test"}, "1.2.3", comments, GTK_LICENSE_MIT_X11
    };

    AboutConstruction construction = {parent, &config, mode, name, 0U, UMI_STATUS_OK};
    if (!strncmp(mode, "construction-", 13U)) {
        construction_signal = g_signal_connect(gtk_window_get_toplevels(), "items-changed",
            G_CALLBACK(ObserveConstruction), &construction);
        UmiStatus result = UmiGtk4AboutDialogPresent(parent, &config);
        g_signal_handler_disconnect(gtk_window_get_toplevels(), construction_signal);
        construction_signal = 0U;
        CHECK(construction.calls == 1U);
        if (!strcmp(mode, "construction-owner-close")) {
            CHECK(result == UMI_STATUS_CANCELLED);
            CHECK(FindAbout(parent) == NULL);
        } else {
            CHECK(result == UMI_STATUS_OK);
            window = FindAbout(parent); CHECK(window != NULL);
            CHECK(!strcmp(gtk_about_dialog_get_program_name(GTK_ABOUT_DIALOG(window)), "Umicom Test"));
            if (!strcmp(mode, "construction-reenter")) CHECK(construction.nested == UMI_STATUS_BUSY);
        }
    } else if (!strcmp(mode, "null-config")) {
        CHECK(UmiGtk4AboutDialogCreate(parent, NULL, &window) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(mode, "null-output")) {
        CHECK(UmiGtk4AboutDialogCreate(parent, &config, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(mode, "occupied-output")) {
        window = g_object_ref(parent);
        CHECK(UmiGtk4AboutDialogCreate(parent, &config, &window) == UMI_STATUS_INVALID_STATE);
        CHECK(window == parent);
        g_clear_object(&window);
    } else if (!strcmp(mode, "invalid-parent")) {
        box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); g_object_ref_sink(box);
        CHECK(UmiGtk4AboutDialogCreate((GtkWindow *)box, &config, &window) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(mode, "closed-parent")) {
        gtk_window_destroy(parent);
        CHECK(UmiGtk4AboutDialogCreate(parent, &config, &window) == UMI_STATUS_CANCELLED);
        CHECK(UmiGtk4AboutDialogPresent(parent, &config) == UMI_STATUS_CANCELLED);
    } else if (!strcmp(mode, "empty-name") || !strcmp(mode, "invalid-utf8") ||
        !strcmp(mode, "name-limit") || !strcmp(mode, "comment-limit") ||
        !strcmp(mode, "invalid-license")) {
        if (!strcmp(mode, "empty-name")) name[0] = '\0';
        if (!strcmp(mode, "invalid-utf8")) { name[0] = (char)0xff; name[1] = '\0'; }
        if (!strcmp(mode, "name-limit")) { memset(name, 'x', 256U); name[256] = '\0'; }
        if (!strcmp(mode, "comment-limit")) { memset(comments, 'x', 4096U); comments[4096] = '\0'; }
        if (!strcmp(mode, "invalid-license")) config.license = (GtkLicense)-1;
        CHECK(UmiGtk4AboutDialogCreate(parent, &config, &window) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4AboutDialogPresent(parent, &config) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(mode, "repeat") || !strcmp(mode, "reopen-retained") ||
        !strcmp(mode, "distinct-owners") || !strcmp(mode, "changed-config") ||
        !strncmp(mode, "observer-", 9U)) {
        CHECK(UmiGtk4AboutDialogPresent(parent, &config) == UMI_STATUS_OK);
        window = FindAbout(parent); CHECK(window != NULL);
        if (!strcmp(mode, "reopen-retained")) gtk_window_destroy(window);
        if (!strcmp(mode, "distinct-owners")) {
            other = GTK_WINDOW(gtk_window_new()); g_object_ref(other);
            CHECK(UmiGtk4AboutDialogPresent(other, &config) == UMI_STATUS_OK);
            again = FindAbout(other); CHECK(again != NULL && again != window);
        } else if (!strcmp(mode, "changed-config")) {
            config.version = "2.0";
            CHECK(UmiGtk4AboutDialogPresent(parent, &config) == UMI_STATUS_INVALID_STATE);
            CHECK(!strcmp(gtk_about_dialog_get_version(GTK_ABOUT_DIALOG(window)), "1.2.3"));
        } else if (!strncmp(mode, "observer-", 9U)) {
            gtk_widget_set_visible(GTK_WIDGET(window), FALSE);
            other = GTK_WINDOW(gtk_window_new()); g_object_ref(other);
            AboutObserver observer = {parent, other, config, mode, 0U, UMI_STATUS_OK};
            gulong signal = g_signal_connect(window, "notify::visible", G_CALLBACK(ObserveVisible), &observer);
            UmiStatus result = UmiGtk4AboutDialogPresent(parent, &config);
            g_signal_handler_disconnect(window, signal);
            CHECK(observer.calls != 0U);
            if (!strcmp(mode, "observer-reenter")) {
                CHECK(result == UMI_STATUS_OK && observer.nested == UMI_STATUS_BUSY);
            } else CHECK(result == UMI_STATUS_CANCELLED);
        } else {
            CHECK(UmiGtk4AboutDialogPresent(parent, &config) == UMI_STATUS_OK);
            again = FindAbout(parent); CHECK(again != NULL);
            CHECK(!strcmp(mode, "repeat") ? again == window : again != window);
        }
    } else {
        if (!strcmp(mode, "empty-comment")) comments[0] = '\0';
        CHECK(UmiGtk4AboutDialogCreate(!strcmp(mode, "null-parent") ? NULL : parent,
            &config, &window) == UMI_STATUS_OK);
        CHECK(GTK_IS_ABOUT_DIALOG(window) && !gtk_widget_get_visible(GTK_WIDGET(window)));
        CHECK(g_signal_lookup("response", GTK_TYPE_ABOUT_DIALOG) == 0U);
        if (!strcmp(mode, "copy")) { strcpy(name, "Changed"); strcpy(comments, "Changed"); }
        CHECK(!strcmp(gtk_about_dialog_get_program_name(GTK_ABOUT_DIALOG(window)), "Umicom Test"));
        CHECK(!strcmp(gtk_about_dialog_get_version(GTK_ABOUT_DIALOG(window)), "1.2.3"));
        CHECK(!g_strcmp0(gtk_about_dialog_get_comments(GTK_ABOUT_DIALOG(window)),
            !strcmp(mode, "empty-comment") ? "" : "Native metadata"));
        CHECK(gtk_about_dialog_get_license_type(GTK_ABOUT_DIALOG(window)) == GTK_LICENSE_MIT_X11);
        if (!strcmp(mode, "brand")) {
            GdkPaintable *logo = gtk_about_dialog_get_logo(GTK_ABOUT_DIALOG(window));
            CHECK(GDK_IS_TEXTURE(logo));
            CHECK(gdk_paintable_get_intrinsic_width(logo) > 0 &&
                gdk_paintable_get_intrinsic_width(logo) == gdk_paintable_get_intrinsic_height(logo));
        }
        if (!strcmp(mode, "null-parent")) {
            CHECK(gtk_window_get_transient_for(window) == NULL && !gtk_window_get_modal(window));
        } else {
            CHECK(gtk_window_get_transient_for(window) == parent && gtk_window_get_modal(window));
            CHECK(gtk_window_get_destroy_with_parent(window));
        }
        if (!strcmp(mode, "native-close")) {
            gtk_window_present(window); gtk_window_close(window);
            CHECK(!UmiGtk4WindowIsOpen(window));
        }
        if (!strcmp(mode, "owner-close")) {
            gtk_window_destroy(parent);
            CHECK(!UmiGtk4WindowIsOpen(window));
        }
    }
cleanup:
    if (construction_signal != 0U) g_signal_handler_disconnect(gtk_window_get_toplevels(), construction_signal);
    if (again == window) g_clear_object(&again);
    ReleaseWindow(&again); ReleaseWindow(&window);
    ReleaseWindow(&other); ReleaseWindow(&parent); g_clear_object(&box);
    return failed;
}
