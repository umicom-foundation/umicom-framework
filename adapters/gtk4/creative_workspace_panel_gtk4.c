/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_workspace_panel_gtk4.c
 * PURPOSE: Build a reusable creative editor with explicit edits, stable control IDs and safe teardown.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "creative_workspace_private.h"
#include <string.h>
/* Each connected object is retained until its signal has been disconnected.
 * A caller retaining a button beyond the panel lifetime gets an inert widget. */
void UmiCreativeGtkTrack(UmiCreativeGtkPanel * panel, GObject * object)
{
    g_ptr_array_add(panel -> connections, g_object_ref(object));
}

static void Tag(GtkWidget * widget, const char * id)
{
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free);
}

static GtkWidget * Text(GtkWidget * box, const char * label)
{
    GtkWidget * widget = gtk_label_new(label);
    gtk_label_set_xalign(GTK_LABEL(widget), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(widget), TRUE);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}

static GtkWidget * Entry(GtkWidget * box, const char * label, const char * id, const char * initial)
{
    Text(box, label);
    GtkWidget * widget = gtk_entry_new();
    Tag(widget, id);
    gtk_editable_set_text(GTK_EDITABLE(widget), initial);
    gtk_widget_set_hexpand(widget, TRUE);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}

static GtkWidget * Number(GtkWidget * box, const char * label, const char * id, double minimum, double maximum, double initial)
{
    Text(box, label);
    GtkWidget * widget = gtk_spin_button_new_with_range(minimum, maximum, 1.0);
    Tag(widget, id);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), initial);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}

static GtkWidget * Choice(GtkWidget * box, const char * label, const char * id, const char * const * items)
{
    Text(box, label);
    GtkWidget * widget = gtk_drop_down_new_from_strings(items);
    Tag(widget, id);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}

static GtkWidget * Button(UmiCreativeGtkPanel * panel, GtkWidget * box, const char * label, const char * action)
{
    GtkWidget * button = gtk_button_new_with_label(label);
    char * tag = g_strdup_printf("creative.%s", action);
    Tag(button, tag);
    g_free(tag);
    g_object_set_data(G_OBJECT(button), "creative-action", (gpointer)action);
    UmiCreativeGtkTrack(panel, G_OBJECT(button));
    g_signal_connect(button, "clicked", G_CALLBACK(UmiCreativeGtkAction), panel);
    gtk_box_append(GTK_BOX(box), button);
    return button;
}

static GtkWidget * Page(GtkNotebook * notebook, const char * label)
{
    GtkWidget * box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_margin_start(box, 10);
    gtk_widget_set_margin_end(box, 10);
    gtk_widget_set_margin_top(box, 10);
    gtk_widget_set_margin_bottom(box, 10);
    GtkWidget * scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);
    gtk_notebook_append_page(notebook, scroll, gtk_label_new(label));
    return box;
}

