/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/tool_rail_gtk4.c
 *
 * PURPOSE:
 *   Render reusable vertical or horizontal workstation activity/tool rails for app-specific surface launchers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/tool_rail.h"

/* Limit the caption's long axis, not the user's font size. The enclosing rail
 * remains responsible for scrolling when its collection exceeds the viewport. */
#define TOOL_RAIL_CAPTION_EXTENT 240

typedef struct UmiToolRailCaption {
    GtkWidget parent_instance;
    char *title;
    gboolean vertical;
} UmiToolRailCaption;

typedef struct UmiToolRailCaptionClass {
    GtkWidgetClass parent_class;
} UmiToolRailCaptionClass;

/* Keep the rotated text implementation private to the shared GTK adapter. */
static GType umi_tool_rail_caption_get_type(void);
G_DEFINE_TYPE(UmiToolRailCaption, umi_tool_rail_caption, GTK_TYPE_WIDGET)

/* Recreate the small layout from the widget's current Pango context so font,
 * language and scale changes are reflected without a stale cached layout. */
static PangoLayout *tool_rail_caption_layout(GtkWidget *widget, int extent)
{
    UmiToolRailCaption *caption = (UmiToolRailCaption *)widget;
    PangoLayout *layout = gtk_widget_create_pango_layout(
        widget, caption->title != NULL ? caption->title : "");
    int text_width;

    pango_layout_set_single_paragraph_mode(layout, TRUE);
    pango_layout_get_pixel_size(layout, &text_width, NULL);
    extent = MAX(1, MIN(extent, TOOL_RAIL_CAPTION_EXTENT));
    if (text_width > extent) {
        pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);
        pango_layout_set_width(layout, extent * PANGO_SCALE);
    }
    return layout;
}

/* Swap the measured axes for a vertical caption. Button padding and the rail
 * scroll container are handled by their own widgets, not baked into the text. */
static void tool_rail_caption_measure(
    GtkWidget *widget, GtkOrientation orientation, int for_size,
    int *minimum, int *natural, int *minimum_baseline, int *natural_baseline)
{
    UmiToolRailCaption *caption = (UmiToolRailCaption *)widget;
    PangoLayout *layout = tool_rail_caption_layout(widget, TOOL_RAIL_CAPTION_EXTENT);
    int width;
    int height;
    int extent;

    (void)for_size;
    pango_layout_get_pixel_size(layout, &width, &height);
    extent = (orientation == GTK_ORIENTATION_HORIZONTAL) != caption->vertical
        ? width : height;
    if (minimum != NULL) *minimum = MAX(1, extent);
    if (natural != NULL) *natural = MAX(1, extent);
    if (minimum_baseline != NULL) *minimum_baseline = -1;
    if (natural_baseline != NULL) *natural_baseline = -1;
    g_object_unref(layout);
}

/* GTK snapshots render Pango text using the normal graphics backend. Rotating
 * its coordinate system keeps complete words, shaping and Unicode intact. */
static void tool_rail_caption_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    UmiToolRailCaption *caption = (UmiToolRailCaption *)widget;
    int width = gtk_widget_get_width(widget);
    int height = gtk_widget_get_height(widget);
    PangoLayout *layout;
    GdkRGBA colour;
    graphene_point_t origin;
    int text_width;
    int text_height;

    if (width <= 0 || height <= 0) return;
    layout = tool_rail_caption_layout(widget, caption->vertical ? height : width);
    pango_layout_get_pixel_size(layout, &text_width, &text_height);
    gtk_widget_get_color(widget, &colour);
    if (caption->vertical) {
        graphene_point_init(&origin,
            ((float)width + (float)text_height) / 2.0f,
            ((float)height - (float)text_width) / 2.0f);
    } else {
        graphene_point_init(&origin,
            ((float)width - (float)text_width) / 2.0f,
            ((float)height - (float)text_height) / 2.0f);
    }
    gtk_snapshot_save(snapshot);
    gtk_snapshot_translate(snapshot, &origin);
    if (caption->vertical) gtk_snapshot_rotate(snapshot, 90.0f);
    gtk_snapshot_append_layout(snapshot, layout, &colour);
    gtk_snapshot_restore(snapshot);
    g_object_unref(layout);
}

