/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/gtk4_widget_catalogue.c
 *
 * PURPOSE:
 *   Maintain the GTK4 widget inventory and auditable exposure classification.
 *   No GTK headers are required, so headless tools can inspect coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/widget_catalogue.h"

#include <string.h>

#define WIDGET(id, type, group, mode, minor, deprecated, abstract, semantic) \
    {(id), (type), (group), (mode), 4U, (minor), (deprecated), (abstract), (semantic)}
#define SEM(id, type, group, semantic) \
    WIDGET((id), (type), (group), UMI_GTK4_EXPOSURE_SEMANTIC_FACTORY, 0U, 0U, 0, (semantic))
#define NATIVE(id, type, group, minor) \
    WIDGET((id), (type), (group), UMI_GTK4_EXPOSURE_BUILDER_NATIVE, (minor), 0U, 0, 0)
#define INTERNAL(id, type, group, minor) \
    WIDGET((id), (type), (group), UMI_GTK4_EXPOSURE_FRAMEWORK_INTERNAL, (minor), 0U, 1, 0)
#define LEGACY(id, type, group, deprecated) \
    WIDGET((id), (type), (group), UMI_GTK4_EXPOSURE_DEPRECATED_COMPATIBILITY, 0U, (deprecated), 0, 0)

/* This inventory follows GtkWidget descendants, including public subclasses.
 * Newer-than-4.10 rows are metadata-only until the runtime GTK version exposes
 * them, preserving the Framework's GTK 4.10 compilation floor. */
