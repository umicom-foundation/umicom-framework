/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_audio_gtk4.c
 * PURPOSE: Share sample-range editing without placing audio logic in products.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/creative_audio.h"
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct AudioPanel {
    UmiCreativeAudioClip *clip;
    UmiCreativeExport preview;
    UmiCreativeAudioInfo info;
    GtkWidget *source, *destination, *begin, *end, *gain, *fadeIn, *fadeOut;
    GtkWidget *drawing, *status, *description, *write;
} AudioPanel;
static void Tag(GtkWidget *widget, const char *id)
{ g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free); }
static AudioPanel *State(GtkWidget *root)
{ return g_object_get_data(G_OBJECT(root), "umicom-creative-audio"); }
static void Dispose(gpointer data)
{
    AudioPanel *p = data;
    UmiCreativeAudioDestroy(p->clip);
    UmiCreativeExportFree(&p->preview);
    g_free(p);
}
static void Status(AudioPanel *p, const char *operation, UmiStatus status)
{
    char *text = g_strdup_printf("%s: %s", operation, umi_status_text(status));
    gtk_label_set_text(GTK_LABEL(p->status), text); g_free(text);
}
static void Draw(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data)
{
    const UmiCreativeAudioOverview *o = data;
    if (o == NULL || width <= 32 || height <= 24 || o->binCount == 0U) return;
    GdkRGBA colour;
    gtk_widget_get_color(GTK_WIDGET(area), &colour);
    gdk_cairo_set_source_rgba(cr, &colour);
    double lane = (double)height / o->source.channels;
    for (unsigned c = 0U; c < o->source.channels; ++c) {
        double centre = lane * ((double)c + 0.5), amplitude = lane * 0.40;
        cairo_set_line_width(cr, 0.5);
        cairo_move_to(cr, 16.0, centre); cairo_line_to(cr, width - 16.0, centre); cairo_stroke(cr);
        cairo_set_line_width(cr, 1.0);
        for (size_t i = 0U; i < o->binCount; ++i) {
            double x = 16.0 + ((double)i + 0.5) * (width - 32.0) / (double)o->binCount;
            double top = centre - amplitude * o->bins[i].maximum[c] / 32768.0;
            double bottom = centre - amplitude * o->bins[i].minimum[c] / 32768.0;
            cairo_move_to(cr, x, top); cairo_line_to(cr, x, bottom); cairo_stroke(cr);
        }
    }
}
static void Invalidate(GtkWidget *changed, gpointer root)
{
    (void)changed;
    AudioPanel *p = State(GTK_WIDGET(root));
    UmiCreativeExportFree(&p->preview);
    gtk_widget_set_sensitive(p->write, FALSE);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(p->drawing), NULL, NULL, NULL);
    gtk_label_set_text(GTK_LABEL(p->status), "Selection changed. Preview the range before exporting.");
}
static void Adopt(AudioPanel *p, UmiCreativeAudioClip *clip, const char *label)
{
    UmiCreativeAudioDestroy(p->clip); p->clip = clip;
    (void)UmiCreativeAudioGetInfo(clip, &p->info);
    UmiCreativeExportFree(&p->preview);
    gtk_spin_button_set_range(GTK_SPIN_BUTTON(p->begin), 0.0, (double)p->info.frames);
    gtk_spin_button_set_range(GTK_SPIN_BUTTON(p->end), 0.0, (double)p->info.frames);
    gtk_spin_button_set_range(GTK_SPIN_BUTTON(p->fadeIn), 0.0, (double)p->info.frames);
    gtk_spin_button_set_range(GTK_SPIN_BUTTON(p->fadeOut), 0.0, (double)p->info.frames);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(p->begin), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(p->end), (double)p->info.frames);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(p->fadeIn), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(p->fadeOut), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(p->gain), 1000.0);
    gtk_widget_set_sensitive(p->write, FALSE);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(p->drawing), NULL, NULL, NULL);
    char *description = g_strdup_printf("Loaded: %s\n%u Hz, %u channel(s), %" PRIu64 " frames (%.6f seconds).\n"
        "%zu optional metadata chunks will not be copied on export.", label, p->info.sampleRate,
        (unsigned)p->info.channels, p->info.frames, (double)p->info.frames / p->info.sampleRate, p->info.ignoredChunks);
    gtk_label_set_text(GTK_LABEL(p->description), description); g_free(description);
    gtk_label_set_text(GTK_LABEL(p->status), "Clip copied into memory. Select a range and choose Preview range.");
}
UmiStatus UmiCreativeAudioGtkLoadBytes(GtkWidget *root, const void *bytes, size_t length)
{
    if (!GTK_IS_WIDGET(root) || State(root) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeAudioClip *clip = NULL;
    UmiStatus status = UmiCreativeAudioDecode(bytes, length, &clip);
    if (status == UMI_STATUS_OK) Adopt(State(root), clip, "supplied PCM-WAVE bytes");
    return status;
}
static void Load(GtkButton *button, gpointer root)
{
    (void)button;
    AudioPanel *p = State(GTK_WIDGET(root));
    UmiCreativeAudioClip *clip = NULL;
    const char *path = gtk_editable_get_text(GTK_EDITABLE(p->source));
    UmiStatus status = UmiCreativeAudioLoadFile(path, &clip);
    if (status == UMI_STATUS_OK) Adopt(p, clip, path);
    else Status(p, "Load failed; previous clip remains", status);
}
static void Preview(GtkButton *button, gpointer root)
{
    (void)button;
    AudioPanel *p = State(GTK_WIDGET(root));
    UmiCreativeExport rendered = {0};
    UmiCreativeAudioClip *clip = NULL;
    UmiCreativeAudioOverview *peaks = g_new0(UmiCreativeAudioOverview, 1);
    GtkWidget *fields[] = {p->begin, p->end, p->gain, p->fadeIn, p->fadeOut};
    for (size_t i = 0U; i < 5U; ++i) {
        double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(fields[i]));
        if (!isfinite(value) || value < 0.0 || value > (i == 2U ? 1000.0 : 2097152.0) || floor(value) != value) {
            g_free(peaks); Invalidate(NULL, root);
            Status(p, "Whole, finite frame counts are required", UMI_STATUS_INVALID_ARGUMENT);
            return;
        }
    }
    UmiCreativeAudioEdit edit = {
        (uint64_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(p->begin)),
        (uint64_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(p->end)),
        (unsigned)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(p->gain)),
        (uint64_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(p->fadeIn)),
        (uint64_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(p->fadeOut))};
    UmiStatus status = UmiCreativeAudioRender(p->clip, &edit, &rendered);
    if (status == UMI_STATUS_OK) status = UmiCreativeAudioDecode(rendered.bytes, rendered.size, &clip);
    if (status == UMI_STATUS_OK) status = UmiCreativeAudioInspect(clip, 0U,
        edit.endFrame - edit.beginFrame, UMI_CREATIVE_AUDIO_MAX_BINS, peaks);
    if (status == UMI_STATUS_OK) {
        UmiCreativeExportFree(&p->preview); p->preview = rendered; rendered = (UmiCreativeExport){0};
        /* The drawing keeps only an owned value, never a pointer into this panel.
         * Retaining the drawing widget after closing the panel is therefore safe. */
        gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(p->drawing), Draw, peaks, g_free);
        peaks = NULL;
        gtk_widget_set_sensitive(p->write, TRUE);
        char *text = g_strdup_printf("Preview ready: %" PRIu64 " frames, %.6f seconds; %zu bytes. "
            "Source unchanged. This is a waveform preview, not audio playback.",
            edit.endFrame - edit.beginFrame, (double)(edit.endFrame - edit.beginFrame) / p->info.sampleRate, p->preview.size);
        gtk_label_set_text(GTK_LABEL(p->status), text); g_free(text);
    } else {
        UmiCreativeExportFree(&p->preview); gtk_widget_set_sensitive(p->write, FALSE);
        gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(p->drawing), NULL, NULL, NULL);
        Status(p, "Preview unavailable", status);
    }
    g_free(peaks); UmiCreativeAudioDestroy(clip); UmiCreativeExportFree(&rendered);
}
static void Write(GtkButton *button, gpointer root)
{
    (void)button;
    AudioPanel *p = State(GTK_WIDGET(root));
    UmiStatus status = UmiCreativeExportWriteNew(&p->preview,
        gtk_editable_get_text(GTK_EDITABLE(p->destination)));
    if (status == UMI_STATUS_OK) gtk_label_set_text(GTK_LABEL(p->status),
        "New WAVE written. Play it in your chosen audio player at a low volume. Source unchanged.");
    else Status(p, "Export failed; no overwrite or deletion; inspect any partial new file", status);
}
static GtkWidget *Text(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text); gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f); gtk_box_append(GTK_BOX(box), label); return label;
}
static GtkWidget *Entry(GtkWidget *box, const char *label, const char *id)
{
    Text(box, label); GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entry), 4000); Tag(entry, id);
    gtk_box_append(GTK_BOX(box), entry); return entry;
}
static GtkWidget *Number(GtkWidget *root, const char *label, const char *id, double max, double value)
{
    Text(root, label); GtkWidget *spin = gtk_spin_button_new_with_range(0.0, max, 1.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), value); Tag(spin, id);
    gtk_box_append(GTK_BOX(root), spin);
    g_signal_connect_object(spin, "value-changed", G_CALLBACK(Invalidate), root, 0);
    return spin;
}
static GtkWidget *Button(GtkWidget *root, const char *label, const char *id, GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(label); Tag(button, id);
    gtk_box_append(GTK_BOX(root), button);
    /* Automatic disconnection follows the root's lifetime, not a raw struct. */
    g_signal_connect_object(button, "clicked", callback, root, 0);
    return button;
}
GtkWidget *UmiCreativeAudioGtkCreate(void)
{
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    Tag(root, "creative.audio");
    AudioPanel *p = g_new0(AudioPanel, 1);
    g_object_set_data_full(G_OBJECT(root), "umicom-creative-audio", p, Dispose);
    Text(root, "Audio clip: PCM16 mono/stereo WAVE, up to 4 MiB. Imports and edits stay in memory; "
        "this tab does not save musical notes or an audio attachment to the project. Closing it discards the clip.");
    p->source = Entry(root, "Absolute source WAVE path", "creative.audio.source");
    Button(root, "Load WAVE", "creative.audio.load", G_CALLBACK(Load));
    p->description = Text(root, "No audio file has been opened.");
    /* Construct status and output controls before connecting editable numbers. */
    p->status = gtk_label_new("Choose a file. Nothing is read automatically.");
    gtk_label_set_wrap(GTK_LABEL(p->status), TRUE); gtk_label_set_xalign(GTK_LABEL(p->status), 0.0f);
    p->drawing = gtk_drawing_area_new(); Tag(p->drawing, "creative.audio.waveform");
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(p->drawing), 480);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(p->drawing), 200);
    p->write = gtk_button_new_with_label("Write new WAVE"); Tag(p->write, "creative.audio.write");
    gtk_widget_set_sensitive(p->write, FALSE);
    g_signal_connect_object(p->write, "clicked", G_CALLBACK(Write), root, 0);
    p->begin = Number(root, "First frame (included)", "creative.audio.begin", 2097152.0, 0.0);
    p->end = Number(root, "End frame (excluded)", "creative.audio.end", 2097152.0, 0.0);
    p->gain = Number(root, "Gain in thousandths (1000 = unchanged; 700 = 70%)", "creative.audio.gain", 1000.0, 1000.0);
    p->fadeIn = Number(root, "Fade-in length in frames", "creative.audio.fade_in", 2097152.0, 0.0);
    p->fadeOut = Number(root, "Fade-out length in frames", "creative.audio.fade_out", 2097152.0, 0.0);
    Button(root, "Preview range", "creative.audio.preview", G_CALLBACK(Preview));
    gtk_box_append(GTK_BOX(root), p->drawing);
    p->destination = Entry(root, "New absolute destination WAVE path", "creative.audio.destination");
    gtk_box_append(GTK_BOX(root), p->write); gtk_box_append(GTK_BOX(root), p->status);
    return root;
}
