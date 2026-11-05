/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/media_brief_gtk4.c
 *
 * PURPOSE:
 *   Connect explicit source excerpts to existing model and media-generation owners.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/media_brief.h"
#include "umicom/creative_workspace/media_brief.h"
#include "umicom/media_generation/pixverse_gtk4.h"
#include "umicom/media_generation/seedream_gtk4.h"
#include "umicom/provider_connections/gtk4.h"
#include "umicom/ui/gtk4/text_export.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include "umicom/security/secrets.h"
#include <string.h>

typedef struct BriefPanel {
    GtkWidget *root, *title, *source, *direction, *kind, *prompt, *message;
    GtkWidget *replace, *image, *video, *models, *database, *export;
    GWeakRef image_panel, video_panel;
    bool ready, closed, painting, edited;
} BriefPanel;
#define BRIEF_KEY "umicom-media-brief"

static BriefPanel *Panel(gpointer root)
{ return g_object_get_data(G_OBJECT(root), BRIEF_KEY); }
static void Tag(GtkWidget *widget, const char *id)
{ g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free); }
static void Message(BriefPanel *p, const char *message)
{ gtk_label_set_text(GTK_LABEL(p->message), message); }
static GtkWidget *Label(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(box), label);
    return label;
}
static GtkWidget *Text(GtkWidget *box, const char *id, const char *label)
{
    Label(box, label);
    GtkWidget *view = gtk_text_view_new(), *scroll = gtk_scrolled_window_new();
    Tag(view, id);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_size_request(scroll, -1, 120);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_box_append(GTK_BOX(box), scroll);
    gtk_accessible_update_property(GTK_ACCESSIBLE(view), GTK_ACCESSIBLE_PROPERTY_LABEL, label, -1);
    (void)UmiGtk4RecordingSetPrivate(view, 1);
    return view;
}
static GtkWidget *Entry(GtkWidget *box, const char *id, const char *label)
{
    Label(box, label);
    GtkWidget *entry = gtk_entry_new();
    Tag(entry, id);
    gtk_accessible_update_property(GTK_ACCESSIBLE(entry), GTK_ACCESSIBLE_PROPERTY_LABEL, label, -1);
    (void)UmiGtk4RecordingSetPrivate(entry, 1);
    gtk_box_append(GTK_BOX(box), entry);
    return entry;
}
static UmiStatus Copy(const char *text, char *out, size_t capacity)
{
    size_t length = strlen(text);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}
