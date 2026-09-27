/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_viewport_gtk4.c
 * Author: Sammy Hegab, Umicom Foundation | Licence: MIT
 * GTK owns widgets; the shared C projection owns geometry and tree validation.
 * Preview edits never rebuild leaf content or reach around its controller.
 *---------------------------------------------------------------------------*/
#include "umicom/workbench_layout/viewport_gtk4.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define NODES UMI_WORKBENCH_LAYOUT_MAX_NODES
#define NONE UMI_WORKBENCH_LAYOUT_INDEX_NONE

typedef struct UmiViewportWidget {
    GtkWidget parent;
    UmiWorkbenchLayoutDocument *document;
    UmiWorkbenchLayoutDocument *measured;
    UmiWorkbenchViewportPlan plan;
    GtkWidget *leaves[NODES];
    GtkWidget *decorations[NODES];
    GtkWidget *tabs[NODES][UMI_WORKBENCH_LAYOUT_MAX_CHILDREN];
    bool disposed;
} UmiViewportWidget;

typedef struct UmiViewportWidgetClass {
    GtkWidgetClass parent;
} UmiViewportWidgetClass;

G_DEFINE_TYPE(UmiViewportWidget, umi_viewport_widget, GTK_TYPE_WIDGET)
static guint changedSignal;

static UmiViewportWidget *View(GtkWidget *widget)
{
    if (!widget || !G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_viewport_widget_get_type())) {
        return NULL;
    }
    return (UmiViewportWidget *)widget;
}

static void Consume(GtkWidget *widget)
{
    if (g_object_is_floating(widget)) g_object_ref_sink(widget);
    g_object_unref(widget);
}

static void TakeChild(GtkWidget *parent, GtkWidget *child)
{
    bool floating = g_object_is_floating(child) != 0;
    gtk_widget_set_parent(child, parent);
    if (!floating) g_object_unref(child);
}

/* Actual widget measurements only change the adapter-local projection copy.
 * The caller's semantic document and its own minimum sizes stay unchanged. */
static UmiStatus Project(UmiViewportWidget *view, int width, int height)
{
    if (view->disposed || !view->document || !view->measured) {
        return UMI_STATUS_INVALID_STATE;
    }
    *view->measured = *view->document;
    UmiWorkbenchViewportOptions options = UmiWorkbenchViewportDefaults();
    for (size_t i = 0; i < view->document->node_count; ++i) {
        if (view->leaves[i]) {
            int minimumWidth = 0, minimumHeight = 0;
            gtk_widget_measure(view->leaves[i], GTK_ORIENTATION_HORIZONTAL, -1,
                &minimumWidth, NULL, NULL, NULL);
            gtk_widget_measure(view->leaves[i], GTK_ORIENTATION_VERTICAL, -1,
                &minimumHeight, NULL, NULL, NULL);
            if (minimumWidth > view->measured->nodes[i].minimum_size.width) {
                view->measured->nodes[i].minimum_size.width = minimumWidth;
            }
            if (minimumHeight > view->measured->nodes[i].minimum_size.height) {
                view->measured->nodes[i].minimum_size.height = minimumHeight;
            }
        }
        if (view->document->nodes[i].kind == UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP &&
            view->decorations[i]) {
            int barHeight = 0;
            /* Measure the strip itself even when its outer scroller is an
             * inactive child. Otherwise a hidden tab strip can lose its native
             * theme minimum just before it becomes visible again. */
            GtkWidget *strip = gtk_scrolled_window_get_child(
                GTK_SCROLLED_WINDOW(view->decorations[i]));
            gtk_widget_measure(strip ? strip : view->decorations[i],
                GTK_ORIENTATION_VERTICAL, -1, &barHeight, NULL, NULL, NULL);
            if (barHeight > options.tabHeight) options.tabHeight = barHeight;
        }
    }
    return UmiWorkbenchViewportBuild(view->measured,
        (UmiWorkbenchLayoutRect){0, 0, width, height}, &options, &view->plan, NULL);
}

static void Measure(GtkWidget *widget, GtkOrientation orientation, int forSize,
    int *minimum, int *natural, int *minBaseline, int *natBaseline)
{
    (void)forSize;
    UmiViewportWidget *view = (UmiViewportWidget *)widget;
    int min = 0;
    if (Project(view, 1000, 600) == UMI_STATUS_OK) {
        min = orientation == GTK_ORIENTATION_HORIZONTAL ?
            view->plan.minimum.width : view->plan.minimum.height;
    }
    if (minimum) *minimum = min;
    if (natural) *natural = MAX(min, orientation == GTK_ORIENTATION_HORIZONTAL ? 1000 : 600);
    if (minBaseline) *minBaseline = -1;
    if (natBaseline) *natBaseline = -1;
}