static const UmiGtk4WidgetDefinition WIDGETS[] = {
    SEM("gtk.window", "GtkWindow", UMI_GTK4_WIDGET_WINDOW, UMI_UI_COMPONENT_WINDOW),
    SEM("gtk.application-window", "GtkApplicationWindow", UMI_GTK4_WIDGET_WINDOW, UMI_UI_COMPONENT_WINDOW),
    NATIVE("gtk.about-dialog", "GtkAboutDialog", UMI_GTK4_WIDGET_WINDOW, 0U),
    NATIVE("gtk.assistant", "GtkAssistant", UMI_GTK4_WIDGET_WINDOW, 0U),
    NATIVE("gtk.dialog", "GtkDialog", UMI_GTK4_WIDGET_WINDOW, 0U),
    LEGACY("gtk.app-chooser-dialog", "GtkAppChooserDialog", UMI_GTK4_WIDGET_WINDOW, 10U),
    LEGACY("gtk.color-chooser-dialog", "GtkColorChooserDialog", UMI_GTK4_WIDGET_WINDOW, 10U),
    LEGACY("gtk.file-chooser-dialog", "GtkFileChooserDialog", UMI_GTK4_WIDGET_WINDOW, 10U),
    LEGACY("gtk.font-chooser-dialog", "GtkFontChooserDialog", UMI_GTK4_WIDGET_WINDOW, 10U),
    LEGACY("gtk.message-dialog", "GtkMessageDialog", UMI_GTK4_WIDGET_WINDOW, 10U),
    LEGACY("gtk.shortcuts-window", "GtkShortcutsWindow", UMI_GTK4_WIDGET_WINDOW, 18U),
    SEM("gtk.header-bar", "GtkHeaderBar", UMI_GTK4_WIDGET_WINDOW, UMI_UI_COMPONENT_HEADER_BAR),
    NATIVE("gtk.window-controls", "GtkWindowControls", UMI_GTK4_WIDGET_WINDOW, 0U),
    NATIVE("gtk.window-handle", "GtkWindowHandle", UMI_GTK4_WIDGET_WINDOW, 0U),

    NATIVE("gtk.action-bar", "GtkActionBar", UMI_GTK4_WIDGET_LAYOUT, 0U),
    NATIVE("gtk.aspect-frame", "GtkAspectFrame", UMI_GTK4_WIDGET_LAYOUT, 0U),
    SEM("gtk.box", "GtkBox", UMI_GTK4_WIDGET_LAYOUT, UMI_UI_COMPONENT_BOX),
    NATIVE("gtk.center-box", "GtkCenterBox", UMI_GTK4_WIDGET_LAYOUT, 0U),
    NATIVE("gtk.fixed", "GtkFixed", UMI_GTK4_WIDGET_LAYOUT, 0U),
    SEM("gtk.frame", "GtkFrame", UMI_GTK4_WIDGET_LAYOUT, UMI_UI_COMPONENT_FRAME),
    SEM("gtk.grid", "GtkGrid", UMI_GTK4_WIDGET_LAYOUT, UMI_UI_COMPONENT_GRID),
    SEM("gtk.overlay", "GtkOverlay", UMI_GTK4_WIDGET_LAYOUT, UMI_UI_COMPONENT_OVERLAY),
    SEM("gtk.paned", "GtkPaned", UMI_GTK4_WIDGET_LAYOUT, UMI_UI_COMPONENT_PANED),
    SEM("gtk.scrolled-window", "GtkScrolledWindow", UMI_GTK4_WIDGET_LAYOUT, UMI_UI_COMPONENT_SCROLLED),
    NATIVE("gtk.viewport", "GtkViewport", UMI_GTK4_WIDGET_LAYOUT, 0U),
    NATIVE("gtk.graphics-offload", "GtkGraphicsOffload", UMI_GTK4_WIDGET_LAYOUT, 14U),

    SEM("gtk.button", "GtkButton", UMI_GTK4_WIDGET_INPUT, UMI_UI_COMPONENT_BUTTON),
    SEM("gtk.check-button", "GtkCheckButton", UMI_GTK4_WIDGET_INPUT, UMI_UI_COMPONENT_CHECK_BUTTON),
    NATIVE("gtk.color-button", "GtkColorButton", UMI_GTK4_WIDGET_INPUT, 0U),
    NATIVE("gtk.color-dialog-button", "GtkColorDialogButton", UMI_GTK4_WIDGET_INPUT, 10U),
    NATIVE("gtk.drop-down", "GtkDropDown", UMI_GTK4_WIDGET_INPUT, 0U),
    NATIVE("gtk.editable-label", "GtkEditableLabel", UMI_GTK4_WIDGET_INPUT, 0U),
    SEM("gtk.entry", "GtkEntry", UMI_GTK4_WIDGET_INPUT, UMI_UI_COMPONENT_ENTRY),
    NATIVE("gtk.font-button", "GtkFontButton", UMI_GTK4_WIDGET_INPUT, 0U),
    NATIVE("gtk.font-dialog-button", "GtkFontDialogButton", UMI_GTK4_WIDGET_INPUT, 10U),
    NATIVE("gtk.link-button", "GtkLinkButton", UMI_GTK4_WIDGET_INPUT, 0U),
    NATIVE("gtk.lock-button", "GtkLockButton", UMI_GTK4_WIDGET_INPUT, 0U),
    NATIVE("gtk.menu-button", "GtkMenuButton", UMI_GTK4_WIDGET_INPUT, 0U),
    NATIVE("gtk.password-entry", "GtkPasswordEntry", UMI_GTK4_WIDGET_INPUT, 0U),
    NATIVE("gtk.scale", "GtkScale", UMI_GTK4_WIDGET_INPUT, 0U),
    NATIVE("gtk.scale-button", "GtkScaleButton", UMI_GTK4_WIDGET_INPUT, 0U),
    NATIVE("gtk.search-entry", "GtkSearchEntry", UMI_GTK4_WIDGET_INPUT, 0U),
    NATIVE("gtk.spin-button", "GtkSpinButton", UMI_GTK4_WIDGET_INPUT, 0U),
    SEM("gtk.switch", "GtkSwitch", UMI_GTK4_WIDGET_INPUT, UMI_UI_COMPONENT_SWITCH),
    NATIVE("gtk.toggle-button", "GtkToggleButton", UMI_GTK4_WIDGET_INPUT, 0U),
    NATIVE("gtk.volume-button", "GtkVolumeButton", UMI_GTK4_WIDGET_INPUT, 0U),
    LEGACY("gtk.app-chooser-button", "GtkAppChooserButton", UMI_GTK4_WIDGET_INPUT, 10U),
    LEGACY("gtk.combo-box", "GtkComboBox", UMI_GTK4_WIDGET_INPUT, 10U),
    LEGACY("gtk.combo-box-text", "GtkComboBoxText", UMI_GTK4_WIDGET_INPUT, 10U),

    NATIVE("gtk.calendar", "GtkCalendar", UMI_GTK4_WIDGET_DISPLAY, 0U),
    SEM("gtk.drawing-area", "GtkDrawingArea", UMI_GTK4_WIDGET_DISPLAY, UMI_UI_COMPONENT_DRAWING_SURFACE),
    SEM("gtk.expander", "GtkExpander", UMI_GTK4_WIDGET_DISPLAY, UMI_UI_COMPONENT_EXPANDER),
    NATIVE("gtk.image", "GtkImage", UMI_GTK4_WIDGET_DISPLAY, 0U),
    NATIVE("gtk.inscription", "GtkInscription", UMI_GTK4_WIDGET_DISPLAY, 8U),
    SEM("gtk.label", "GtkLabel", UMI_GTK4_WIDGET_DISPLAY, UMI_UI_COMPONENT_LABEL),
    NATIVE("gtk.level-bar", "GtkLevelBar", UMI_GTK4_WIDGET_DISPLAY, 0U),
    SEM("gtk.progress-bar", "GtkProgressBar", UMI_GTK4_WIDGET_DISPLAY, UMI_UI_COMPONENT_PROGRESS),
    NATIVE("gtk.revealer", "GtkRevealer", UMI_GTK4_WIDGET_DISPLAY, 0U),
    NATIVE("gtk.scrollbar", "GtkScrollbar", UMI_GTK4_WIDGET_DISPLAY, 0U),
    NATIVE("gtk.separator", "GtkSeparator", UMI_GTK4_WIDGET_DISPLAY, 0U),
    NATIVE("gtk.shortcut-label", "GtkShortcutLabel", UMI_GTK4_WIDGET_DISPLAY, 0U),
    SEM("gtk.spinner", "GtkSpinner", UMI_GTK4_WIDGET_DISPLAY, UMI_UI_COMPONENT_SPINNER),
    NATIVE("gtk.text", "GtkText", UMI_GTK4_WIDGET_DISPLAY, 0U),
    SEM("gtk.text-view", "GtkTextView", UMI_GTK4_WIDGET_DISPLAY, UMI_UI_COMPONENT_TEXT_VIEW),
    NATIVE("gtk.svg-widget", "GtkSvgWidget", UMI_GTK4_WIDGET_DISPLAY, 20U),
    LEGACY("gtk.info-bar", "GtkInfoBar", UMI_GTK4_WIDGET_DISPLAY, 10U),
    LEGACY("gtk.statusbar", "GtkStatusbar", UMI_GTK4_WIDGET_DISPLAY, 10U),

    INTERNAL("gtk.list-base", "GtkListBase", UMI_GTK4_WIDGET_COLLECTION, 0U),
    INTERNAL("gtk.range", "GtkRange", UMI_GTK4_WIDGET_COLLECTION, 0U),
    NATIVE("gtk.column-view", "GtkColumnView", UMI_GTK4_WIDGET_COLLECTION, 0U),
    NATIVE("gtk.flow-box", "GtkFlowBox", UMI_GTK4_WIDGET_COLLECTION, 0U),
    INTERNAL("gtk.flow-box-child", "GtkFlowBoxChild", UMI_GTK4_WIDGET_COLLECTION, 0U),
    NATIVE("gtk.grid-view", "GtkGridView", UMI_GTK4_WIDGET_COLLECTION, 0U),
    NATIVE("gtk.list-box", "GtkListBox", UMI_GTK4_WIDGET_COLLECTION, 0U),
    INTERNAL("gtk.list-box-row", "GtkListBoxRow", UMI_GTK4_WIDGET_COLLECTION, 0U),
    NATIVE("gtk.list-view", "GtkListView", UMI_GTK4_WIDGET_COLLECTION, 0U),
    NATIVE("gtk.tree-expander", "GtkTreeExpander", UMI_GTK4_WIDGET_COLLECTION, 0U),
    LEGACY("gtk.cell-view", "GtkCellView", UMI_GTK4_WIDGET_COLLECTION, 10U),
    LEGACY("gtk.icon-view", "GtkIconView", UMI_GTK4_WIDGET_COLLECTION, 10U),
    LEGACY("gtk.tree-view", "GtkTreeView", UMI_GTK4_WIDGET_COLLECTION, 10U),

    SEM("gtk.notebook", "GtkNotebook", UMI_GTK4_WIDGET_NAVIGATION, UMI_UI_COMPONENT_TAB_HOST),
    SEM("gtk.stack", "GtkStack", UMI_GTK4_WIDGET_NAVIGATION, UMI_UI_COMPONENT_STACK),
    NATIVE("gtk.stack-sidebar", "GtkStackSidebar", UMI_GTK4_WIDGET_NAVIGATION, 0U),
    SEM("gtk.stack-switcher", "GtkStackSwitcher", UMI_GTK4_WIDGET_NAVIGATION, UMI_UI_COMPONENT_STACK_SWITCHER),
    NATIVE("gtk.search-bar", "GtkSearchBar", UMI_GTK4_WIDGET_NAVIGATION, 0U),
    NATIVE("gtk.popover", "GtkPopover", UMI_GTK4_WIDGET_NAVIGATION, 0U),
    NATIVE("gtk.popover-menu", "GtkPopoverMenu", UMI_GTK4_WIDGET_NAVIGATION, 0U),
    NATIVE("gtk.popover-menu-bar", "GtkPopoverMenuBar", UMI_GTK4_WIDGET_NAVIGATION, 0U),
    NATIVE("gtk.emoji-chooser", "GtkEmojiChooser", UMI_GTK4_WIDGET_NAVIGATION, 0U),
    INTERNAL("gtk.popover-bin", "GtkPopoverBin", UMI_GTK4_WIDGET_NAVIGATION, 0U),

    SEM("gtk.picture", "GtkPicture", UMI_GTK4_WIDGET_MEDIA, UMI_UI_COMPONENT_PICTURE),
    SEM("gtk.video", "GtkVideo", UMI_GTK4_WIDGET_MEDIA, UMI_UI_COMPONENT_VIDEO),
    INTERNAL("gtk.media-controls", "GtkMediaControls", UMI_GTK4_WIDGET_MEDIA, 0U),
    NATIVE("gtk.gl-area", "GtkGLArea", UMI_GTK4_WIDGET_MEDIA, 0U),

    LEGACY("gtk.app-chooser-widget", "GtkAppChooserWidget", UMI_GTK4_WIDGET_SPECIALIST, 10U),
    LEGACY("gtk.color-chooser-widget", "GtkColorChooserWidget", UMI_GTK4_WIDGET_SPECIALIST, 10U),
    LEGACY("gtk.file-chooser-widget", "GtkFileChooserWidget", UMI_GTK4_WIDGET_SPECIALIST, 10U),
    LEGACY("gtk.font-chooser-widget", "GtkFontChooserWidget", UMI_GTK4_WIDGET_SPECIALIST, 10U),
    NATIVE("gtk.drag-icon", "GtkDragIcon", UMI_GTK4_WIDGET_SPECIALIST, 0U),
    INTERNAL("gtk.shortcuts-shortcut", "GtkShortcutsShortcut", UMI_GTK4_WIDGET_SPECIALIST, 0U)
};