static UmiStatus Capture(GtkWidget *view, char *out, size_t capacity)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    if ((size_t)gtk_text_buffer_get_char_count(buffer) >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
    UmiStatus status = Copy(text, out, capacity);
    umi_secret_clear(text, strlen(text));
    g_free(text);
    return status;
}
static void Sensitivity(BriefPanel *p)
{
    guint kind = gtk_drop_down_get_selected(GTK_DROP_DOWN(p->kind));
    GObject *image = g_weak_ref_get(&p->image_panel), *video = g_weak_ref_get(&p->video_panel);
    gtk_widget_set_sensitive(p->image, p->ready && !p->closed && image != NULL && kind == UMI_MEDIA_BRIEF_IMAGE);
    gtk_widget_set_sensitive(p->video, p->ready && !p->closed && video != NULL && kind == UMI_MEDIA_BRIEF_VIDEO);
    gtk_widget_set_sensitive(p->models, p->ready && !p->closed);
    g_clear_object(&image);
    g_clear_object(&video);
}
static void SourceChanged(GObject *unused, gpointer root)
{
    (void)unused;
    BriefPanel *p = Panel(root);
    if (p == NULL || p->closed || p->painting) return;
    p->ready = false;
    /* Preserve the reviewed draft for comparison, but prevent a stale send. */
    Message(p, "Source or direction changed. You can save the previous brief; prepare it again before sending it to another tool.");
    Sensitivity(p);
}
static void KindChanged(GObject *object, GParamSpec *property, gpointer root)
{ (void)property; SourceChanged(object, root); }
static void PromptChanged(GtkTextBuffer *buffer, gpointer root)
{
    (void)buffer;
    BriefPanel *p = Panel(root);
    if (p == NULL || p->closed || p->painting) return;
    p->edited = true;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(p->replace), FALSE);
}
static void Prepare(GtkButton *button, gpointer root)
{
    (void)button;
    BriefPanel *p = Panel(root);
    if (p == NULL || p->closed) return;
    if (p->edited && !gtk_check_button_get_active(GTK_CHECK_BUTTON(p->replace))) {
        Message(p, "Your brief has edits. Save them or allow replacement before preparing another brief.");
        return;
    }
    UmiMediaBriefDraft draft = {0};
    draft.kind = (UmiMediaBriefKind)gtk_drop_down_get_selected(GTK_DROP_DOWN(p->kind));
    UmiStatus status = Copy(gtk_editable_get_text(GTK_EDITABLE(p->title)), draft.title, sizeof(draft.title));
    if (status == UMI_STATUS_OK) status = Capture(p->source, draft.source, sizeof(draft.source));
    if (status == UMI_STATUS_OK) status = Capture(p->direction, draft.direction, sizeof(draft.direction));
    UmiMediaBrief *brief = NULL;
    if (status == UMI_STATUS_OK) status = UmiMediaBriefCreate(&draft, &brief);
    umi_secret_clear(&draft, sizeof(draft));
    if (status == UMI_STATUS_OK) {
        p->painting = true;
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->prompt)), UmiMediaBriefPrompt(brief), -1);
        p->painting = false;
        p->ready = true;
        p->edited = false;
        gtk_check_button_set_active(GTK_CHECK_BUTTON(p->replace), FALSE);
        Message(p, "Brief prepared locally. Edit it, then choose a model or a generation tool.");
    } else {
        p->ready = false;
        Message(p, status == UMI_STATUS_CAPACITY_EXCEEDED
            ? "Text is too long. Use a shorter excerpt; nothing was truncated."
            : "Enter a title and source excerpt using valid text. The previous brief was kept.");
    }
    UmiMediaBriefDestroy(brief);
    Sensitivity(p);
}
static UmiStatus CurrentPrompt(BriefPanel *p, char *text, size_t capacity, bool require_current_source)
{
    if (p == NULL || p->closed || (require_current_source && !p->ready)) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = Capture(p->prompt, text, capacity);
    if (status != UMI_STATUS_OK) return status;
    bool visible = false;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (c == 127U || (c < 32U && c != '\t' && c != '\r' && c != '\n'))
            return UMI_STATUS_INVALID_ARGUMENT;
        if (c > 32U) visible = true;
    }
    return visible ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
static UmiStatus ExportAdmission(void *data)
{
    GWeakRef *ref = data;
    GtkWidget *root = g_weak_ref_get(ref);
    char text[UMI_MEDIA_BRIEF_PROMPT_CAPACITY];
    /* Saving a draft must remain possible after its source changes. Generation
     * requires a current source; a user-selected text export preserves exactly
     * the displayed draft without claiming it reflects that new source. */
    UmiStatus status = root != NULL ? CurrentPrompt(Panel(root), text, sizeof(text), false) : UMI_STATUS_INVALID_STATE;
    umi_secret_clear(text, sizeof(text));
    g_clear_object(&root);
    return status;
}
static void FreeReference(gpointer data)
{ g_weak_ref_clear(data); g_free(data); }
/* Navigation follows the existing generator widget. No duplicate provider
 * page or second request owner is created for a creation brief. */
