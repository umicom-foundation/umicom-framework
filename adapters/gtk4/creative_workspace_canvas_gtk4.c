/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_workspace_canvas_gtk4.c
 * PURPOSE: Render copied creative frames and report geometry without mutating the drawing callback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "creative_workspace_private.h"
#include <inttypes.h>
#include <math.h>
#include <pango/pangocairo.h>
#include <string.h>
static double Scale(const UmiCreativeGtkPanel * p, int width, int height)
{
    if (!p -> frameValid || width <= 32 || height <= 32)return 1.0;
    return fmin(((double)width-32.0) / (double)p -> project -> settings.width, ((double)height-32.0) / (double)p -> project -> settings.height);
}

static void Report(UmiCreativeGtkPanel * p)
{
    GString * text = g_string_new(NULL);
    UmiCreativeHistory history = {0};
    (void)UmiCreativeWorkspaceHistory(p -> workspace, & history);
    g_string_append_printf(text, "Project %s · revision %" PRIu64 " · %zu undo / %zu redo\n", p -> project -> id, p -> project -> revision, history.undoCount, history.redoCount);
    g_string_append_printf(text, "%d × %d %s · %u ms · %u BPM\n", (int)p -> project -> settings.width, (int)p -> project -> settings.height,
    UmiCreativeUnitText(p -> project -> settings.unit), (unsigned)p -> project -> settings.durationMs, (unsigned)p -> project -> settings.tempoBpm);
    for (size_t i = 0U;i < p -> project -> elementCount; ++ i){const UmiCreativeElement * e = & p -> project -> elements[i];
        g_string_append_printf(text, "%zu: %s — %s · (%d,%d) %d×%d · %zu keys%s\n", i, e -> id, e -> label, (int)e -> x, (int)e -> y, (int)e -> width, (int)e -> height, e -> keyCount, e -> visible ? "" : " · hidden");
        for (size_t j = 0U;j < e -> keyCount; ++ j)g_string_append_printf(text, "  %u ms: offset (%d,%d), %s\n", (unsigned)e -> keys[j].timeMs, (int)e -> keys[j].offsetX, (int)e -> keys[j].offsetY, e -> keys[j].interpolation == UMI_MEDIA_ANIMATION_LINEAR ? "linear" : "hold");
    }
    for (size_t i = 0U;i < p -> project -> noteCount; ++ i){const UmiCreativeNote * n = & p -> project -> notes[i];
        g_string_append_printf(text, "Note %s: tick %u, length %u, pitch %u, velocity %u\n", n -> id, (unsigned)n -> startTick, (unsigned)n -> durationTicks, (unsigned)n -> pitch, (unsigned)n -> velocity);
    }
    if (p -> frameValid){UmiCreativeOverlap pairs[8];
        size_t count = 0U;
        UmiStatus status = UmiCreativeSceneOverlaps( & p -> frame, pairs, 8U, & count);
        if (status == UMI_STATUS_OK || status == UMI_STATUS_CAPACITY_EXCEEDED){g_string_append_printf(text, "Visible rectangular overlaps at this time: %zu (labels excluded).\n", count);
            for (size_t i = 0U;i < count && i < 8U; ++ i)g_string_append_printf(text, "  %s / %s\n", p -> frame.elements[pairs[i].first].id, p -> frame.elements[pairs[i].second].id);
        }}
    if (p -> playActive)g_string_append_printf(text, "Temporary actor position: %s (%d,%d); saved geometry is unchanged.\n", p -> play.actorId, (int)p -> play.x, (int)p -> play.y);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(p -> report)), text -> str, -1);
    g_string_free(text, TRUE);
}

void UmiCreativeGtkFrameRefresh(UmiCreativeGtkPanel * p)
{
    if (p -> workspace == NULL)return;
    UmiStatus status = UmiCreativeSceneSample(p -> project, (uint32_t)gtk_range_get_value(GTK_RANGE(p -> timeline)), & p -> frame);
    p -> frameValid = status == UMI_STATUS_OK;
    if (p -> frameValid && p -> playActive && p -> play.sourceRevision == p -> project -> revision)
    for (size_t i = 0U;i < p -> frame.elementCount; ++ i)if (strcmp(p -> frame.elements[i].id, p -> play.actorId) == 0){p -> frame.elements[i].x = p -> play.x;
        p -> frame.elements[i].y = p -> play.y;
    }
    Report(p);
    gtk_widget_queue_draw(p -> canvas);
    if (status!= UMI_STATUS_OK)UmiCreativeGtkStatus(p, status, "Preview; saved project remains intact");
}

