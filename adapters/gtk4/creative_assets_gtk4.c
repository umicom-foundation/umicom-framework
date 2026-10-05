/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_assets_gtk4.c
 * PURPOSE: Connect captured asset ownership to reusable native controls without running file reads or writes on the GTK thread.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/creative_assets.h"
#include "umicom/creative_workspace/asset_archive.h"
#include "umicom/media/png_image.h"
#include "umicom/media/image_edit.h"
#include <string.h>

typedef struct AssetPanel
{
    UmiCreativeAsset *asset;
    UmiCancellationToken *cancel; /* Borrowed from the active job only. */
    bool busy;
    GtkWidget *preview, *picture, *crop_x, *crop_y, *crop_width, *crop_height, *orientation, *image_save,
        *image_reset;
    bool preview_running;
    GtkWidget *archive_path, *archive_browse, *archive_save, *archive_open;
    GCancellable *chooser; /* Borrowed from the active file selection. */
    GtkWidget *source, *browse, *label, *kind, *destination, *load, *write, *clear, *cancel_button, *details,
        *status;
} AssetPanel;
typedef struct AssetJob
{
    GWeakRef root;
    char *path, *label;
    UmiCreativeAssetKind kind;
    UmiCreativeAsset *asset;
    UmiCancellationToken *cancel;
    UmiCreativeAssetWriteResult written;
    UmiStatus status;
    bool preview;
    bool save_image;
    UmiMediaImageEdit edit;
    GBytes *pixels;
    int width, height;
    bool writing;
    bool archive; /* Save/Open use the same lifetime owner as raw captures. */
} AssetJob;
static AssetPanel *State(GtkWidget *root)
{
    return g_object_get_data(G_OBJECT(root), "umicom-creative-assets");
}
static void Dispose(gpointer data)
{
    AssetPanel *panel = data;
    if (panel->cancel != NULL)
        umi_cancellation_token_request(panel->cancel);
    if (panel->chooser != NULL)
        g_cancellable_cancel(panel->chooser);
    UmiCreativeAssetDestroy(panel->asset);
    g_free(panel);
}
static void Tag(GtkWidget *widget, const char *id)
{
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free);
}
static void Sensitivity(AssetPanel *panel)
{
    bool blocked = panel->busy || panel->chooser != NULL;
    GtkWidget *inputs[] = {
        panel->source, panel->browse,       panel->label,          panel->kind,         panel->destination,
        panel->load,   panel->archive_path, panel->archive_browse, panel->archive_open, panel->crop_x,
        panel->crop_y, panel->crop_width,   panel->crop_height,    panel->orientation,  panel->image_reset};
    for (size_t i = 0U; i < sizeof(inputs) / sizeof(inputs[0]); ++i)
        gtk_widget_set_sensitive(inputs[i], !blocked);
    gtk_widget_set_sensitive(panel->write, !blocked && panel->asset != NULL);
    gtk_widget_set_sensitive(panel->preview, !blocked && panel->asset != NULL && UmiMediaPngAvailable());
    gtk_widget_set_sensitive(panel->image_save, !blocked && panel->asset != NULL && UmiMediaPngAvailable() &&
                                                    UmiMediaPngEncoderAvailable());
    gtk_widget_set_sensitive(panel->archive_save, !blocked && panel->asset != NULL);
    gtk_widget_set_sensitive(panel->clear, !blocked && panel->asset != NULL);
    gtk_widget_set_sensitive(panel->cancel_button, blocked);
}
static const char *Kind(UmiCreativeAssetKind kind)
{
    switch (kind)
    {
    case UMI_CREATIVE_ASSET_DOCUMENT:
        return "Document";
    case UMI_CREATIVE_ASSET_IMAGE:
        return "Image";
    case UMI_CREATIVE_ASSET_VIDEO:
        return "Video";
    case UMI_CREATIVE_ASSET_AUDIO:
        return "Audio";
    case UMI_CREATIVE_ASSET_LYRICS:
        return "Lyrics";
    default:
        return "Binary file";
    }
}
static void Describe(AssetPanel *panel)
{
    UmiCreativeAssetInfo info;
    if (UmiCreativeAssetInspect(panel->asset, &info) != UMI_STATUS_OK)
    {
        gtk_label_set_text(GTK_LABEL(panel->details), "No asset captured.");
        return;
    }
    char *text = g_strdup_printf("%s\nDeclared purpose: %s\nComplete captured size: %zu bytes\nSource: %s\n"
                                 "Content is retained in memory. Preview validates supported PNG pixels; "
                                 "other uses need their own validation.",
                                 info.label, Kind(info.declared_kind), info.byte_count,
                                 info.source_path[0] != '\0' ? info.source_path : "supplied bytes");
    gtk_label_set_text(GTK_LABEL(panel->details), text);
    g_free(text);
}
static void JobFree(gpointer data)
{
    AssetJob *job = data;
    g_weak_ref_clear(&job->root);
    g_free(job->path);
    g_free(job->label);
    g_clear_pointer(&job->pixels, g_bytes_unref);
    UmiCreativeAssetDestroy(job->asset);
    umi_cancellation_token_destroy(job->cancel);
    g_free(job);
}
/* GTK receives packed pixels only after the portable decoder has accepted the
 * complete input. Packing also runs on the worker; source labels/extensions
 * never select an unchecked image loader. */