static void Reveal(GtkWidget *target)
{
    GtkWidget *page = target;
    for (GtkWidget *parent = gtk_widget_get_parent(page); parent != NULL;
         page = parent, parent = gtk_widget_get_parent(parent)) {
        if (GTK_IS_NOTEBOOK(parent)) {
            int index = gtk_notebook_page_num(GTK_NOTEBOOK(parent), page);
            if (index >= 0) gtk_notebook_set_current_page(GTK_NOTEBOOK(parent), index);
        }
        if (GTK_IS_STACK(parent)) gtk_stack_set_visible_child(GTK_STACK(parent), page);
    }
}
static void Handoff(GtkButton *button, gpointer root)
{
    BriefPanel *p = Panel(root);
    if (p == NULL || p->closed || !gtk_widget_is_sensitive(GTK_WIDGET(button))) return;
    char text[UMI_MEDIA_BRIEF_PROMPT_CAPACITY] = {0};
    UmiStatus status = CurrentPrompt(p, text, sizeof(text), true);
    if (status == UMI_STATUS_OK && GTK_WIDGET(button) == p->models) {
        const char *path = gtk_editable_get_text(GTK_EDITABLE(p->database));
        UmiProviderConnectionsGtkConfig config = {"umicom-media", "default", path};
        GtkRoot *host = gtk_widget_get_root(p->root);
        status = UmiProviderConnectionsGtkPresentAt(GTK_IS_WINDOW(host) ? GTK_WINDOW(host) : NULL, &config, text);
    } else if (status == UMI_STATUS_OK) {
        bool image = GTK_WIDGET(button) == p->image;
        GtkWidget *target = g_weak_ref_get(image ? &p->image_panel : &p->video_panel);
        bool replace = gtk_check_button_get_active(GTK_CHECK_BUTTON(p->replace));
        status = target == NULL ? UMI_STATUS_UNAVAILABLE : image
            ? UmiSeedreamGtkSetPrompt(target, text, replace)
            : UmiPixVerseGtkSetPrompt(target, text, replace);
        if (status == UMI_STATUS_OK) Reveal(target);
        g_clear_object(&target);
    }
    umi_secret_clear(text, sizeof(text));
    gtk_check_button_set_active(GTK_CHECK_BUTTON(p->replace), FALSE);
    if (status == UMI_STATUS_OK) Message(p, "Brief copied. Review and send in the selected tool when ready.");
    else if (status == UMI_STATUS_ALREADY_EXISTS) Message(p, "The generator already has a prompt. Allow replacement to copy this brief, or keep its draft.");
    else if (status == UMI_STATUS_BUSY) Message(p, "The generator is busy. Its request and your brief have been preserved.");
    else if (status == UMI_STATUS_CAPACITY_EXCEEDED) Message(p, "The edited brief exceeds 4096 UTF-8 bytes. Shorten it before continuing.");
    else Message(p, "Unable to continue. Check the brief and the absolute path for model settings; no generation was started.");
}
static GtkWidget *Button(BriefPanel *p, GtkWidget *box, const char *id, const char *label, GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    Tag(button, id);
    g_signal_connect_object(button, "clicked", callback, p->root, 0);
    gtk_box_append(GTK_BOX(box), button);
    return button;
}
void UmiMediaBriefGtkRetire(GtkWidget *root)
{
    if (root == NULL) return;
    BriefPanel *p = Panel(root);
    if (p == NULL || p->closed) return;
    p->closed = true;
    p->ready = false;
    UmiTextExportGtk4Retire(p->export);
    gtk_widget_set_sensitive(root, FALSE);
    Sensitivity(p);
}
static void Release(gpointer data)
{
    BriefPanel *p = data;
    UmiTextExportGtk4Retire(p->export);
    g_clear_object(&p->export);
    g_weak_ref_clear(&p->image_panel);
    g_weak_ref_clear(&p->video_panel);
    g_free(p);
}
GtkWidget *UmiMediaBriefGtkCreate(GtkWidget *image_panel, GtkWidget *video_panel)
{
    if ((image_panel != NULL && !GTK_IS_WIDGET(image_panel)) ||
        (video_panel != NULL && !GTK_IS_WIDGET(video_panel))) return NULL;
    BriefPanel *p = g_new0(BriefPanel, 1);
    p->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    Tag(p->root, "media.brief.panel");
    g_object_set_data_full(G_OBJECT(p->root), BRIEF_KEY, p, Release);
    g_weak_ref_init(&p->image_panel, image_panel);
    g_weak_ref_init(&p->video_panel, video_panel);
    Label(p->root, "Create from text");
    Label(p->root, "Turn a book excerpt, chat or document into a creation brief. Preparing a brief is local; generation happens only after review in a connected tool.");
    GtkWidget *split = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    Tag(split, "media.brief.split");
    gtk_widget_set_vexpand(split, TRUE);
    gtk_paned_set_resize_start_child(GTK_PANED(split), TRUE);
    gtk_paned_set_resize_end_child(GTK_PANED(split), TRUE);
    gtk_paned_set_shrink_start_child(GTK_PANED(split), TRUE);
    gtk_paned_set_shrink_end_child(GTK_PANED(split), TRUE);
    gtk_paned_set_position(GTK_PANED(split), 420);
    gtk_box_append(GTK_BOX(p->root), split);
    GtkWidget *left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8), *right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *ls = gtk_scrolled_window_new(), *rs = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(ls), left);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(rs), right);
    gtk_paned_set_start_child(GTK_PANED(split), ls);
    gtk_paned_set_end_child(GTK_PANED(split), rs);
    p->title = Entry(left, "media.brief.title", "Project title (128 UTF-8 bytes)");
    const char *kinds[] = {"Storyboard", "Image", "Video", "Music brief", NULL};
    Label(left, "What would you like to create?");
    p->kind = gtk_drop_down_new_from_strings(kinds);
    Tag(p->kind, "media.brief.kind");
    gtk_box_append(GTK_BOX(left), p->kind);
    p->source = Text(left, "media.brief.source", "Source excerpt (2048 UTF-8 bytes)");
    p->direction = Text(left, "media.brief.direction", "Creative direction (512 UTF-8 bytes)");
    Button(p, left, "media.brief.prepare", "Prepare brief", G_CALLBACK(Prepare));
    p->prompt = Text(right, "media.brief.prompt", "Creation brief — review and edit before continuing");
    p->replace = gtk_check_button_new_with_label("Allow replacing the edited brief or generator prompt on the next action");
    Tag(p->replace, "media.brief.replace");
    gtk_box_append(GTK_BOX(right), p->replace);
    p->image = Button(p, right, "media.brief.image", "Use in AI Image", G_CALLBACK(Handoff));
    p->video = Button(p, right, "media.brief.video", "Use in AI Video", G_CALLBACK(Handoff));
    p->database = Entry(right, "media.brief.database", "Model settings file — choose an absolute .sqlite3 path");
    gtk_entry_set_placeholder_text(GTK_ENTRY(p->database), "C:\\umicom\\work\\media-connections.sqlite3");
    p->models = Button(p, right, "media.brief.models", "Open model connections with this brief", G_CALLBACK(Handoff));
    Label(right, "Choose a supported local or remote text model to develop a storyboard or music brief. Model output is text; it does not create an audio track automatically.");
    GWeakRef *ref = g_new0(GWeakRef, 1);
    g_weak_ref_init(ref, p->root);
    p->export = UmiTextExportGtk4CreateGuardedBuffer(gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->prompt)),
        "media-creation-brief.txt", ExportAdmission, ref, FreeReference);
    if (p->export != NULL) { g_object_ref_sink(p->export); gtk_box_append(GTK_BOX(right), p->export); }
    else FreeReference(ref);
    p->message = Label(p->root, "Enter a title and source excerpt to begin.");
    Tag(p->message, "media.brief.message");
    g_signal_connect_object(p->title, "changed", G_CALLBACK(SourceChanged), p->root, 0);
    g_signal_connect_object(p->kind, "notify::selected", G_CALLBACK(KindChanged), p->root, 0);
    g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->source)), "changed", G_CALLBACK(SourceChanged), p->root, 0);
    g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->direction)), "changed", G_CALLBACK(SourceChanged), p->root, 0);
    g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->prompt)), "changed", G_CALLBACK(PromptChanged), p->root, 0);
    Sensitivity(p);
    return p->root;
}
