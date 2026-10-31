/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_settings_sections_gtk4.c
 *
 * PURPOSE:
 *   Verify settings navigation retains drafts, binding, layout and native ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/developer_dialog.h"
#include "umicom/ui/gtk4/build_configurations.h"
#include "umicom/ui/gtk4/automation.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto cleanup; } } while (0)
typedef struct SettingsProbe {
    UmiGtk4DeveloperDialog *dialog;
    GtkWindow *parent;
    const char *mode;
    unsigned accepted, observed, storage;
    int trusted;
    UmiStatus nested;
    UmiBuildProfile applied;
} SettingsProbe;
static const char *const Names[] = {
    "build", "tools", "presets", "run", "targets", "sdk", "intelligence", "configurations"
};
static GtkWidget *Find(GtkWindow *window, const char *tag)
{
    return umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(window), tag);
}
static GtkWindow *FindForm(GtkWindow *parent)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (gtk_window_get_transient_for(window) == parent) return window;
        g_object_unref(window);
    }
    return NULL;
}
static void Dispose(SettingsProbe *probe)
{
    UmiGtk4DeveloperDialog *dialog = probe->dialog;
    probe->dialog = NULL;
    UmiGtk4DeveloperDialogDestroy(dialog);
}
static UmiStatus Applied(const UmiBuildProfile *profile, int trusted, void *context)
{
    SettingsProbe *probe = context;
    ++probe->accepted;
    probe->applied = *profile;
    probe->trusted = trusted;
    return UMI_STATUS_OK;
}
static UmiStatus Created(const UmiDeveloperProjectModel *model,
    const UmiBuildProfile *profile, void *context)
{
    (void)model; return Applied(profile, 0, context);
}
static UmiStatus Capture(const char *root, UmiBuildConfigurationCatalogue *out, void *context)
{
    (void)root; (void)out; ++((SettingsProbe *)context)->storage;
    return UMI_STATUS_INVALID_STATE;
}
static UmiStatus Load(const char *root, const char *name, uint64_t revision,
    UmiBuildProfile *out, void *context)
{
    (void)root; (void)name; (void)revision; (void)out;
    ++((SettingsProbe *)context)->storage; return UMI_STATUS_INVALID_STATE;
}
static UmiStatus Save(const char *name, const UmiBuildProfile *profile, uint64_t revision,
    uint64_t *out, void *context)
{
    (void)name; (void)profile; (void)revision; (void)out;
    ++((SettingsProbe *)context)->storage; return UMI_STATUS_INVALID_STATE;
}
/* Selection is an observable boundary. Exercise a host retiring the handle or
 * requesting another section synchronously, while GTK still owns the call. */