/*
 * Return the number of records represented by gtk4 widget catalogue without changing their
 * state.
 */
size_t umi_gtk4_widget_catalogue_count(void)
{
    return sizeof(WIDGETS) / sizeof(WIDGETS[0]);
}

/*
 * Find gtk4 widget catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiGtk4WidgetDefinition *umi_gtk4_widget_catalogue_at(size_t index)
{
    return index < umi_gtk4_widget_catalogue_count() ? &WIDGETS[index] : NULL;
}

/*
 * Find gtk4 widget catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiGtk4WidgetDefinition *umi_gtk4_widget_catalogue_find(
    const char *gtk_type_name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (gtk_type_name == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_gtk4_widget_catalogue_count(); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(WIDGETS[index].gtk_type_name, gtk_type_name) == 0)
            return &WIDGETS[index];
    }
    return NULL;
}

/*
 * Return the number of records represented by gtk4 widget category without changing their
 * state.
 */
size_t umi_gtk4_widget_category_count(UmiGtk4WidgetCategory category)
{
    size_t index;
    size_t count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_gtk4_widget_catalogue_count(); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (WIDGETS[index].category == category) count += 1U;
    }
    return count;
}

/*
 * Provide the gtk4 widget coverage operation used by this module and its client
 * applications.
 */
