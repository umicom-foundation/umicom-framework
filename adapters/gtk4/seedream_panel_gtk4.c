/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/seedream_panel_gtk4.c
 * PURPOSE: Keep image generation and file export off the GTK owner thread.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media_generation/seedream_gtk4.h"
#include "umicom/security/gtk4/profile_keys.h"
#include "umicom/security/secrets.h"
#include <string.h>

#define PANEL_KEY "umicom-seedream-panel"
typedef struct ImagePanel
{
    char application[96], profile[97];
    GtkWidget *root, *form, *alias, *password, *model, *prompt, *review;
    GtkWidget *approval, *acknowledge, *send, *stop, *picture, *path, *message, *save;
    UmiSeedreamPlan *plan;
    UmiSeedreamResult result;
    UmiCancellationToken *pending;
    bool busy, painting, uncertain;
} ImagePanel;
typedef struct ImageJob
{
    GWeakRef root;
    char application[96], profile[97], alias[97], password[257];
    char *path;
    UmiSeedreamPlan *plan;
    UmiSeedreamResult result;
    UmiMediaPngWriteResult written;
    UmiCancellationToken *cancel;
    UmiStatus status, preview_status;
    GBytes *pixels;
    int width, height;
    bool writing;
} ImageJob;
static ImagePanel *Panel(gpointer root) { return g_object_get_data(G_OBJECT(root), PANEL_KEY); }
static void Tag(GtkWidget *widget, const char *id)
{
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free);
}
static GtkWidget *Label(GtkWidget *box, const char *text)
{
    GtkWidget *widget = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(widget), TRUE);
    gtk_label_set_xalign(GTK_LABEL(widget), 0);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}
static void Message(ImagePanel *panel, const char *text)
{
    gtk_label_set_text(GTK_LABEL(panel->message), text);
}
static GtkWidget *Entry(GtkWidget *box, const char *label, const char *id)
{
    Label(box, label);
    GtkWidget *widget = gtk_entry_new();
    Tag(widget, id);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}
static GtkWidget *Check(GtkWidget *box, const char *label, const char *id)
{
    GtkWidget *widget = gtk_check_button_new_with_label(label);
    Tag(widget, id);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}
static GtkWidget *View(GtkWidget *box, const char *id, bool editable)
{
    GtkWidget *widget = gtk_text_view_new(), *scroll = gtk_scrolled_window_new();
    Tag(widget, id);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(widget), editable);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(widget), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_size_request(scroll, -1, 130);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), widget);
    gtk_box_append(GTK_BOX(box), scroll);
    return widget;
}
static void Display(GtkWidget *widget, const char *value)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget)), value, -1);
}
static UmiStatus Copy(char *out, size_t capacity, const char *value)
{
    if (strlen(value) >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, value, strlen(value) + 1U);
    return UMI_STATUS_OK;
}
/* Approval belongs to immutable fields and a local key alias. Any edit retires
 * both. Contributors adding a request field must connect its change signal. */
static void Invalidate(ImagePanel *panel)
{
    UmiSeedreamDestroy(panel->plan);
    panel->plan = NULL;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
    gtk_widget_set_sensitive(panel->send, FALSE);
    Display(panel->review, "");
}
static void Changed(GObject *unused, gpointer root)
{
    (void)unused;
    ImagePanel *panel = Panel(root);
    if (panel && !panel->painting)
        Invalidate(panel);
}
static void FreeJob(gpointer data)
{
    ImageJob *job = data;
    g_weak_ref_clear(&job->root);
    UmiSeedreamDestroy(job->plan);
    UmiSeedreamResultClear(&job->result);
    umi_cancellation_token_destroy(job->cancel);
    if (job->pixels)
        g_bytes_unref(job->pixels);
    g_free(job->path);
    umi_secret_clear(job, sizeof(*job));
    g_free(job);
}
static void Release(gpointer data)
{
    ImagePanel *panel = data;
    if (panel->pending)
        umi_cancellation_token_request(panel->pending);
    UmiSeedreamDestroy(panel->plan);
    UmiSeedreamResultClear(&panel->result);
    g_free(panel);
}
/* Copy owned RGBA rows on the worker. GBytes carries immutable pixels to GTK,
 * so the paintable cannot borrow a surface that export later moves or releases. */