static void AllocateChild(GtkWidget *child, UmiWorkbenchLayoutRect bounds, bool visible)
{
    if (!child) return;
    gtk_widget_set_child_visible(child, visible && bounds.width > 0 && bounds.height > 0);
    if (!visible || !bounds.width || !bounds.height) return;
    graphene_point_t point = GRAPHENE_POINT_INIT((float)bounds.x, (float)bounds.y);
    /* gtk_widget_allocate takes ownership of the newly created transform. */
    gtk_widget_allocate(child, bounds.width, bounds.height, -1,
        gsk_transform_translate(NULL, &point));
}

static void Allocate(GtkWidget *widget, int width, int height, int baseline)
{
    (void)baseline;
    UmiViewportWidget *view = (UmiViewportWidget *)widget;
    if (Project(view, width, height) != UMI_STATUS_OK) {
        for (GtkWidget *child = gtk_widget_get_first_child(widget); child;
            child = gtk_widget_get_next_sibling(child)) {
            gtk_widget_set_child_visible(child, FALSE);
        }
        return;
    }
    for (size_t i = 0; i < view->plan.nodeCount; ++i) {
        const UmiWorkbenchViewportSlot *slot = &view->plan.slots[i];
        const UmiWorkbenchLayoutNode *node = &view->document->nodes[i];
        AllocateChild(view->leaves[i], slot->bounds, slot->visible);
        AllocateChild(view->decorations[i], slot->decoration, slot->visible);
        for (size_t j = 0; j < node->child_count; ++j) {
            if (view->tabs[i][j]) {
                gtk_widget_set_visible(view->tabs[i][j],
                    view->document->nodes[node->child_indices[j]].visibility !=
                    UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(view->tabs[i][j]),
                    slot->activeChild == j);
            }
        }
    }
}

static void DrawOne(GtkWidget *widget, GtkWidget *child, GtkSnapshot *snapshot,
    UmiWorkbenchLayoutRect bounds)
{
    if (!child || !gtk_widget_get_child_visible(child) || !bounds.width || !bounds.height) return;
    graphene_rect_t clip = GRAPHENE_RECT_INIT((float)bounds.x, (float)bounds.y,
        (float)bounds.width, (float)bounds.height);
    gtk_snapshot_push_clip(snapshot, &clip);
    gtk_widget_snapshot_child(widget, child, snapshot);
    gtk_snapshot_pop(snapshot);
}

static void Snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    UmiViewportWidget *view = (UmiViewportWidget *)widget;
    for (size_t i = 0; i < view->plan.nodeCount; ++i) {
        DrawOne(widget, view->leaves[i], snapshot, view->plan.slots[i].bounds);
        DrawOne(widget, view->decorations[i], snapshot, view->plan.slots[i].decoration);
    }
}

static void Dispose(GObject *object)
{
    UmiViewportWidget *view = (UmiViewportWidget *)object;
    view->disposed = true;
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(GTK_WIDGET(view)))) gtk_widget_unparent(child);
    memset(view->leaves, 0, sizeof view->leaves);
    memset(view->decorations, 0, sizeof view->decorations);
    memset(view->tabs, 0, sizeof view->tabs);
    view->plan.nodeCount = 0;
    G_OBJECT_CLASS(umi_viewport_widget_parent_class)->dispose(object);
}

static void Finalize(GObject *object)
{
    UmiViewportWidget *view = (UmiViewportWidget *)object;
    free(view->document);
    free(view->measured);
    G_OBJECT_CLASS(umi_viewport_widget_parent_class)->finalize(object);
}

static void umi_viewport_widget_class_init(UmiViewportWidgetClass *klass)
{
    GtkWidgetClass *widgetClass = GTK_WIDGET_CLASS(klass);
    GObjectClass *objectClass = G_OBJECT_CLASS(klass);
    widgetClass->measure = Measure;
    widgetClass->size_allocate = Allocate;
    widgetClass->snapshot = Snapshot;
    objectClass->dispose = Dispose;
    objectClass->finalize = Finalize;
    gtk_widget_class_set_css_name(widgetClass, "umicom-workbench-viewport");
    changedSignal = g_signal_new("layout-changed", G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 0);
}