/* A theme or user-font change can alter both the text's length and thickness. */
static void tool_rail_caption_css_changed(GtkWidget *widget, GtkCssStyleChange *change)
{
    GtkWidgetClass *parent_class = GTK_WIDGET_CLASS(umi_tool_rail_caption_parent_class);
    if (parent_class->css_changed != NULL) parent_class->css_changed(widget, change);
    gtk_widget_queue_resize(widget);
    gtk_widget_queue_draw(widget);
}

/* The caption owns a value copy, so temporary product catalogue labels remain
 * valid even after the caller releases its original string. */
static void tool_rail_caption_finalize(GObject *object)
{
    UmiToolRailCaption *caption = (UmiToolRailCaption *)object;
    g_free(caption->title);
    G_OBJECT_CLASS(umi_tool_rail_caption_parent_class)->finalize(object);
}

/* The button exposes the full accessible name; its drawn child is decorative
 * and must not be announced as a second control by assistive technology. */
static void umi_tool_rail_caption_class_init(UmiToolRailCaptionClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    G_OBJECT_CLASS(klass)->finalize = tool_rail_caption_finalize;
    widget_class->measure = tool_rail_caption_measure;
    widget_class->snapshot = tool_rail_caption_snapshot;
    widget_class->css_changed = tool_rail_caption_css_changed;
    gtk_widget_class_set_css_name(widget_class, "label");
    gtk_widget_class_set_accessible_role(widget_class, GTK_ACCESSIBLE_ROLE_PRESENTATION);
}

/* Do not let a narrow allocation paint over a neighbouring edge tab. */
static void umi_tool_rail_caption_init(UmiToolRailCaption *caption)
{
    gtk_widget_set_overflow(GTK_WIDGET(caption), GTK_OVERFLOW_HIDDEN);
    gtk_widget_add_css_class(GTK_WIDGET(caption), "umicom-tool-rail-caption");
}

/*
 * Initialise gtk4 ws tool rail from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_tool_rail_create(GtkOrientation orientation) {
    GtkWidget *rail = gtk_box_new(orientation, 4);
    gtk_widget_add_css_class(rail, "umicom-tool-rail");
    return rail;
}

/* Add gtk4 ws tool rail only after its inputs and available capacity have been checked. */
GtkWidget *umi_gtk4_ws_tool_rail_append(GtkWidget *rail, const char *icon_name, const char *tooltip, gboolean active) {
    GtkWidget *button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (rail == NULL || !GTK_IS_BOX(rail)) return NULL;
    button = gtk_toggle_button_new();
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (icon_name != NULL && icon_name[0] != '\0') gtk_button_set_icon_name(GTK_BUTTON(button), icon_name);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tooltip != NULL) gtk_widget_set_tooltip_text(button, tooltip);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), active);
    gtk_widget_add_css_class(button, "umicom-tool-rail-item");
    gtk_box_append(GTK_BOX(rail), button);
    return button;
}

/* Named tabs share the existing toggle-button appearance and ownership. Only
 * caption rendering differs; the host retains all reveal and docking policy. */
GtkWidget *umi_gtk4_ws_tool_rail_append_label(
    GtkWidget *rail, const char *title, gboolean vertical, gboolean active)
{
    GtkWidget *button;
    UmiToolRailCaption *caption;

    if (rail == NULL || !GTK_IS_BOX(rail) || title == NULL || title[0] == '\0' ||
        !g_utf8_validate(title, -1, NULL)) return NULL;
    button = gtk_toggle_button_new();
    caption = g_object_new(umi_tool_rail_caption_get_type(), NULL);
    caption->title = g_strdup(title);
    caption->vertical = vertical != FALSE;
    gtk_button_set_child(GTK_BUTTON(button), GTK_WIDGET(caption));
    gtk_widget_set_tooltip_text(button, title);
    gtk_accessible_update_property(GTK_ACCESSIBLE(button),
        GTK_ACCESSIBLE_PROPERTY_LABEL, title, -1);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), active);
    gtk_widget_add_css_class(button, "umicom-tool-rail-item");
    gtk_widget_add_css_class(button, "umicom-tool-rail-named-item");
    gtk_box_append(GTK_BOX(rail), button);
    return button;
}