static void BuildForms(UmiCreativeGtkPanel * p, GtkNotebook * notebook)
{
    static const char * const units[] = {"pixels", "millimetres", NULL};
    static const char * const kinds[] = {"Rectangle", "Text", "Panel", "Cabinet", "Actor", "Obstacle", NULL};
    static const char * const interpolation[] = {"Hold until next key", "Linear movement", NULL};
    static const char * const exports[] = {"SVG drawing", "Responsive visual page", "Three-frame storyboard", "PCM-WAVE notes", NULL};
    GtkWidget * page = Page(notebook, "Project");
    p -> projectId = Entry(page, "Project ID (letters, digits, . _ -)", "creative.project.id", "workshop");
    p -> title = Entry(page, "Project title", "creative.project.title", "Workshop design");
    Button(p, page, "Create new project", "project.create");
    Button(p, page, "Open project by ID", "project.open");
    Text(page, "Create never overwrites an existing project. Open reads the saved project.");
    p -> unit = Choice(page, "Drawing units", "creative.settings.unit", units);
    p -> width = Number(page, "Canvas width", "creative.settings.width", 1, 1000000, 960);
    p -> height = Number(page, "Canvas height", "creative.settings.height", 1, 1000000, 600);
    p -> duration = Number(page, "Duration in milliseconds", "creative.settings.duration", 1, 30000, 8000);
    p -> tempo = Number(page, "Tempo in beats per minute", "creative.settings.tempo", 20, 300, 120);
    Button(p, page, "Save project settings", "settings.save");
    Text(page, "Units cannot change while objects remain in a design. Every successful edit is saved immediately.");
    page = Page(notebook, "Objects");
    p -> elementId = Entry(page, "Object ID", "creative.object.id", "object-1");
    Button(p, page, "Load object into form", "object.load");
    p -> label = Entry(page, "Visible label", "creative.object.label", "Workshop table");
    p -> kind = Choice(page, "Object type", "creative.object.kind", kinds);
    p -> x = Number(page, "Left (x)", "creative.object.x", 0, 1000000, 40);
    p -> y = Number(page, "Top (y)", "creative.object.y", 0, 1000000, 120);
    p -> objectWidth = Number(page, "Width", "creative.object.width", 1, 1000000, 200);
    p -> objectHeight = Number(page, "Height", "creative.object.height", 1, 1000000, 100);
    p -> colour = Entry(page, "Colour as #RRGGBB", "creative.object.colour", "#609EB0");
    p -> visible = gtk_check_button_new_with_label("Visible");
    Tag(p -> visible, "creative.object.visible");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(p -> visible), TRUE);
    gtk_box_append(GTK_BOX(page), p -> visible);
    Button(p, page, "Add object", "object.add");
    Button(p, page, "Update object", "object.update");
    Button(p, page, "Remove object", "object.remove");
    Button(p, page, "Send to back", "object.back");
    Button(p, page, "Bring to front", "object.front");
    Text(page, "Click the drawing to load the topmost visible object. Form edits are not saved until Add or Update.");
    page = Page(notebook, "Motion");
    Text(page, "Keys belong to the Object ID shown in Objects. Offsets are measured from the object's saved left/top position. The first key must be at time zero.");
    p -> keyTime = Number(page, "Key time (milliseconds)", "creative.key.time", 0, 30000, 0);
    p -> offsetX = Number(page, "Horizontal offset", "creative.key.x", -1000000, 1000000, 0);
    p -> offsetY = Number(page, "Vertical offset", "creative.key.y", -1000000, 1000000, 0);
    p -> interpolation = Choice(page, "Movement after this key", "creative.key.interpolation", interpolation);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(p -> interpolation), 1U);
    Button(p, page, "Save or replace key at this time", "key.save");
    Button(p, page, "Remove key at this time", "key.remove");
    Text(page, "Collision playground: select an Actor by its Object ID. Movement is temporary and never changes the saved design. Animated scenes are not accepted here.");
    Button(p, page, "Reset play position", "play.reset");
    Button(p, page, "Move left 20 units", "play.left");
    Button(p, page, "Move right 20 units", "play.right");
    Button(p, page, "Move up 20 units", "play.up");
    Button(p, page, "Move down 20 units", "play.down");
    page = Page(notebook, "Notes");
    Text(page, "One beat is 480 ticks. Notes are rendered to a WAVE export, not sent to an audio device. A note must end within the project's duration.");
    p -> noteId = Entry(page, "Note ID", "creative.note.id", "note-1");
    p -> startTick = Number(page, "Start tick", "creative.note.start", 0, 100000, 0);
    p -> durationTicks = Number(page, "Length in ticks", "creative.note.duration", 1, 100000, 480);
    p -> pitch = Number(page, "Pitch number (69 = A4)", "creative.note.pitch", 0, 127, 69);
    p -> velocity = Number(page, "Velocity", "creative.note.velocity", 1, 127, 80);
    Button(p, page, "Add note", "note.add");
    Button(p, page, "Update note", "note.update");
    Button(p, page, "Remove note", "note.remove");
    page = Page(notebook, "Export");
    Text(page, "Export uses the saved project, not unsaved form fields. SVG uses the displayed time. Storyboards sample start, middle and end. Existing files are never overwritten.");
    p -> exportKind = Choice(page, "Export format", "creative.export.kind", exports);
    p -> exportPath = Entry(page, "New absolute output file path", "creative.export.path", "");
    Button(p, page, "Suggest an output path", "export.suggest");
    Button(p, page, "Write a new export file", "export.write");
    Text(page, "The page is a visual prototype, not a deployed website or mobile package. The storyboard is not a video. WAVE uses a simple sine-wave instrument.");
}