static gboolean Key(GtkEventControllerKey *controller, guint key, guint code,
    GdkModifierType modifiers, gpointer data)
{
    (void)controller;
    (void)code;
    if (key != GDK_KEY_F6) return FALSE;
    UmiViewportWidget *view = data;
    if (view->disposed) return FALSE;
    GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(view));
    GtkWidget *focus = root ? gtk_root_get_focus(root) : NULL;
    size_t current = NONE;
    for (size_t i = 0; i < view->plan.nodeCount; ++i) {
        if (view->leaves[i] && focus && (focus == view->leaves[i] ||
            gtk_widget_is_ancestor(focus, view->leaves[i]))) current = i;
    }
    bool reverse = (modifiers & GDK_SHIFT_MASK) != 0;
    for (size_t k = 0; k < view->plan.focusCount; ++k) {
        current = UmiWorkbenchViewportNextFocus(&view->plan, current, reverse);
        if (current == NONE) break;
        if (gtk_widget_child_focus(view->leaves[current],
            reverse ? GTK_DIR_TAB_BACKWARD : GTK_DIR_TAB_FORWARD)) return TRUE;
    }
    return FALSE;
}

static void umi_viewport_widget_init(UmiViewportWidget *view)
{
    gtk_widget_set_hexpand(GTK_WIDGET(view), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(view), TRUE);
    gtk_widget_set_overflow(GTK_WIDGET(view), GTK_OVERFLOW_HIDDEN);
    GtkEventController *keys = gtk_event_controller_key_new();
    gtk_event_controller_set_propagation_phase(keys, GTK_PHASE_CAPTURE);
    g_signal_connect_object(keys, "key-pressed", G_CALLBACK(Key), view, 0);
    gtk_widget_add_controller(GTK_WIDGET(view), keys);
}

typedef struct TabContext {
    GWeakRef owner;
    size_t node, position;
} TabContext;

static void FreeTab(gpointer data, GClosure *closure)
{
    (void)closure;
    TabContext *context = data;
    g_weak_ref_clear(&context->owner);
    free(context);
}

static void ClickTab(GtkButton *button, gpointer data)
{
    (void)button;
    TabContext *context = data;
    UmiViewportWidget *view = g_weak_ref_get(&context->owner);
    if (view) {
        if (!view->disposed) {
            (void)UmiWorkbenchViewportWidgetSetActive(GTK_WIDGET(view),
                view->document->nodes[context->node].node_id, context->position);
        }
        g_object_unref(view);
    }
}