static UmiStatus PreviewPixels(AssetJob *job)
{
    const void *bytes = NULL;
    size_t size = 0U;
    UmiStatus status = UmiCreativeAssetBytes(job->asset, &bytes, &size);
    UmiMediaImageSurface *image = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiMediaPngDecode(bytes, size, job->cancel, &image);
    if (status != UMI_STATUS_OK)
        return status;
    UmiMediaImageSurfaceSnapshot info;
    status = umi_media_image_surface_snapshot(image, &info);
    UmiMediaImageEdit edit = job->edit;
    /* Zero crop extent in the form means the rest of this captured image. The
     * portable edit API itself always receives exact, nonempty dimensions. */
    if (status == UMI_STATUS_OK && (edit.x >= info.width || edit.y >= info.height))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
    {
        if (edit.width == 0U)
            edit.width = info.width - edit.x;
        if (edit.height == 0U)
            edit.height = info.height - edit.y;
        if (edit.x != 0U || edit.y != 0U || edit.width != info.width || edit.height != info.height ||
            edit.orientation != UMI_MEDIA_IMAGE_ORIGINAL)
        {
            UmiMediaImageSurface *changed = NULL;
            status = UmiMediaImageSurfaceApplyEdit(image, &edit, job->cancel, &changed);
            if (status == UMI_STATUS_OK)
            {
                umi_media_image_surface_destroy(image);
                image = changed;
                status = umi_media_image_surface_snapshot(image, &info);
            }
        }
    }
    unsigned char *pixels = NULL;
    UmiMediaRgbaPixel *row = NULL;
    if (status == UMI_STATUS_OK)
    {
        pixels = g_try_malloc(info.pixel_count * 4U);
        row = g_try_malloc(info.width * sizeof(*row));
        if (pixels == NULL || row == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t y = 0U; status == UMI_STATUS_OK && y < info.height; ++y)
    {
        if (umi_cancellation_token_is_requested(job->cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        status = umi_media_image_surface_read_row(image, y, row, info.width);
        if (status != UMI_STATUS_OK)
            break;
        for (size_t x = 0U; x < info.width; ++x)
        {
            unsigned char *pixel = pixels + (y * info.width + x) * 4U;
            pixel[0] = row[x].red;
            pixel[1] = row[x].green;
            pixel[2] = row[x].blue;
            pixel[3] = row[x].alpha;
        }
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(job->cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
    {
        job->pixels = g_bytes_new_take(pixels, info.pixel_count * 4U);
        pixels = NULL;
        job->width = (int)info.width;
        job->height = (int)info.height;
    }
    if (status == UMI_STATUS_OK && job->save_image)
    {
        UmiMediaPngWriteResult result;
        status = UmiMediaPngWriteNew(image, job->path, job->cancel, &result);
        job->written.created = result.created;
        job->written.file = result.file;
    }
    g_free(pixels);
    g_free(row);
    umi_media_image_surface_destroy(image);
    return status;
}
static void Work(GTask *task, gpointer source, gpointer data, GCancellable *unused)
{
    (void)source;
    (void)unused;
    AssetJob *job = data;
    if (job->preview)
        job->status = PreviewPixels(job);
    else if (job->archive && job->writing)
        job->status = UmiCreativeAssetArchiveSaveNew(job->asset, job->path, job->cancel, &job->written);
    else if (job->archive)
        job->status =
            UmiCreativeAssetArchiveLoad(job->path, UMI_CREATIVE_ASSET_MAX_BYTES, job->cancel, &job->asset);
    else if (job->writing)
        job->status = UmiCreativeAssetWriteNew(job->asset, job->path, job->cancel, &job->written);
    else
        job->status = UmiCreativeAssetLoadFile(job->path, job->label, job->kind, UMI_CREATIVE_ASSET_MAX_BYTES,
                                               job->cancel, &job->asset);
    g_task_return_boolean(task, TRUE);
}
static void Finished(GObject *source, GAsyncResult *result, gpointer unused)
{
    (void)source;
    (void)unused;
    AssetJob *job = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    GtkWidget *root = g_weak_ref_get(&job->root);
    if (root == NULL)
        return;
    AssetPanel *panel = State(root);
    panel->busy = false;
    panel->preview_running = false;
    panel->cancel = NULL;
    /* A cancelled load must not replace the previous capture merely because
     * its blocking read finished just before the owner handled cancellation. */
    if (!job->writing && umi_cancellation_token_is_requested(job->cancel))
        job->status = UMI_STATUS_CANCELLED;
    /* A write or preview job temporarily owns the original capture. Return it even on
     * cancellation or I/O failure; a failed new load retains the old capture. */
    if (job->writing || job->preview || job->status == UMI_STATUS_OK)
    {
        UmiCreativeAssetDestroy(panel->asset);
        panel->asset = job->asset;
        job->asset = NULL;
        Describe(panel);
        if (!job->writing && !job->preview)
            gtk_picture_set_paintable(GTK_PICTURE(panel->picture), NULL);
    }
    if (job->preview && job->status == UMI_STATUS_OK)
    {
        GdkTexture *texture = gdk_memory_texture_new(job->width, job->height, GDK_MEMORY_R8G8B8A8,
                                                     job->pixels, (gsize)job->width * 4U);
        gtk_picture_set_paintable(GTK_PICTURE(panel->picture), GDK_PAINTABLE(texture));
        g_object_unref(texture);
    }
    const char *operation = job->save_image ? "PNG export"
                            : job->preview  ? "PNG preview"
                            : job->archive  ? (job->writing ? "Save archive" : "Open archive")
                                            : (job->writing ? "Copy" : "Capture");
    char *message = NULL;
    if (job->preview && !job->save_image && job->status == UMI_STATUS_OK)
        message = g_strdup_printf("PNG preview ready: %d by %d pixels. Original bytes are unchanged.",
                                  job->width, job->height);
    else if ((job->writing || job->save_image) && job->written.created)
        message = g_strdup_printf("%s: %s. New file retained: %s (%" G_GUINT64_FORMAT " bytes written).",
                                  operation, umi_status_text(job->status), job->written.file.path,
                                  (guint64)job->written.file.bytes_written);
    else
        message = g_strdup_printf("%s: %s.%s", operation, umi_status_text(job->status),
                                  !job->writing && job->status != UMI_STATUS_OK ? " Previous capture remains."
                                                                                : "");
    gtk_label_set_text(GTK_LABEL(panel->status), message);
    g_free(message);
    Sensitivity(panel);
    g_object_unref(root);
}
static void Launch(GtkWidget *root, bool writing, bool archive, bool preview, bool save_image)
{
    AssetPanel *panel = State(root);
    if (panel->busy || panel->chooser != NULL || ((writing || preview) && panel->asset == NULL))
        return;
    AssetJob *job = g_new0(AssetJob, 1);
    g_weak_ref_init(&job->root, G_OBJECT(root));
    job->writing = writing;
    job->preview = preview;
    job->save_image = save_image;
    job->edit.x = (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->crop_x));
    job->edit.y = (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->crop_y));
    job->edit.width = (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->crop_width));
    job->edit.height = (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->crop_height));
    job->edit.orientation =
        (UmiMediaImageOrientation)gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->orientation));
    job->archive = archive;
    job->status = umi_cancellation_token_create(&job->cancel);
    if (job->status != UMI_STATUS_OK)
    {
        gtk_label_set_text(GTK_LABEL(panel->status), "Unable to prepare asset operation.");
        JobFree(job);
        return;
    }
    job->path = g_strdup(gtk_editable_get_text(GTK_EDITABLE(save_image ? panel->destination
                                                            : archive  ? panel->archive_path
                                                            : writing  ? panel->destination
                                                                       : panel->source)));
    job->label = g_strdup(gtk_editable_get_text(GTK_EDITABLE(panel->label)));
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->kind));
    job->kind = selected < 6U ? (UmiCreativeAssetKind)(selected + 1U) : (UmiCreativeAssetKind)0;
    if (writing || preview)
    {
        job->asset = panel->asset;
        panel->asset = NULL;
    }
    panel->busy = true;
    panel->preview_running = preview;
    panel->cancel = job->cancel;
    Sensitivity(panel);
    gtk_label_set_text(GTK_LABEL(panel->status),
                       preview   ? "Decoding captured PNG content..."
                       : archive ? (writing ? "Saving a complete new asset archive..."
                                            : "Checking and opening an asset archive...")
                                 : (writing ? "Writing a new copy of the captured bytes..."
                                            : "Reading a complete local asset..."));
    GTask *task = g_task_new(NULL, NULL, Finished, NULL);
    g_task_set_task_data(task, job, JobFree);
    g_task_run_in_thread(task, Work);
    g_object_unref(task);
}
static void Load(GtkButton *button, gpointer root)
{
    (void)button;
    Launch(GTK_WIDGET(root), false, false, false, false);
}
static void Write(GtkButton *button, gpointer root)
{
    (void)button;
    Launch(GTK_WIDGET(root), true, false, false, false);
}
static void SaveArchive(GtkButton *button, gpointer root)
{
    (void)button;
    Launch(GTK_WIDGET(root), true, true, false, false);
}
static void OpenArchive(GtkButton *button, gpointer root)
{
    (void)button;
    Launch(GTK_WIDGET(root), false, true, false, false);
}
static void Preview(GtkButton *button, gpointer root)
{
    (void)button;
    Launch(GTK_WIDGET(root), false, false, true, false);
}
static void SaveImage(GtkButton *button, gpointer root)
{
    (void)button;
    Launch(GTK_WIDGET(root), false, false, true, true);
}
/* An old preview must not look like the result of a newly edited form. The
 * same rule cancels a worker if a host changes disabled controls programmatically. */
