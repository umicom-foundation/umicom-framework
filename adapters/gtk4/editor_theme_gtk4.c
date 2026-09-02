/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/editor_theme_gtk4.c
 *
 * PURPOSE:
 *   Apply Framework editor themes to GtkSourceView without exposing GTK types
 *   through public Framework contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "gtk4_internal.h"

#include "umicom/ui/editor_theme.h"

#if defined(UMICOM_GTK4_HAS_SOURCEVIEW5)
#include <gtksourceview/gtksource.h>
#endif

/*
 * Provide the active editor theme operation used by this module and its client
 * applications.
 */
static UmiStatus active_editor_theme(UmiUiWorkbench *workbench,
                                     UmiUiEditorThemeSnapshot *out_theme)
{
    UmiUiAppearanceProfile appearance;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || out_theme == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ui_appearance_model_active(
        umi_ui_workbench_appearance(workbench), &appearance);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) {
        status = umi_ui_appearance_profile_init(
            &appearance, "framework.editor.fallback", "Framework editor",
            UMI_UI_THEME_MODE_DARK, UMI_UI_DENSITY_COMFORTABLE);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_editor_theme_resolve(&appearance, out_theme);
}

#if defined(UMICOM_GTK4_HAS_SOURCEVIEW5)
/* Provide the find style scheme operation used by this module and its client applications. */
static GtkSourceStyleScheme *find_style_scheme(
    const UmiUiEditorThemeSnapshot *theme)
{
    GtkSourceStyleSchemeManager *manager =
        gtk_source_style_scheme_manager_get_default();
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < theme->preferred_scheme_count; ++index) {
        GtkSourceStyleScheme *scheme =
            gtk_source_style_scheme_manager_get_scheme(
                manager, theme->preferred_scheme_ids[index]);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (scheme != NULL) return scheme;
    }
    return NULL;
}

/*
 * Provide the apply source theme operation used by this module and its client
 * applications.
 */
static void apply_source_theme(GtkSourceView *view,
                               const UmiUiEditorThemeSnapshot *theme)
{
    GtkSourceBuffer *buffer;
    GtkSourceStyleScheme *scheme;
    GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    /* Apply this branch only when its contract condition is satisfied. */
    if (!GTK_SOURCE_IS_BUFFER(text_buffer)) return;
    buffer = GTK_SOURCE_BUFFER(text_buffer);
    scheme = find_style_scheme(theme);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (scheme != NULL) {
        gtk_source_buffer_set_style_scheme(buffer, scheme);
    }
    /* A light toolkit default caused an opaque white current-line rectangle
     * over dark editor text. Keep the feature only when a mode-compatible
     * GtkSourceView scheme was resolved by the Framework. */
    gtk_source_view_set_highlight_current_line(
        view, scheme != NULL && theme->highlight_current_line != 0);
}

/*
 * Provide the apply descendant themes operation used by this module and its client
 * applications.
 */
static void apply_descendant_themes(
    GtkWidget *widget,
    const UmiUiEditorThemeSnapshot *theme)
{
    GtkWidget *child;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (widget == NULL) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (GTK_SOURCE_IS_VIEW(widget)) {
        apply_source_theme(GTK_SOURCE_VIEW(widget), theme);
    }
    child = gtk_widget_get_first_child(widget);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        apply_descendant_themes(child, theme);
        child = next;
    }
}
#endif

/*
 * Provide the gtk4 configure editor theme operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_configure_editor_theme(GtkWidget *editor,
                                          UmiUiWorkbench *workbench)
{
    UmiUiEditorThemeSnapshot theme;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (editor == NULL || workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = active_editor_theme(workbench, &theme);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
#if defined(UMICOM_GTK4_HAS_SOURCEVIEW5)
    /* Apply this branch only when its contract condition is satisfied. */
    if (GTK_SOURCE_IS_VIEW(editor)) {
        apply_source_theme(GTK_SOURCE_VIEW(editor), &theme);
    }
#else
    (void)theme;
#endif
    return UMI_STATUS_OK;
}

/*
 * Provide the gtk4 apply editor themes operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_apply_editor_themes(UmiGtk4Adapter *adapter,
                                       UmiUiWorkbench *workbench)
{
    UmiUiEditorThemeSnapshot theme;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = active_editor_theme(workbench, &theme);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
#if defined(UMICOM_GTK4_HAS_SOURCEVIEW5)
    apply_descendant_themes(adapter->document_notebook, &theme);
    apply_descendant_themes(adapter->secondary_document_notebook, &theme);
#else
    (void)theme;
#endif
    return UMI_STATUS_OK;
}