static UmiStatus FindEditable(GtkWidget *widget, const char *id,
    UmiViewportWidget **outView, size_t *index)
{
    UmiViewportWidget *view = View(widget);
    if (!view || view->disposed || !view->document || !id) return UMI_STATUS_INVALID_ARGUMENT;
    if (view->document->flags & (UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED |
        UMI_WORKBENCH_LAYOUT_DOCUMENT_READ_ONLY)) return UMI_STATUS_PERMISSION_DENIED;
    for (size_t i = 0; i < view->document->node_count; ++i) {
        if (!strcmp(view->document->nodes[i].node_id, id)) {
            if (view->document->nodes[i].flags & UMI_WORKBENCH_LAYOUT_NODE_LOCKED) {
                return UMI_STATUS_PERMISSION_DENIED;
            }
            *outView = view;
            *index = i;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

static void Changed(UmiViewportWidget *view)
{
    gtk_widget_queue_resize(GTK_WIDGET(view));
    g_signal_emit(view, changedSignal, 0);
}

UmiStatus UmiWorkbenchViewportWidgetSetSplit(GtkWidget *widget, const char *id, double ratio)
{
    UmiViewportWidget *view;
    size_t index;
    UmiStatus status = FindEditable(widget, id, &view, &index);
    if (status != UMI_STATUS_OK) return status;
    if (view->document->nodes[index].kind != UMI_WORKBENCH_LAYOUT_NODE_SPLIT ||
        !isfinite(ratio) || ratio < 0.05 || ratio > 0.95) return UMI_STATUS_INVALID_ARGUMENT;
    if (view->document->nodes[index].split_ratio != ratio) {
        view->document->nodes[index].split_ratio = ratio;
        Changed(view);
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiWorkbenchViewportWidgetSetActive(GtkWidget *widget, const char *id, size_t position)
{
    UmiViewportWidget *view;
    size_t index;
    UmiStatus status = FindEditable(widget, id, &view, &index);
    if (status != UMI_STATUS_OK) return status;
    UmiWorkbenchLayoutNode *node = &view->document->nodes[index];
    if (node->kind != UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP || position >= node->child_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (view->document->nodes[node->child_indices[position]].visibility ==
        UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN) return UMI_STATUS_UNAVAILABLE;
    if (node->active_child_index != position) {
        node->active_child_index = position;
        Changed(view);
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiWorkbenchViewportWidgetSetVisible(GtkWidget *widget, const char *id, bool visible)
{
    UmiViewportWidget *view;
    size_t index;
    UmiStatus status = FindEditable(widget, id, &view, &index);
    if (status != UMI_STATUS_OK) return status;
    UmiWorkbenchLayoutVisibility value = visible ? UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE :
        UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;
    if (view->document->nodes[index].visibility != value) {
        view->document->nodes[index].visibility = value;
        Changed(view);
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiWorkbenchViewportWidgetPlan(GtkWidget *widget, UmiWorkbenchViewportPlan *outPlan)
{
    if (!outPlan) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outPlan, 0, sizeof *outPlan);
    UmiViewportWidget *view = View(widget);
    if (!view || view->disposed) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = Project(view, gtk_widget_get_width(widget), gtk_widget_get_height(widget));
    if (status == UMI_STATUS_OK) *outPlan = view->plan;
    return status;
}

UmiStatus UmiWorkbenchViewportWidgetCreate(const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchViewportFactory factory, void *context, GtkWidget **outWidget,
    UmiWorkbenchViewportDiagnostic *diagnostic)
{
    if (!outWidget) return UMI_STATUS_INVALID_ARGUMENT;
    *outWidget = NULL;
    UmiWorkbenchViewportPlan *check = calloc(1, sizeof *check);
    if (!check) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiWorkbenchViewportBuild(document,
        (UmiWorkbenchLayoutRect){0, 0, 1000, 600}, NULL, check, diagnostic);
    free(check);
    if (status != UMI_STATUS_OK) return status;
    if (!factory) return UMI_STATUS_INVALID_ARGUMENT;
    UmiViewportWidget *view = g_object_new(umi_viewport_widget_get_type(), NULL);
    view->document = malloc(sizeof *document);
    view->measured = malloc(sizeof *document);
    if (!view->document || !view->measured) {
        Consume(GTK_WIDGET(view));
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *view->document = *document;
    for (size_t i = 0; i < document->node_count; ++i) {
        const UmiWorkbenchLayoutNode *node = &document->nodes[i];
        if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_PANEL ||
            node->kind == UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP) {
            GtkWidget *content = factory(node, context);
            if (!content) {
                Consume(GTK_WIDGET(view));
                return UMI_STATUS_UNAVAILABLE;
            }
            /* A parented result violates the transfer contract. Release only
             * the transferred reference; never unparent another host's child. */
            if (!GTK_IS_WIDGET(content) || gtk_widget_get_parent(content)) {
                Consume(content);
                Consume(GTK_WIDGET(view));
                return UMI_STATUS_INVALID_STATE;
            }
            GtkWidget *scroll = gtk_scrolled_window_new();
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
            bool floating = g_object_is_floating(content) != 0;
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), content);
            if (!floating) g_object_unref(content);
            gtk_widget_set_name(scroll, node->node_id);
            gtk_widget_set_tooltip_text(scroll, node->title);
            view->leaves[i] = scroll;
            TakeChild(GTK_WIDGET(view), scroll);
        } else if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_SPLIT) {
            GtkWidget *line = gtk_separator_new(
                node->orientation == UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL ?
                GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL);
            view->decorations[i] = line;
            TakeChild(GTK_WIDGET(view), line);
        } else if (node->kind == UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP) {
            GtkWidget *bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
            GtkWidget *scroll = gtk_scrolled_window_new();
            gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), bar);
            view->decorations[i] = scroll;
            TakeChild(GTK_WIDGET(view), scroll);
            GtkToggleButton *group = NULL;
            for (size_t j = 0; j < node->child_count; ++j) {
                GtkWidget *button = gtk_toggle_button_new_with_label(
                    document->nodes[node->child_indices[j]].title);
                TabContext *tab = calloc(1, sizeof *tab);
                if (!tab) {
                    Consume(button);
                    Consume(GTK_WIDGET(view));
                    return UMI_STATUS_OUT_OF_MEMORY;
                }
                g_weak_ref_init(&tab->owner, view);
                tab->node = i;
                tab->position = j;
                g_signal_connect_data(button, "clicked", G_CALLBACK(ClickTab), tab, FreeTab, 0);
                if (group) gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(button), group);
                else group = GTK_TOGGLE_BUTTON(button);
                gtk_box_append(GTK_BOX(bar), button);
                view->tabs[i][j] = button;
            }
        }
    }
    *outWidget = GTK_WIDGET(view);
    return UMI_STATUS_OK;
}