static void EditChanged(AssetPanel *panel)
{
    if (panel->picture != NULL)
        gtk_picture_set_paintable(GTK_PICTURE(panel->picture), NULL);
    if (panel->preview_running && panel->cancel != NULL)
        umi_cancellation_token_request(panel->cancel);
    if (panel->status != NULL)
        gtk_label_set_text(GTK_LABEL(panel->status),
                           panel->preview_running
                               ? "Image edits changed. Cancelling the pending image operation..."
                               : "Image edits changed. Preview or export to apply them.");
}
static void CropChanged(GtkSpinButton *button, gpointer root)
{
    (void)button;
    EditChanged(State(GTK_WIDGET(root)));
}
static void OrientationChanged(GObject *object, GParamSpec *parameter, gpointer root)
{
    (void)object;
    (void)parameter;
    EditChanged(State(GTK_WIDGET(root)));
}
static void ResetImage(GtkButton *button, gpointer root)
{
    (void)button;
    AssetPanel *panel = State(GTK_WIDGET(root));
    if (panel->busy || panel->chooser != NULL)
        return;
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->crop_x), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->crop_y), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->crop_width), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->crop_height), 0.0);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->orientation), 0U);
    EditChanged(panel);
    gtk_label_set_text(GTK_LABEL(panel->status),
                       "Image edits reset. Preview again to inspect the original pixels.");
}
static void Cancel(GtkButton *button, gpointer root)
{
    (void)button;
    AssetPanel *panel = State(GTK_WIDGET(root));
    if (panel->cancel != NULL)
        umi_cancellation_token_request(panel->cancel);
    if (panel->chooser != NULL)
        g_cancellable_cancel(panel->chooser);
}
static void Clear(GtkButton *button, gpointer root)
{
    (void)button;
    AssetPanel *panel = State(GTK_WIDGET(root));
    if (panel->busy || panel->chooser != NULL)
        return;
    UmiCreativeAssetDestroy(panel->asset);
    panel->asset = NULL;
    gtk_picture_set_paintable(GTK_PICTURE(panel->picture), NULL);
    Describe(panel);
    Sensitivity(panel);
    gtk_label_set_text(GTK_LABEL(panel->status),
                       "Capture released. Source and copied files remain unchanged.");
}
typedef struct AssetChoice
{
    GWeakRef root;
    GCancellable *cancel;
    char *previous;
    bool archive;
} AssetChoice;
static void Chosen(GObject *source, GAsyncResult *result, gpointer data)
{
    AssetChoice *choice = data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &error);
    char *path = file != NULL ? g_file_get_path(file) : NULL;
    GtkWidget *root = g_weak_ref_get(&choice->root);
    if (root != NULL)
    {
        AssetPanel *panel = State(root);
        GtkWidget *field = choice->archive ? panel->archive_path : panel->source;
        if (!g_cancellable_is_cancelled(choice->cancel) && path != NULL &&
            UmiOutputFileValidatePath(path) == UMI_STATUS_OK &&
            strcmp(choice->previous, gtk_editable_get_text(GTK_EDITABLE(field))) == 0)
        {
            /* Choosing a file only changes the form. Capture remains a
             * separate action so selection cannot unexpectedly read data. */
            gtk_editable_set_text(GTK_EDITABLE(field), path);
            if (!choice->archive && gtk_editable_get_text(GTK_EDITABLE(panel->label))[0] == '\0')
            {
                char *name = g_filename_display_basename(path);
                gtk_editable_set_text(GTK_EDITABLE(panel->label), name);
                g_free(name);
            }
            gtk_label_set_text(
                GTK_LABEL(panel->status),
                choice->archive
                    ? "Archive selected. Choose Open archive to validate and restore it."
                    : "File selected. Check its name and purpose, then choose Capture local file.");
        }
        else if (file != NULL)
            gtk_label_set_text(
                GTK_LABEL(panel->status),
                "Selection was not applied. Choose a local file again or enter an absolute path.");
        else if (error != NULL && !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) &&
                 !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED) &&
                 !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
            gtk_label_set_text(GTK_LABEL(panel->status),
                               "The file chooser could not complete. You can still enter a path manually.");
        panel->chooser = NULL;
        Sensitivity(panel);
        g_object_unref(root);
    }
    g_free(path);
    g_clear_object(&file);
    g_clear_error(&error);
    g_weak_ref_clear(&choice->root);
    g_object_unref(choice->cancel);
    g_free(choice->previous);
    g_free(choice);
}
static void ChooseFile(GtkWidget *root, bool archive)
{
    AssetPanel *panel = State(root);
    if (panel->busy || panel->chooser != NULL)
        return;
    AssetChoice *choice = g_new0(AssetChoice, 1);
    g_weak_ref_init(&choice->root, G_OBJECT(root));
    choice->archive = archive;
    choice->previous =
        g_strdup(gtk_editable_get_text(GTK_EDITABLE(archive ? panel->archive_path : panel->source)));
    choice->cancel = g_cancellable_new();
    panel->chooser = choice->cancel;
    Sensitivity(panel);
    GtkRoot *parent = gtk_widget_get_root(root);
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog,
                              archive ? "Choose a saved asset archive" : "Choose an asset to capture");
    gtk_file_dialog_set_modal(dialog, TRUE);
    gtk_file_dialog_open(dialog, GTK_IS_WINDOW(parent) ? GTK_WINDOW(parent) : NULL, choice->cancel, Chosen,
                         choice);
    g_object_unref(dialog);
}
static void Choose(GtkButton *button, gpointer root)
{
    (void)button;
    ChooseFile(GTK_WIDGET(root), false);
}
static void ChooseArchive(GtkButton *button, gpointer root)
{
    (void)button;
    ChooseFile(GTK_WIDGET(root), true);
}
static GtkWidget *Text(GtkWidget *root, const char *text)
{
    GtkWidget *widget = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(widget), TRUE);
    gtk_label_set_xalign(GTK_LABEL(widget), 0.0F);
    gtk_box_append(GTK_BOX(root), widget);
    return widget;
}
static GtkWidget *Entry(GtkWidget *root, const char *label, const char *id)
{
    Text(root, label);
    GtkWidget *entry = gtk_entry_new();
    Tag(entry, id);
    gtk_box_append(GTK_BOX(root), entry);
    return entry;
}
static GtkWidget *Button(GtkWidget *root, const char *label, const char *id, GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    Tag(button, id);
    g_signal_connect_object(button, "clicked", callback, root, 0);
    gtk_box_append(GTK_BOX(root), button);
    return button;
}
GtkWidget *UmiCreativeAssetsGtkCreate(void)
{
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    Tag(root, "creative.assets");
    AssetPanel *panel = g_new0(AssetPanel, 1);
    g_object_set_data_full(G_OBJECT(root), "umicom-creative-assets", panel, Dispose);
    Text(root, "Capture a local document, image, video, audio file or lyrics. The complete file stays in "
               "memory until replaced or closed. Save an archive to reopen it later. "
               "Capturing does not save a project attachment, decode media or send anything to a "
               "provider. Maximum: 64 MiB.");
    panel->source = Entry(root, "Absolute source file path", "creative.assets.source");
    panel->browse = Button(root, "Choose source file...", "creative.assets.browse", G_CALLBACK(Choose));
    panel->label = Entry(root, "Name for this capture", "creative.assets.label");
    Text(root, "Declared purpose (the file format is not inferred)");
    const char *kinds[] = {"Document", "Image", "Video", "Audio", "Lyrics", "Binary file", NULL};
    panel->kind = gtk_drop_down_new_from_strings(kinds);
    Tag(panel->kind, "creative.assets.kind");
    gtk_box_append(GTK_BOX(root), panel->kind);
    panel->load = Button(root, "Capture local file", "creative.assets.load", G_CALLBACK(Load));
    panel->details = Text(root, "No asset captured.");
    Tag(panel->details, "creative.assets.details");
    Text(root, "PNG image edits: crop in original pixel coordinates, then rotate or flip. "
               "Zero width or height uses the remaining image. These controls affect preview and PNG export; "
               "the captured bytes and raw copy remain unchanged.");
    const char *crop_labels[] = {"Crop left", "Crop top", "Crop width (0 = remainder)",
                                 "Crop height (0 = remainder)"};
    const char *crop_tags[] = {"creative.assets.crop-x", "creative.assets.crop-y",
                               "creative.assets.crop-width", "creative.assets.crop-height"};
    GtkWidget **crop_fields[] = {&panel->crop_x, &panel->crop_y, &panel->crop_width, &panel->crop_height};
    for (size_t i = 0U; i < 4U; ++i)
    {
        Text(root, crop_labels[i]);
        *crop_fields[i] = gtk_spin_button_new_with_range(0.0, (double)UMI_MEDIA_PNG_MAX_DIMENSION, 1.0);
        Tag(*crop_fields[i], crop_tags[i]);
        gtk_box_append(GTK_BOX(root), *crop_fields[i]);
        g_signal_connect_object(*crop_fields[i], "value-changed", G_CALLBACK(CropChanged), root, 0);
    }
    const char *orientations[] = {
        "Original",          "Clockwise quarter turn", "Half turn", "Counterclockwise quarter turn",
        "Flip horizontally", "Flip vertically",        NULL};
    panel->orientation = gtk_drop_down_new_from_strings(orientations);
    Tag(panel->orientation, "creative.assets.orientation");
    gtk_box_append(GTK_BOX(root), panel->orientation);
    g_signal_connect_object(panel->orientation, "notify::selected", G_CALLBACK(OrientationChanged), root, 0);
    panel->image_reset =
        Button(root, "Reset image edits", "creative.assets.image-reset", G_CALLBACK(ResetImage));
    panel->preview = Button(root, "Preview captured PNG", "creative.assets.preview", G_CALLBACK(Preview));
    Text(root, UmiMediaPngAvailable()
                   ? "PNG preview validates a static, noninterlaced image up to 8192 pixels per side and 16 "
                     "million pixels. "
                     "The view fits the available space. Other formats, animation and colour-profile "
                     "conversion are not supported here."
                   : "PNG preview is unavailable in this build because the optional decoder is not present.");
    panel->picture = gtk_picture_new();
    Tag(panel->picture, "creative.assets.picture");
    gtk_picture_set_content_fit(GTK_PICTURE(panel->picture), GTK_CONTENT_FIT_CONTAIN);
    gtk_widget_set_size_request(panel->picture, 320, 240);
    gtk_box_append(GTK_BOX(root), panel->picture);
    panel->destination = Entry(root, "New absolute destination file path", "creative.assets.destination");
    if (!UmiMediaPngEncoderAvailable())
        Text(root, "PNG export is unavailable in this build because the optional encoder is not present.");
    panel->image_save =
        Button(root, "Export edited PNG to new file", "creative.assets.image-save", G_CALLBACK(SaveImage));
    panel->write =
        Button(root, "Write captured bytes to new file", "creative.assets.write", G_CALLBACK(Write));
    Text(root, "An asset archive retains its name, declared purpose and complete bytes. It is separate from "
               "the scene project. Save needs a new file name; Open validates a complete archive before "
               "replacing the current capture. Archives are plaintext and omit the original source path.");
    panel->archive_path = Entry(root, "Absolute asset archive path (for example, capture.umiasset)",
                                "creative.assets.archive-path");
    panel->archive_browse =
        Button(root, "Choose saved archive...", "creative.assets.archive-browse", G_CALLBACK(ChooseArchive));
    panel->archive_save =
        Button(root, "Save new archive", "creative.assets.archive-save", G_CALLBACK(SaveArchive));
    panel->archive_open =
        Button(root, "Open archive", "creative.assets.archive-open", G_CALLBACK(OpenArchive));
    panel->clear = Button(root, "Release capture", "creative.assets.clear", G_CALLBACK(Clear));
    panel->cancel_button =
        Button(root, "Cancel pending operation", "creative.assets.cancel", G_CALLBACK(Cancel));
    panel->status = Text(root, "Nothing is read automatically. Capture only the file you select.");
    Tag(panel->status, "creative.assets.status");
    Sensitivity(panel);
    return root;
}
UmiStatus UmiCreativeAssetsGtkCapture(GtkWidget *root, const char *label, UmiCreativeAssetKind kind,
                                      const void *bytes, size_t size)
{
    if (!GTK_IS_WIDGET(root) || State(root) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    AssetPanel *panel = State(root);
    if (panel->busy || panel->chooser != NULL)
        return UMI_STATUS_BUSY;
    UmiCreativeAsset *asset = NULL;
    UmiStatus status = UmiCreativeAssetCapture(label, kind, bytes, size, NULL, &asset);
    if (status == UMI_STATUS_OK)
    {
        UmiCreativeAssetDestroy(panel->asset);
        panel->asset = asset;
        gtk_picture_set_paintable(GTK_PICTURE(panel->picture), NULL);
        Describe(panel);
        Sensitivity(panel);
        gtk_label_set_text(GTK_LABEL(panel->status), "Complete bytes captured in memory.");
    }
    return status;
}
UmiStatus UmiCreativeAssetsGtkInspect(GtkWidget *root, UmiCreativeAssetInfo *out)
{
    if (!GTK_IS_WIDGET(root) || State(root) == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    AssetPanel *panel = State(root);
    if (panel->busy || panel->chooser != NULL)
        return UMI_STATUS_BUSY;
    return panel->asset != NULL ? UmiCreativeAssetInspect(panel->asset, out) : UMI_STATUS_NOT_FOUND;
}

/* Shared libraries prepare copies on workers, then transfer ownership here.
 * This bridge keeps the existing PNG editor usable without blocking GTK to
 * duplicate a large captured asset or coupling the editor to library storage. */
UmiStatus UmiCreativeAssetsGtkAdopt(GtkWidget *root, UmiCreativeAsset **asset)
{
    if (!GTK_IS_WIDGET(root) || State(root)==NULL || asset==NULL || *asset==NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    AssetPanel *panel=State(root);
    if (panel->busy || panel->chooser!=NULL) return UMI_STATUS_BUSY;
    UmiCreativeAssetDestroy(panel->asset);
    panel->asset=*asset;
    *asset=NULL;
    gtk_picture_set_paintable(GTK_PICTURE(panel->picture),NULL);
    ResetImage(NULL,root);
    Describe(panel);
    Sensitivity(panel);
    gtk_label_set_text(GTK_LABEL(panel->status),"Library asset copied into this editor. Edits and exports leave the library capture unchanged.");
    return UMI_STATUS_OK;
}