static void SelectionChanged(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object; (void)property;
    SettingsProbe *probe = context;
    if (probe->observed++ != 0U) return;
    if (strcmp(probe->mode, "owner-close") == 0) Dispose(probe);
    else if (strcmp(probe->mode, "parent-close") == 0) gtk_window_destroy(probe->parent);
    else probe->nested = UmiGtk4BuildSettingsSelectSection(probe->dialog, UMI_GTK4_BUILD_SETTINGS_SDK);
}
static void ConstructionChanged(GObject *object, GParamSpec *property, gpointer context)
{
    (void)property;
    SettingsProbe *probe = context;
    int width = 0, height = 0;
    gtk_window_get_default_size(GTK_WINDOW(object), &width, &height);
    if (width == 920 && probe->observed++ == 0U) gtk_window_destroy(probe->parent);
}
static void WindowAdded(GListModel *model, guint at, guint removed, guint added, gpointer context)
{
    (void)removed;
    for (guint i = 0U; i < added; ++i) {
        GtkWindow *window = g_list_model_get_item(model, at + i);
        if (window != NULL) {
            g_signal_connect(window, "notify::default-width", G_CALLBACK(ConstructionChanged), context);
            g_object_unref(window);
        }
    }
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = { "navigation-cycle", "field-map", "prepared", "same-section", "invalid-section", "invalid-output", "null-dialog", "project-form", "owner-close", "parent-close", "reentrant-selection", "retained-controls", "apply-drafts", "hidden-apply", "cancel-drafts", "independent-forms", "configuration-binding", "sidebar", "divider", "independent-scroll", "pinned-actions", "construction-close" };
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(argv[1], cases[i]) == 0) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0;
    SettingsProbe probe = {0}, second = {0};
    GtkWindow *form = NULL;
    GtkWidget *stack = NULL, *retained = NULL;
    gulong changed = 0U, added = 0U;
    GListModel *windows = gtk_window_get_toplevels();
    probe.mode = argv[1];
    probe.parent = GTK_WINDOW(gtk_window_new()); g_object_ref(probe.parent);
    UmiBuildProfile profile;
    umi_build_profile_init(&profile);
    if (strcmp(probe.mode, "construction-close") == 0)
        added = g_signal_connect(windows, "items-changed", G_CALLBACK(WindowAdded), &probe);
    UmiStatus status = strcmp(probe.mode, "project-form") == 0
        ? UmiGtk4NewProjectDialogPrepare(probe.parent, Created, &probe, &probe.dialog)
        : UmiGtk4BuildSettingsDialogPrepare(probe.parent, &profile, 0, Applied, &probe, &probe.dialog);
    if (added != 0U) { g_signal_handler_disconnect(windows, added); added = 0U; }
    if (strcmp(probe.mode, "construction-close") == 0) {
        CHECK(status == UMI_STATUS_CANCELLED && probe.dialog == NULL && probe.observed == 1U);
        goto cleanup;
    }
    CHECK(status == UMI_STATUS_OK && probe.dialog != NULL);
    form = FindForm(probe.parent);
    CHECK(form != NULL && !gtk_widget_get_visible(GTK_WIDGET(form)));
    UmiGtk4BuildSettingsSection section = UMI_GTK4_BUILD_SETTINGS_RUN;
    if (strcmp(probe.mode, "project-form") == 0) {
        CHECK(UmiGtk4BuildSettingsSelectSection(probe.dialog, UMI_GTK4_BUILD_SETTINGS_RUN) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiGtk4BuildSettingsCurrentSection(probe.dialog, &section) == UMI_STATUS_INVALID_STATE);
        CHECK(section == UMI_GTK4_BUILD_SETTINGS_RUN); goto cleanup;
    }
    GtkWidget *foundStack = Find(form, "developer.settings.sections");
    CHECK(GTK_IS_STACK(foundStack)); stack = g_object_ref(foundStack);
    CHECK(UmiGtk4BuildSettingsCurrentSection(probe.dialog, &section) == UMI_STATUS_OK);
    CHECK(section == UMI_GTK4_BUILD_SETTINGS_BUILD);
    GtkWidget *accept = Find(form, "developer.dialog.accept");
    GtkWidget *trust = Find(form, "developer.build.trusted");
    CHECK(GTK_IS_BUTTON(accept) && GTK_IS_CHECK_BUTTON(trust));
    if (strcmp(probe.mode, "invalid-section") == 0) {
        CHECK(UmiGtk4BuildSettingsSelectSection(probe.dialog, (UmiGtk4BuildSettingsSection)-1) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4BuildSettingsSelectSection(probe.dialog, UMI_GTK4_BUILD_SETTINGS_SECTION_COUNT) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4BuildSettingsCurrentSection(probe.dialog, &section) == UMI_STATUS_OK && section == UMI_GTK4_BUILD_SETTINGS_BUILD);
    } else if (strcmp(probe.mode, "invalid-output") == 0) {
        CHECK(UmiGtk4BuildSettingsCurrentSection(probe.dialog, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(probe.mode, "null-dialog") == 0) {
        CHECK(UmiGtk4BuildSettingsCurrentSection(NULL, &section) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(section == UMI_GTK4_BUILD_SETTINGS_BUILD);
        CHECK(UmiGtk4BuildSettingsSelectSection(NULL, UMI_GTK4_BUILD_SETTINGS_RUN) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(probe.mode, "field-map") == 0) {
        const unsigned fields[] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U};
        const char *pages[] = {"build","build","build","tools","tools","build","presets","build","run","run","sdk","tools","build","run"};
        for (size_t i = 0U; i < sizeof fields / sizeof fields[0]; ++i) {
            char fieldTag[80], pageTag[80];
            g_snprintf(fieldTag, sizeof fieldTag, "developer.build.field.%u", fields[i]);
            g_snprintf(pageTag, sizeof pageTag, "developer.settings.section.%s", pages[i]);
            GtkWidget *field = Find(form, fieldTag), *page = Find(form, pageTag);
            CHECK(GTK_IS_ENTRY(field) && GTK_IS_BOX(page) && gtk_widget_is_ancestor(field, page));
        }
        CHECK(!gtk_editable_get_editable(GTK_EDITABLE(Find(form, "developer.build.field.1"))));
        CHECK(gtk_widget_is_ancestor(trust, Find(form, "developer.settings.section.build")));
    } else if (strcmp(probe.mode, "sidebar") == 0) {
        GtkWidget *sidebar = Find(form, "developer.settings.navigation");
        CHECK(GTK_IS_STACK_SIDEBAR(sidebar));
        CHECK(gtk_stack_sidebar_get_stack(GTK_STACK_SIDEBAR(sidebar)) == GTK_STACK(stack));
        gtk_stack_set_visible_child_name(GTK_STACK(stack), "run");
        CHECK(UmiGtk4BuildSettingsCurrentSection(probe.dialog, &section) == UMI_STATUS_OK && section == UMI_GTK4_BUILD_SETTINGS_RUN);
    } else if (strcmp(probe.mode, "divider") == 0) {
        GtkWidget *split = Find(form, "developer.settings.split");
        CHECK(GTK_IS_PANED(split) && gtk_window_get_resizable(form));
        gtk_paned_set_position(GTK_PANED(split), 240);
        CHECK(gtk_paned_get_position(GTK_PANED(split)) == 240);
        CHECK(UmiGtk4BuildSettingsSelectSection(probe.dialog, UMI_GTK4_BUILD_SETTINGS_TOOLS) == UMI_STATUS_OK);
        CHECK(gtk_paned_get_position(GTK_PANED(split)) == 240);
    } else if (strcmp(probe.mode, "independent-scroll") == 0) {
        GtkWidget *build = gtk_stack_get_child_by_name(GTK_STACK(stack), "build");
        GtkWidget *run = gtk_stack_get_child_by_name(GTK_STACK(stack), "run");
        CHECK(GTK_IS_SCROLLED_WINDOW(build) && GTK_IS_SCROLLED_WINDOW(run));
        GtkAdjustment *first = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(build));
        GtkAdjustment *next = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(run));
        CHECK(first != next);
        gtk_adjustment_configure(first, 120.0, 0.0, 800.0, 10.0, 50.0, 200.0);
        gtk_adjustment_configure(next, 30.0, 0.0, 800.0, 10.0, 50.0, 200.0);
        CHECK(UmiGtk4BuildSettingsSelectSection(probe.dialog, UMI_GTK4_BUILD_SETTINGS_RUN) == UMI_STATUS_OK);
        CHECK(gtk_adjustment_get_value(first) == 120.0 && gtk_adjustment_get_value(next) == 30.0);
    } else if (strcmp(probe.mode, "pinned-actions") == 0) {
        CHECK(!gtk_widget_is_ancestor(accept, stack));
        CHECK(!gtk_widget_is_ancestor(Find(form, "developer.dialog.cancel"), stack));
        CHECK(gtk_widget_get_ancestor(accept, GTK_TYPE_SCROLLED_WINDOW) == NULL);
    } else if (strcmp(probe.mode, "configuration-binding") == 0) {
        UmiGtk4BuildConfigurationCallbacks callbacks = {Capture, Load, Save, &probe};
        CHECK(UmiGtk4BuildSettingsBindConfigurations(probe.dialog, &callbacks) == UMI_STATUS_OK);
        GtkWidget *refresh = Find(form, "developer.configurations.refresh");
        GtkWidget *page = Find(form, "developer.settings.section.configurations");
        CHECK(GTK_IS_BUTTON(refresh) && gtk_widget_is_ancestor(refresh, page));
        CHECK(UmiGtk4BuildSettingsSelectSection(probe.dialog, UMI_GTK4_BUILD_SETTINGS_CONFIGURATIONS) == UMI_STATUS_OK);
        CHECK(probe.storage == 0U);
    } else if (strcmp(probe.mode, "independent-forms") == 0) {
        CHECK(UmiGtk4BuildSettingsDialogPrepare(probe.parent, &profile, 0, Applied, &second, &second.dialog) == UMI_STATUS_OK);
        CHECK(UmiGtk4BuildSettingsSelectSection(second.dialog, UMI_GTK4_BUILD_SETTINGS_SDK) == UMI_STATUS_OK);
        CHECK(UmiGtk4BuildSettingsCurrentSection(probe.dialog, &section) == UMI_STATUS_OK && section == UMI_GTK4_BUILD_SETTINGS_BUILD);
    } else if (strcmp(probe.mode, "owner-close") == 0 || strcmp(probe.mode, "parent-close") == 0 ||
               strcmp(probe.mode, "reentrant-selection") == 0) {
        changed = g_signal_connect(stack, "notify::visible-child-name", G_CALLBACK(SelectionChanged), &probe);
        status = UmiGtk4BuildSettingsSelectSection(probe.dialog, UMI_GTK4_BUILD_SETTINGS_RUN);
        if (strcmp(probe.mode, "reentrant-selection") == 0) {
            CHECK(status == UMI_STATUS_OK && probe.nested == UMI_STATUS_BUSY);
            CHECK(UmiGtk4BuildSettingsCurrentSection(probe.dialog, &section) == UMI_STATUS_OK && section == UMI_GTK4_BUILD_SETTINGS_RUN);
        } else CHECK(status == UMI_STATUS_CANCELLED && probe.observed >= 1U);
    } else if (strcmp(probe.mode, "retained-controls") == 0) {
        retained = g_object_ref(accept);
        Dispose(&probe);
        gtk_stack_set_visible_child_name(GTK_STACK(stack), "run");
        g_signal_emit_by_name(retained, "clicked");
        CHECK(probe.accepted == 0U);
    } else {
        GtkWidget *program = Find(form, "developer.build.field.8");
        GtkWidget *target = Find(form, "developer.build.field.7");
        CHECK(GTK_IS_ENTRY(program) && GTK_IS_ENTRY(target));
        gtk_editable_set_text(GTK_EDITABLE(program), "build/bin/notes");
        gtk_editable_set_text(GTK_EDITABLE(target), "notes");
        for (size_t i = 0U; i < sizeof Names / sizeof Names[0]; ++i) {
            CHECK(UmiGtk4BuildSettingsSelectSection(probe.dialog, (UmiGtk4BuildSettingsSection)i) == UMI_STATUS_OK);
            CHECK(UmiGtk4BuildSettingsCurrentSection(probe.dialog, &section) == UMI_STATUS_OK && section == (UmiGtk4BuildSettingsSection)i);
            CHECK(strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack)), Names[i]) == 0);
            CHECK(Find(form, "developer.build.field.8") == program);
        }
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(program)), "build/bin/notes") == 0);
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(trust)) && probe.accepted == 0U);
        CHECK(strcmp(profile.run_program, "build/bin/notes") != 0);
        if (strcmp(probe.mode, "same-section") == 0) {
            CHECK(UmiGtk4BuildSettingsSelectSection(probe.dialog, section) == UMI_STATUS_OK);
            CHECK(Find(form, "developer.build.field.7") == target);
        } else if (strcmp(probe.mode, "hidden-apply") == 0) {
            g_signal_emit_by_name(accept, "clicked"); CHECK(probe.accepted == 0U);
        } else if (strcmp(probe.mode, "apply-drafts") == 0 || strcmp(probe.mode, "cancel-drafts") == 0) {
            CHECK(UmiGtk4DeveloperDialogPresent(probe.dialog) == UMI_STATUS_OK);
            gtk_check_button_set_active(GTK_CHECK_BUTTON(trust), TRUE);
            if (strcmp(probe.mode, "apply-drafts") == 0) {
                g_signal_emit_by_name(accept, "clicked");
                CHECK(probe.accepted == 1U && probe.trusted);
                CHECK(strcmp(probe.applied.run_program, "build/bin/notes") == 0);
                CHECK(strcmp(probe.applied.build_target, "notes") == 0);
            } else {
                g_signal_emit_by_name(Find(form, "developer.dialog.cancel"), "clicked");
                CHECK(probe.accepted == 0U);
            }
        } else CHECK(!gtk_widget_get_visible(GTK_WIDGET(form)));
    }
cleanup:
    if (added != 0U) g_signal_handler_disconnect(windows, added);
    if (changed != 0U && stack != NULL) g_signal_handler_disconnect(stack, changed);
    Dispose(&second); Dispose(&probe);
    g_clear_object(&retained); g_clear_object(&stack);
    g_clear_object(&form);
    gtk_window_destroy(probe.parent); g_object_unref(probe.parent);
    return failed;
}