UmiStatus UmiCreativeGtkPanelCreate(UmiDataServer * server, const char * profile, UmiCreativeGtkPanel * * out)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    * out = NULL;
    if (profile == NULL || strlen(profile) >= 32U)return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(profile, "cad")!= 0 && strcmp(profile, "kitchen")!= 0 && strcmp(profile, "media")!= 0 && strcmp(profile, "music")!= 0 && strcmp(profile, "games")!= 0 && strcmp(profile, "web-studio")!= 0 && strcmp(profile, "mobile-studio")!= 0)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeGtkPanel * p = g_new0(UmiCreativeGtkPanel, 1);
    p -> project = g_new0(UmiCreativeProject, 1);
    p -> server = server;
    (void)g_strlcpy(p -> profile, profile, sizeof(p -> profile));
    p -> connections = g_ptr_array_new_with_free_func(g_object_unref);
    p -> root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    g_object_ref_sink(p -> root);
    Tag(p -> root, "creative.workspace");
    p -> storage = Text(p -> root, server!= NULL ? "Injected Data Server; no project is open." : "Local project storage opens only when you choose Create or Open. Projects are plaintext.");
    GtkWidget * toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_append(GTK_BOX(p -> root), toolbar);
    Button(p, toolbar, "Load starter into empty project", "project.starter");
    Button(p, toolbar, "Undo", "history.undo");
    Button(p, toolbar, "Redo", "history.redo");
    Button(p, toolbar, "Reload saved project", "project.reload");
    GtkWidget * paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_vexpand(paned, TRUE);
    gtk_box_append(GTK_BOX(p -> root), paned);
    GtkWidget * notebook = gtk_notebook_new();
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
    gtk_widget_set_size_request(notebook, 290, -1);
    gtk_paned_set_start_child(GTK_PANED(paned), notebook);
    gtk_paned_set_resize_start_child(GTK_PANED(paned), FALSE);
    gtk_paned_set_position(GTK_PANED(paned), 330);
    BuildForms(p, GTK_NOTEBOOK(notebook));
    GtkWidget * right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_paned_set_end_child(GTK_PANED(paned), right);
    p -> canvas = gtk_drawing_area_new();
    Tag(p -> canvas, "creative.canvas");
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(p -> canvas), 600);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(p -> canvas), 340);
    gtk_widget_set_hexpand(p -> canvas, TRUE);
    gtk_widget_set_vexpand(p -> canvas, TRUE);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(p -> canvas), UmiCreativeGtkDraw, p, NULL);
    gtk_box_append(GTK_BOX(right), p -> canvas);
    GtkGesture * click = gtk_gesture_click_new();
    UmiCreativeGtkTrack(p, G_OBJECT(click));
    g_signal_connect(click, "pressed", G_CALLBACK(UmiCreativeGtkPressed), p);
    gtk_widget_add_controller(p -> canvas, GTK_EVENT_CONTROLLER(click));
    Text(right, "Preview time in milliseconds (scrubbing does not edit the project)");
    p -> timeline = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 8000.0, 1.0);
    Tag(p -> timeline, "creative.timeline");
    gtk_scale_set_digits(GTK_SCALE(p -> timeline), 0);
    UmiCreativeGtkTrack(p, G_OBJECT(p -> timeline));
    g_signal_connect(p -> timeline, "value-changed", G_CALLBACK(UmiCreativeGtkTimelineChanged), p);
    gtk_box_append(GTK_BOX(right), p -> timeline);
    p -> report = gtk_text_view_new();
    Tag(p -> report, "creative.report");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(p -> report), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(p -> report), GTK_WRAP_WORD_CHAR);
    GtkWidget * scroll = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scroll, -1, 160);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), p -> report);
    gtk_box_append(GTK_BOX(right), scroll);
    p -> status = Text(p -> root, "Create or open a project in the Project tab to begin.");
    Tag(p -> status, "creative.status");
    * out = p;
    return UMI_STATUS_OK;
}

GtkWidget * UmiCreativeGtkPanelWidget(UmiCreativeGtkPanel * panel){return panel!= NULL ? panel -> root : NULL;
}

void UmiCreativeGtkPanelDestroy(UmiCreativeGtkPanel * panel)
{
    if (panel == NULL)return;
    /* Disable drawing before releasing its context, even when the root or the
 * drawing area is retained by a window or an automation test. */
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(panel -> canvas), NULL, NULL, NULL);
    for (guint i = 0U;i < panel -> connections -> len; ++ i)
    g_signal_handlers_disconnect_by_data(g_ptr_array_index(panel -> connections, i), panel);
    g_ptr_array_unref(panel -> connections);
    UmiCreativeWorkspaceDestroy(panel -> workspace);
    if (panel -> ownsServer)umi_data_server_destroy(panel -> server);
    g_object_unref(panel -> root);
    g_free(panel -> project);
    g_free(panel);
}