void UmiCreativeGtkRefresh(UmiCreativeGtkPanel * p)
{
    if (p -> workspace == NULL)return;
    UmiStatus status = UmiCreativeWorkspaceCopy(p -> workspace, p -> project);
    if (status!= UMI_STATUS_OK){p -> frameValid = false;
        gtk_widget_queue_draw(p -> canvas);
        UmiCreativeGtkStatus(p, status, "Read workspace");
        return;
    }
    p -> refreshing = true;
    p -> playActive = false;
    double time = gtk_range_get_value(GTK_RANGE(p -> timeline));
    gtk_range_set_range(GTK_RANGE(p -> timeline), 0.0, (double)p -> project -> settings.durationMs);
    if (time > (double)p -> project -> settings.durationMs)gtk_range_set_value(GTK_RANGE(p -> timeline), (double)p -> project -> settings.durationMs);
    p -> refreshing = false;
    UmiCreativeGtkFrameRefresh(p);
}

void UmiCreativeGtkTimelineChanged(GtkRange * range, gpointer data)
{
    UmiCreativeGtkPanel * p = data;
    (void)range;
    if (!p -> refreshing)UmiCreativeGtkFrameRefresh(p);
}

void UmiCreativeGtkDraw(GtkDrawingArea * area, cairo_t * cr, int width, int height, gpointer data)
{
    UmiCreativeGtkPanel * p = data;
    (void)area;
    cairo_set_source_rgb(cr, 0.91, 0.94, 0.96);
    cairo_paint(cr);
    if (!p -> frameValid)return;
    double scale = Scale(p, width, height);
    cairo_translate(cr, 16.0, 16.0);
    cairo_scale(cr, scale, scale);
    cairo_rectangle(cr, 0, 0, p -> project -> settings.width, p -> project -> settings.height);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_fill(cr);
    cairo_rectangle(cr, 0, 0, p -> project -> settings.width, p -> project -> settings.height);
    cairo_clip(cr);
    for (size_t i = 0U;i < p -> frame.elementCount; ++ i){const UmiCreativeFrameElement * e = & p -> frame.elements[i];
        if (!e -> visible)continue;
        cairo_save(cr);
        cairo_rectangle(cr, e -> x, e -> y, e -> width, e -> height);
        cairo_clip(cr);
        if (e -> kind!= UMI_CREATIVE_TEXT){cairo_set_source_rgb(cr, (double)((e -> colourRgb >> 16U) & 255U) / 255.0, (double)((e -> colourRgb >> 8U) & 255U) / 255.0, (double)(e -> colourRgb & 255U) / 255.0);
            cairo_paint(cr);
        }
        cairo_set_source_rgb(cr, 0.08, 0.15, 0.20);
        /* Pango handles UTF-8 and wrapping; Cairo's toy text API is not used. */
        PangoLayout * layout = pango_cairo_create_layout(cr);
        pango_layout_set_text(layout, e -> label, -1);
        double fontSize = fmin(e -> height / 3.0, 16.0 / scale);
        if (fontSize < 1.0)fontSize = 1.0;
        PangoFontDescription * font = pango_font_description_new();
        pango_font_description_set_family(font, "Sans");
        pango_font_description_set_absolute_size(font, fontSize * PANGO_SCALE);
        pango_layout_set_font_description(layout, font);
        pango_font_description_free(font);
        double usable = e -> width > 8.0 ? e -> width-8.0 : e -> width;
        pango_layout_set_width(layout, (int)fmin(usable * PANGO_SCALE, (double)G_MAXINT));
        pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
        cairo_move_to(cr, e -> x + 4.0, e -> y + 4.0);
        pango_cairo_show_layout(cr, layout);
        g_object_unref(layout);
        cairo_restore(cr);
    }
}

void UmiCreativeGtkPressed(GtkGestureClick * gesture, int presses, double x, double y, gpointer data)
{
    UmiCreativeGtkPanel * p = data;
    (void)gesture;
    (void)presses;
    if (!p -> frameValid)return;
    double scale = Scale(p, gtk_widget_get_width(p -> canvas), gtk_widget_get_height(p -> canvas));
    size_t index;
    if (UmiCreativeSceneHitTest( & p -> frame, (x-16.0) / scale, (y-16.0) / scale, & index) == UMI_STATUS_OK){
        gtk_editable_set_text(GTK_EDITABLE(p -> elementId), p -> frame.elements[index].id);
        UmiCreativeGtkLoadObject(p);
    }
}