UmiGtk4CoverageReport umi_gtk4_widget_coverage(void)
{
    UmiGtk4CoverageReport report = {0};
    size_t index;
    report.widget_count = umi_gtk4_widget_catalogue_count();
    report.minimum_major = 4U;
    report.minimum_minor = 10U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < report.widget_count; ++index) {
        /* Select the behaviour associated with the requested command or state value. */
        switch (WIDGETS[index].exposure) {
            case UMI_GTK4_EXPOSURE_SEMANTIC_FACTORY:
                report.semantic_factory_count += 1U;
                break;
            case UMI_GTK4_EXPOSURE_BUILDER_NATIVE:
                report.builder_native_count += 1U;
                break;
            case UMI_GTK4_EXPOSURE_FRAMEWORK_INTERNAL:
                report.framework_internal_count += 1U;
                break;
            case UMI_GTK4_EXPOSURE_DEPRECATED_COMPATIBILITY:
                report.deprecated_compatibility_count += 1U;
                break;
            default:
                report.uncatalogued_count += 1U;
                break;
        }
    }
    report.complete = report.uncatalogued_count == 0U;
    return report;
}

/*
 * Provide the gtk4 widget category text operation used by this module and its client
 * applications.
 */
const char *umi_gtk4_widget_category_text(UmiGtk4WidgetCategory category)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (category) {
        case UMI_GTK4_WIDGET_WINDOW: return "window";
        case UMI_GTK4_WIDGET_LAYOUT: return "layout";
        case UMI_GTK4_WIDGET_INPUT: return "input";
        case UMI_GTK4_WIDGET_DISPLAY: return "display";
        case UMI_GTK4_WIDGET_COLLECTION: return "collection";
        case UMI_GTK4_WIDGET_NAVIGATION: return "navigation";
        case UMI_GTK4_WIDGET_MEDIA: return "media";
        case UMI_GTK4_WIDGET_FEEDBACK: return "feedback";
        case UMI_GTK4_WIDGET_SPECIALIST: return "specialist";
        default: return "unknown";
    }
}

/*
 * Provide the gtk4 widget exposure text operation used by this module and its client
 * applications.
 */
const char *umi_gtk4_widget_exposure_text(UmiGtk4WidgetExposure exposure)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (exposure) {
        case UMI_GTK4_EXPOSURE_SEMANTIC_FACTORY: return "semantic-factory";
        case UMI_GTK4_EXPOSURE_BUILDER_NATIVE: return "builder-native";
        case UMI_GTK4_EXPOSURE_FRAMEWORK_INTERNAL: return "framework-internal";
        case UMI_GTK4_EXPOSURE_DEPRECATED_COMPATIBILITY:
            return "deprecated-compatibility";
        default: return "unknown";
    }
}