static UmiStatus Pixels(ImageJob *job)
{
    UmiMediaImageSurfaceSnapshot snapshot;
    UmiStatus status = umi_media_image_surface_snapshot(job->result.image, &snapshot);
    if (status != UMI_STATUS_OK)
        return status;
    if (!snapshot.width || !snapshot.height || snapshot.width > 8192U || snapshot.height > 8192U ||
        snapshot.pixel_count > 16777216U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    unsigned char *pixels = g_try_malloc(snapshot.pixel_count * 4U);
    UmiMediaRgbaPixel *row = g_try_malloc(snapshot.width * sizeof(*row));
    if (!pixels || !row)
    {
        g_free(pixels);
        g_free(row);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t y = 0; y < snapshot.height; ++y)
    {
        if (umi_cancellation_token_is_requested(job->cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        status = umi_media_image_surface_read_row(job->result.image, y, row, snapshot.width);
        if (status != UMI_STATUS_OK)
            break;
        for (size_t x = 0; x < snapshot.width; ++x)
        {
            size_t index = (y * snapshot.width + x) * 4U;
            pixels[index] = row[x].red;
            pixels[index + 1U] = row[x].green;
            pixels[index + 2U] = row[x].blue;
            pixels[index + 3U] = row[x].alpha;
        }
    }
    g_free(row);
    if (status == UMI_STATUS_OK)
    {
        job->pixels = g_bytes_new_take(pixels, snapshot.pixel_count * 4U);
        job->width = (int)snapshot.width;
        job->height = (int)snapshot.height;
    }
    else
        g_free(pixels);
    return status;
}
/* No GTK access occurs on this thread. The profile service is opened, used for
 * this one approved request, and destroyed here; the password is then wiped. */
static void Worker(GTask *task, gpointer source, gpointer data, GCancellable *unused)
{
    (void)source;
    (void)unused;
    ImageJob *job = data;
    if (job->writing)
        job->status = UmiMediaPngWriteNew(job->result.image, job->path, job->cancel, &job->written);
    else
    {
        UmiProfileSecrets *secrets = NULL;
        job->status = umi_cancellation_token_is_requested(job->cancel)
                          ? UMI_STATUS_CANCELLED
                          : UmiProfileSecretsPlatform(job->application, job->profile, &secrets);
        if (job->status == UMI_STATUS_OK)
            job->status = UmiSeedreamExecuteWithProfile(job->plan, true, secrets, job->password, job->alias,
                                                        job->cancel, &job->result);
        UmiProfileSecretsDestroy(secrets);
        umi_secret_clear(job->password, sizeof(job->password));
        if (job->status == UMI_STATUS_OK)
            job->preview_status = Pixels(job);
    }
    g_task_return_boolean(task, TRUE);
}
/* Completion uses a weak root. Closing or retaining an individual button cannot
 * turn a late response into access to a retired panel. Export always returns
 * its borrowed image, including cancellation and create-new failures. */
static void Finished(GObject *source, GAsyncResult *result, gpointer unused)
{
    (void)source;
    (void)unused;
    ImageJob *job = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    GObject *root = g_weak_ref_get(&job->root);
    if (!root)
        return;
    ImagePanel *panel = Panel(root);
    if (panel)
    {
        panel->busy = false;
        panel->pending = NULL;
        gtk_widget_set_sensitive(panel->form, TRUE);
        gtk_widget_set_sensitive(panel->stop, FALSE);
        if (job->writing || job->result.image)
        {
            UmiSeedreamResultClear(&panel->result);
            panel->result = job->result;
            memset(&job->result, 0, sizeof(job->result));
        }
        gtk_widget_set_sensitive(panel->save, panel->result.image != NULL);
        char *message = NULL;
        if (job->writing)
        {
            message =
                g_strdup_printf("PNG export: %s. %s%s", umi_status_text(job->status),
                                job->written.created ? "New file retained at " : "No file was replaced. ",
                                job->written.created ? job->path : "");
        }
        else if (job->status == UMI_STATUS_OK)
        {
            gtk_picture_set_paintable(GTK_PICTURE(panel->picture), NULL);
            if (job->pixels)
            {
                GdkTexture *texture = gdk_memory_texture_new(job->width, job->height, GDK_MEMORY_R8G8B8A8,
                                                             job->pixels, (gsize)job->width * 4U);
                gtk_picture_set_paintable(GTK_PICTURE(panel->picture), GDK_PAINTABLE(texture));
                g_object_unref(texture);
            }
            message =
                g_strdup_printf("Image received from %s. Preview: %s. Save the PNG before leaving this page.",
                                panel->result.model, umi_status_text(job->preview_status));
        }
        else
        {
            panel->uncertain = panel->uncertain || job->result.request_may_have_run;
            gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->acknowledge), FALSE);
            message = g_strdup_printf(
                "%s (HTTP %u). %s", umi_status_text(job->status), job->result.http_status,
                panel->uncertain ? "Generation may have been charged. Check BytePlus usage "
                                   "before preparing another request."
                                 : "Check the local profile, saved key and model entitlement.");
        }
        Message(panel, message);
        g_free(message);
    }
    g_object_unref(root);
}
static void Start(ImagePanel *panel, bool writing)
{
    ImageJob *job = g_new0(ImageJob, 1);
    g_weak_ref_init(&job->root, G_OBJECT(panel->root));
    job->writing = writing;
    strcpy(job->application, panel->application);
    strcpy(job->profile, panel->profile);
    UmiStatus status = umi_cancellation_token_create(&job->cancel);
    if (writing)
        job->path = g_strdup(gtk_editable_get_text(GTK_EDITABLE(panel->path)));
    else
    {
        if (status == UMI_STATUS_OK)
            status = Copy(job->alias, sizeof(job->alias), gtk_editable_get_text(GTK_EDITABLE(panel->alias)));
        if (status == UMI_STATUS_OK)
            status = Copy(job->password, sizeof(job->password),
                          gtk_editable_get_text(GTK_EDITABLE(panel->password)));
        panel->painting = true;
        gtk_editable_set_text(GTK_EDITABLE(panel->password), "");
        panel->painting = false;
    }
    if (status != UMI_STATUS_OK)
    {
        FreeJob(job);
        Invalidate(panel);
        Message(panel, umi_status_text(status));
        return;
    }
    if (writing)
    {
        job->result = panel->result;
        memset(&panel->result, 0, sizeof(panel->result));
    }
    else
    {
        job->plan = panel->plan;
        panel->plan = NULL;
        Invalidate(panel);
    }
    panel->busy = true;
    panel->pending = job->cancel;
    gtk_widget_set_sensitive(panel->form, FALSE);
    gtk_widget_set_sensitive(panel->stop, TRUE);
    Message(panel, writing ? "Saving a new PNG…" : "Generating an image with BytePlus…");
    GTask *task = g_task_new(NULL, NULL, Finished, NULL);
    g_task_set_task_data(task, job, FreeJob);
    g_task_run_in_thread(task, Worker);
    g_object_unref(task);
}
/* Explicit review, send and save are separate actions. Previewing a prompt never
 * sends it; the only network path starts after approval of the displayed body. */
static void Action(GtkButton *button, gpointer root)
{
    ImagePanel *panel = Panel(root);
    if (!panel)
        return;
    const char *action = g_object_get_data(G_OBJECT(button), "seedream-action");
    if (!strcmp(action, "stop"))
    {
        if (panel->pending)
            umi_cancellation_token_request(panel->pending);
        Message(panel, "Stopping local work. A provider request already sent may still be charged.");
        return;
    }
    if (panel->busy)
        return;
    if (!strcmp(action, "keys"))
    {
        Invalidate(panel);
        GtkRoot *owner = gtk_widget_get_root(panel->root);
        UmiStatus status = UmiProfileKeysGtkPresent(GTK_IS_WINDOW(owner) ? GTK_WINDOW(owner) : NULL,
                                                    panel->application, panel->profile);
        if (status != UMI_STATUS_OK)
            Message(panel, umi_status_text(status));
        return;
    }
    if (!strcmp(action, "save"))
    {
        if (!panel->result.image)
        {
            Message(panel, "Generate an image first.");
            return;
        }
        Start(panel, true);
        return;
    }
    if (!strcmp(action, "send"))
    {
        if (!panel->plan || !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approval)))
        {
            Message(panel, "Review and approve this request first.");
            return;
        }
        if (!UmiSeedreamHttpAvailable() || !UmiMediaPngAvailable())
        {
            Message(panel,
                    "This build needs the HTTP and PNG adapters before image generation is available.");
            return;
        }
        Start(panel, false);
        return;
    }
    Invalidate(panel);
    if (panel->uncertain && !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->acknowledge)))
    {
        Message(panel, "Check BytePlus usage for the uncertain request and acknowledge that check first.");
        return;
    }
    UmiSeedreamDraft *draft = g_new0(UmiSeedreamDraft, 1);
    UmiStatus status =
        Copy(draft->model, sizeof(draft->model), gtk_editable_get_text(GTK_EDITABLE(panel->model)));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->prompt));
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
    if (status == UMI_STATUS_OK)
        status = Copy(draft->prompt, sizeof(draft->prompt), text);
    umi_secret_clear(text, strlen(text));
    g_free(text);
    if (status == UMI_STATUS_OK)
        status = UmiSeedreamPrepare(draft, &panel->plan);
    umi_secret_clear(draft, sizeof(*draft));
    g_free(draft);
    if (status != UMI_STATUS_OK)
    {
        Message(panel, umi_status_text(status));
        return;
    }
    const char *url = NULL, *body = NULL;
    status = UmiSeedreamReview(panel->plan, &url, &body);
    if (status != UMI_STATUS_OK)
    {
        Invalidate(panel);
        Message(panel, umi_status_text(status));
        return;
    }
    text = g_strdup_printf(
        "POST %s\nLocal profile: %s / %s\nSaved key alias: %s\n%s\n"
        "One image, provider-selected 2K dimensions, PNG, watermark on. This may incur a provider charge.",
        url, panel->application, panel->profile, gtk_editable_get_text(GTK_EDITABLE(panel->alias)), body);
    Display(panel->review, text);
    g_free(text);
    panel->uncertain = false;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->acknowledge), FALSE);
    gtk_widget_set_sensitive(panel->send, TRUE);
    Message(panel, "Review ready. No network request has been sent.");
}
static GtkWidget *Button(ImagePanel *panel, GtkWidget *box, const char *label, const char *action)
{
    GtkWidget *widget = gtk_button_new_with_label(label);
    char *id = g_strdup_printf("seedream.%s", action);
    Tag(widget, id);
    g_free(id);
    g_object_set_data_full(G_OBJECT(widget), "seedream-action", g_strdup(action), g_free);
    g_signal_connect_object(widget, "clicked", G_CALLBACK(Action), G_OBJECT(panel->root), 0);
    gtk_box_append(GTK_BOX(box), widget);
    return widget;
}
UmiStatus UmiSeedreamGtkCreate(const char *application, const char *profile, GtkWidget **out)
{
    if (!out || *out)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiProfileSecretsScopeValidate(application, profile);
    if (status != UMI_STATUS_OK)
        return status;
    ImagePanel *panel = g_new0(ImagePanel, 1);
    status = Copy(panel->application, sizeof(panel->application), application);
    if (status == UMI_STATUS_OK)
        status = Copy(panel->profile, sizeof(panel->profile), profile);
    if (status != UMI_STATUS_OK)
    {
        g_free(panel);
        return status;
    }
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    Tag(panel->root, "seedream.panel");
    g_object_set_data_full(G_OBJECT(panel->root), PANEL_KEY, panel, Release);
    Label(panel->root,
          "Seedream images · create a single PNG from text. Use a BytePlus ModelArk API key with an enabled "
          "model that supports PNG output. A Lumina subscription alone does not establish API access.");
    panel->form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_append(GTK_BOX(panel->root), panel->form);
    Button(panel, panel->form, "Manage local profile and API keys", "keys");
    panel->alias = Entry(panel->form, "Saved key alias", "seedream.alias");
    gtk_editable_set_text(GTK_EDITABLE(panel->alias), "byteplus-modelark");
    panel->password =
        Entry(panel->form, "Local profile password (cleared after each request)", "seedream.password");
    gtk_entry_set_visibility(GTK_ENTRY(panel->password), FALSE);
    panel->model = Entry(panel->form, "Model or endpoint ID from your ModelArk account", "seedream.model");
    Label(panel->form, "Prompt: describe the subject, setting, composition, lighting and intended use (up to "
                       "8192 UTF-8 bytes).");
    panel->prompt = View(panel->form, "seedream.prompt", true);
    panel->acknowledge = Check(panel->form, "I checked BytePlus usage after the earlier uncertain request",
                               "seedream.acknowledge");
    Button(panel, panel->form, "Review image request", "prepare");
    panel->review = View(panel->form, "seedream.review", false);
    panel->approval =
        Check(panel->form, "I approve this request and any provider charge", "seedream.approval");
    panel->send = Button(panel, panel->form, "Generate reviewed image", "send");
    gtk_widget_set_sensitive(panel->send, FALSE);
    panel->picture = gtk_picture_new();
    gtk_picture_set_can_shrink(GTK_PICTURE(panel->picture), TRUE);
    gtk_widget_set_size_request(panel->picture, -1, 260);
    Tag(panel->picture, "seedream.preview");
    gtk_box_append(GTK_BOX(panel->form), panel->picture);
    panel->path = Entry(panel->form, "Full path for a new PNG file (existing files are never replaced)",
                        "seedream.path");
    panel->save = Button(panel, panel->form, "Save new PNG", "save");
    gtk_widget_set_sensitive(panel->save, FALSE);
    panel->stop = Button(panel, panel->root, "Stop local work", "stop");
    gtk_widget_set_sensitive(panel->stop, FALSE);
    panel->message =
        Label(panel->root, "No request sent. Generated images stay in memory until you save them.");
    Tag(panel->message, "seedream.message");
    GtkWidget *entries[] = {panel->alias, panel->password, panel->model};
    for (size_t index = 0; index < G_N_ELEMENTS(entries); ++index)
        g_signal_connect_object(entries[index], "changed", G_CALLBACK(Changed), G_OBJECT(panel->root), 0);
    g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->prompt)), "changed",
                            G_CALLBACK(Changed), G_OBJECT(panel->root), 0);
    *out = panel->root;
    return UMI_STATUS_OK;
}
