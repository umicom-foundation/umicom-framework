/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_typography_gtk4.c
 *
 * PURPOSE:
 *   Check shared font projection, locale independence and actual workstation code-role styling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/appearance_typography_private.h"
#include "umicom/ui/gtk4/workstation/appearance_editor.h"
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr, "%d: %s\n", __LINE__, #condition); return 1; } } while (0)

static void CssError(GtkCssProvider *provider, GtkCssSection *section, const GError *error, void *data)
{
    (void)provider; (void)section;
    unsigned *errors = data;
    ++*errors;
    fprintf(stderr, "Typography CSS: %s\n", error->message);
}
static int Format(gboolean numeric_locale)
{
    UmiUiAppearanceProfile profile;
    CHECK(umi_ui_appearance_profile_init(&profile, "font-test", "Font test",
        UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) == UMI_STATUS_OK);
    char *saved = g_strdup(setlocale(LC_NUMERIC, NULL));
    if (numeric_locale) {
        const char *candidates[] = {"de_DE.UTF-8", "de_DE.utf8", "German_Germany.1252", "de-DE"};
        gboolean selected = FALSE;
        for (size_t i = 0U; i < G_N_ELEMENTS(candidates); ++i) {
            if (setlocale(LC_NUMERIC, candidates[i]) != NULL && strcmp(localeconv()->decimal_point, ",") == 0) {
                selected = TRUE; break;
            }
        }
        if (!selected) {
            (void)setlocale(LC_NUMERIC, saved); g_free(saved);
            fputs("SKIP: no installed comma-decimal locale for this regression.\n", stderr);
            return 77;
        }
    }
    snprintf(profile.interface_font, sizeof(profile.interface_font), "3D.Font, Segoe UI, Sans");
    profile.interface_font_size = 10.25;
    profile.editor_font_size = 11.5;
    profile.font_scale = 1.5;
    char *css = UmiGtk4AppearanceTypographyCss(&profile, TRUE);
    gboolean locale_unchanged = !numeric_locale || strcmp(localeconv()->decimal_point, ",") == 0;
    (void)setlocale(LC_NUMERIC, saved); g_free(saved);
    gboolean valid = css != NULL && strstr(css, "\"3D.Font\", \"Segoe UI\", sans-serif") != NULL &&
        strstr(css, "font-size: 15.38pt") != NULL && strstr(css, "font-size: 17.25pt") != NULL &&
        strstr(css, "textview.monospace text") != NULL;
    g_free(css);
    CHECK(locale_unchanged && valid);
    return 0;
}
static int Invalid(void)
{
    UmiUiAppearanceProfile profile;
    CHECK(umi_ui_appearance_profile_init(&profile, "font-test", "Font test",
        UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) == UMI_STATUS_OK);
    profile.font_scale = NAN;
    CHECK(UmiGtk4AppearanceTypographyCss(&profile, FALSE) == NULL);
    profile.font_scale = 1.0;
    memset(profile.editor_font, 'x', sizeof(profile.editor_font));
    CHECK(UmiGtk4AppearanceTypographyCss(&profile, TRUE) == NULL);
    return 0;
}
static int Native(gboolean workstation)
{
    if (!gtk_init_check()) return 77;
    g_log_set_always_fatal(G_LOG_FATAL_MASK | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL);
    GtkWidget *window = gtk_window_new();
    g_object_ref_sink(window);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *ordinary = gtk_label_new("Interface text");
    GtkWidget *code = gtk_label_new("int main(void)");
    gtk_widget_add_css_class(code, "umicom-code-font");
    gtk_box_append(GTK_BOX(box), ordinary);
    gtk_box_append(GTK_BOX(box), code);
    gtk_window_set_child(GTK_WINDOW(window), box);
    UmiUiAppearanceProfile profile;
    UmiGtk4AppearanceEditor *editor = NULL;
    GtkCssProvider *provider = NULL;
    unsigned errors = 0U;
    int result = 1;
    if (workstation) {
        /* A unique identity avoids loading an owner's saved preferences.
         * Creation reads defaults only; this case never saves a selection. */
        char identity[80];
        snprintf(identity, sizeof(identity), "typography-test-%" G_GINT64_FORMAT, g_get_real_time());
        UmiGtk4AppearanceEditorConfig config = umi_gtk4_appearance_editor_config_default(identity);
        if (umi_gtk4_appearance_editor_create(box, &config, &editor) != UMI_STATUS_OK) goto cleanup;
        if (umi_gtk4_appearance_editor_active(editor, &profile) != UMI_STATUS_OK) goto cleanup;
    } else {
        if (umi_ui_appearance_profile_init(&profile, "font-test", "Font test",
            UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMPACT) != UMI_STATUS_OK) goto cleanup;
        profile.interface_font_size = 12.0;
        profile.editor_font_size = 24.0;
        gtk_widget_add_css_class(window, "umicom-workbench");
        char *css = UmiGtk4AppearanceTypographyCss(&profile, TRUE);
        if (css == NULL) goto cleanup;
        provider = gtk_css_provider_new();
        g_signal_connect(provider, "parsing-error", G_CALLBACK(CssError), &errors);
        gtk_css_provider_load_from_string(provider, css);
        g_free(css);
        gtk_style_context_add_provider_for_display(gtk_widget_get_display(window),
            GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 20U);
    }
    gtk_window_present(GTK_WINDOW(window));
    gint64 until = g_get_monotonic_time() + 2000000;
    while (g_get_monotonic_time() < until) {
        for (unsigned pending = 0U; pending < 32U && g_main_context_iteration(NULL, FALSE); ++pending) { }
        const PangoFontDescription *normal = pango_context_get_font_description(gtk_widget_get_pango_context(ordinary));
        const PangoFontDescription *mono = pango_context_get_font_description(gtk_widget_get_pango_context(code));
        int normal_size = pango_font_description_get_size(normal);
        int code_size = pango_font_description_get_size(mono);
        double expected_ratio = profile.editor_font_size / profile.interface_font_size;
        if (normal_size > 0 && fabs((double)code_size / normal_size - expected_ratio) < 0.02 &&
            strstr(pango_font_description_get_family(mono), "Cascadia Code") != NULL && errors == 0U) {
            result = 0; break;
        }
        g_usleep(1000);
    }
    if (result != 0) fputs("Native interface and code typography did not match the active profile.\n", stderr);
cleanup:
    umi_gtk4_appearance_editor_destroy(editor);
    if (provider != NULL) {
        gtk_style_context_remove_provider_for_display(gtk_widget_get_display(window), GTK_STYLE_PROVIDER(provider));
        g_object_unref(provider);
    }
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
    return result;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "format") == 0) return Format(FALSE);
    if (strcmp(argv[1], "locale") == 0) return Format(TRUE);
    if (strcmp(argv[1], "invalid") == 0) return Invalid();
    if (strcmp(argv[1], "workbench") == 0) return Native(FALSE);
    if (strcmp(argv[1], "workstation") == 0) return Native(TRUE);
    return 2;
}
