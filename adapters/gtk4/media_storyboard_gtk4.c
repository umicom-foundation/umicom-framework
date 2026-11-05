/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/media_storyboard_gtk4.c
 *
 * PURPOSE:
 *   Author tagged scenes, review local instructions and explicitly hand off one scene.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/media_storyboard.h"
#include "umicom/ui/gtk4/media_brief.h"
#include "umicom/ui/gtk4/creative_library.h"
#include "umicom/ui/gtk4/text_export.h"
#include "umicom/creative_workspace/media_storyboard.h"
#include "umicom/security/secrets.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include <string.h>

typedef struct StoryPanel {
    GtkWidget *root, *selection, *title, *duration, *story, *assets, *output, *message;
    GtkWidget *replace, *handoff, *export, *asset_choice;
    char asset_ids[UMI_CREATIVE_LIBRARY_MAX_ASSETS][UMI_CREATIVE_ID_CAPACITY];
    size_t asset_count;
    GWeakRef library, brief;
    UmiMediaStoryboardDraft draft;
    UmiCreativeExport reviewed;
    size_t selected;
    bool closed, painting, dirty, ready;
} StoryPanel;
#define STORY_KEY "umicom-media-storyboard"
static StoryPanel *State(GtkWidget *root) { return g_object_get_data(G_OBJECT(root), STORY_KEY); }
static void Tag(GtkWidget *widget, const char *id)
{ g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free); }
static GtkWidget *Label(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE); gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(box), label); return label;
}
static void Message(StoryPanel *p, const char *text) { gtk_label_set_text(GTK_LABEL(p->message), text); }
static void Invalidate(StoryPanel *p)
{ p->ready = false; gtk_widget_set_sensitive(p->handoff, FALSE); }
static void Reveal(GtkWidget *target)
{
    for (GtkWidget *page = target, *parent = gtk_widget_get_parent(page); parent != NULL;
         page = parent, parent = gtk_widget_get_parent(parent)) {
        if (GTK_IS_STACK(parent)) gtk_stack_set_visible_child(GTK_STACK(parent), page);
        if (GTK_IS_NOTEBOOK(parent)) {
            int index = gtk_notebook_page_num(GTK_NOTEBOOK(parent), page);
            if (index >= 0) gtk_notebook_set_current_page(GTK_NOTEBOOK(parent), index);
        }
    }
}
static void Changed(GObject *object, gpointer root)
{
    (void)object; StoryPanel *p = State(root);
    if (p == NULL || p->closed || p->painting) return;
    p->dirty = true; Invalidate(p);
    Message(p, "Scene edits are pending. Apply changes or add as a new scene; discard reloads the selected scene.");
}
static void DurationChanged(GtkSpinButton *spin, gpointer root) { Changed(G_OBJECT(spin), root); }
static UmiStatus Capture(StoryPanel *p, UmiMediaStoryboardScene *scene)
{
    const char *title = gtk_editable_get_text(GTK_EDITABLE(p->title));
    if (strlen(title) >= sizeof(scene->title)) return UMI_STATUS_CAPACITY_EXCEEDED;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->story));
    /* Reject before allocating arbitrary pasted text. UTF-8 bytes are checked
     * separately because a character may occupy more than one byte. */
    if (gtk_text_buffer_get_char_count(buffer) >= (int)sizeof(scene->story)) return UMI_STATUS_CAPACITY_EXCEEDED;
    GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
    size_t length = strlen(text);
    UmiStatus status = length >= sizeof(scene->story) ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK) {
        memcpy(scene->title, title, strlen(title) + 1U); memcpy(scene->story, text, length + 1U);
        scene->duration_seconds = (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(p->duration));
    }
    umi_secret_clear(text, length); g_free(text); return status;
}
static void Load(StoryPanel *p)
{
    const UmiMediaStoryboardScene *scene = p->selected < p->draft.count ? &p->draft.scenes[p->selected] : NULL;
    p->painting = true;
    gtk_editable_set_text(GTK_EDITABLE(p->title), scene != NULL ? scene->title : "");
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->story)), scene != NULL ? scene->story : "", -1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(p->duration), scene != NULL ? (double)scene->duration_seconds : 10.0);
    p->painting = false; p->dirty = false;
}
static void RefreshScenes(StoryPanel *p)
{
    GtkStringList *list = gtk_string_list_new(NULL);
    for (size_t i = 0U; i < p->draft.count; ++i) {
        char *label = g_strdup_printf("%zu. %s (%u s)", i + 1U, p->draft.scenes[i].title,
            (unsigned)p->draft.scenes[i].duration_seconds);
        gtk_string_list_append(list, label); g_free(label);
    }
    p->painting = true;
    gtk_drop_down_set_model(GTK_DROP_DOWN(p->selection), G_LIST_MODEL(list));
    gtk_drop_down_set_selected(GTK_DROP_DOWN(p->selection), p->selected < p->draft.count ? (guint)p->selected : GTK_INVALID_LIST_POSITION);
    p->painting = false; g_object_unref(list); Load(p);
}
static void Selected(GObject *object, GParamSpec *property, gpointer root)
{
    (void)object; (void)property; StoryPanel *p = State(root);
    if (p == NULL || p->closed || p->painting) return;
    if (p->dirty) {
        p->painting = true;
        gtk_drop_down_set_selected(GTK_DROP_DOWN(p->selection), p->selected < p->draft.count ? (guint)p->selected : GTK_INVALID_LIST_POSITION);
        p->painting = false; Message(p, "Apply or discard the pending scene edits before changing scenes."); return;
    }
    p->selected = (size_t)gtk_drop_down_get_selected(GTK_DROP_DOWN(p->selection)); Load(p);
}
static void Edit(GtkButton *button, gpointer root)
{
    StoryPanel *p = State(root); if (p == NULL || p->closed) return;
    const char *id = g_object_get_data(G_OBJECT(button), "umicom-automation-id");
    if (strcmp(id, "media.storyboard.discard") == 0) { Load(p); Message(p, "Selected scene reloaded; applied scenes are unchanged."); return; }
    if (strcmp(id, "media.storyboard.add") == 0 || strcmp(id, "media.storyboard.apply") == 0) {
        UmiMediaStoryboardScene scene = {0}; UmiStatus status = Capture(p, &scene);
        bool add = strcmp(id, "media.storyboard.add") == 0;
        if (status == UMI_STATUS_OK) status = add ? UmiMediaStoryboardAppend(&p->draft, &scene)
            : UmiMediaStoryboardUpdate(&p->draft, p->selected, &scene);
        umi_secret_clear(&scene, sizeof(scene));
        if (status != UMI_STATUS_OK) {
            Message(p, "Enter a title and story within 128 and 1024 UTF-8 bytes. Up to 16 scenes; select an existing scene to apply changes. Nothing was truncated."); return;
        }
        if (add) p->selected = p->draft.count - 1U;
    } else {
        if (p->dirty) { Message(p, "Apply or discard pending edits before moving or removing a scene."); return; }
        if (p->selected >= p->draft.count) return;
        if (strcmp(id, "media.storyboard.remove") == 0) {
            (void)UmiMediaStoryboardRemove(&p->draft, p->selected);
            if (p->selected >= p->draft.count) p->selected = p->draft.count > 0U ? p->draft.count - 1U : SIZE_MAX;
        } else {
            bool up = strcmp(id, "media.storyboard.up") == 0;
            if ((up && p->selected == 0U) || (!up && p->selected + 1U >= p->draft.count)) return;
            size_t next = up ? p->selected - 1U : p->selected + 1U;
            (void)UmiMediaStoryboardMove(&p->draft, p->selected, next); p->selected = next;
        }
    }
    Invalidate(p); RefreshScenes(p);
    Message(p, "Storyboard draft updated. Prepare instructions to check the scene tags against the asset library.");
}
static UmiStatus Inventory(StoryPanel *p, UmiCreativeAssetLibraryEntry *entries, size_t *count)
{
    GtkWidget *library = g_weak_ref_get(&p->library);
    *count = 0U;
    if (library == NULL) return UMI_STATUS_OK; /* Text-only stories remain useful. */
    UmiCreativeAssetLibraryInfo info = {0};
    UmiStatus status = UmiCreativeAssetLibraryGtkInspect(library, &info);
    if (status == UMI_STATUS_OK && info.asset_count > UMI_CREATIVE_LIBRARY_MAX_ASSETS) status = UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < info.asset_count; ++i)
        status = UmiCreativeAssetLibraryGtkAt(library, i, &entries[i]);
    if (status == UMI_STATUS_OK) *count = info.asset_count;
    g_object_unref(library); return status;
}
static UmiStatus Plan(StoryPanel *p, UmiMediaStoryboard **plan)
{
    /* Metadata snapshots avoid borrowing a library while its worker owns it.
     * Asset payloads and local source paths never enter the model prompt. */
    UmiCreativeAssetLibraryEntry *entries = g_new0(UmiCreativeAssetLibraryEntry, UMI_CREATIVE_LIBRARY_MAX_ASSETS);
    size_t count = 0U; UmiStatus status = Inventory(p, entries, &count);
    if (status == UMI_STATUS_OK) status = UmiMediaStoryboardCreate(&p->draft, entries, count, plan);
    umi_secret_clear(entries, sizeof(*entries) * UMI_CREATIVE_LIBRARY_MAX_ASSETS); g_free(entries); return status;
}
static void Assets(GtkButton *button, gpointer root)
{
    StoryPanel *p = State(root); if (p == NULL || p->closed) return;
    const char *id = g_object_get_data(G_OBJECT(button), "umicom-automation-id");
    if (strcmp(id, "media.storyboard.library") == 0) {
        GtkWidget *library = g_weak_ref_get(&p->library);
        if (library != NULL) { Reveal(library); g_object_unref(library); }
        else Message(p, "The host has no asset library. Text-only stories are available.");
        return;
    }
    UmiCreativeAssetLibraryEntry *entries = g_new0(UmiCreativeAssetLibraryEntry, UMI_CREATIVE_LIBRARY_MAX_ASSETS);
    size_t count = 0U; UmiStatus status = Inventory(p, entries, &count);
    if (status == UMI_STATUS_OK) {
        GString *text = g_string_new("Import images, avatar images, video, audio or documents in the asset library. Use each exact ID in a story tag.\n\n");
        GtkStringList *choices = gtk_string_list_new(NULL);
        umi_secret_clear(p->asset_ids, sizeof(p->asset_ids));
        p->asset_count = count;
        for (size_t i = 0U; i < count; ++i) {
            memcpy(p->asset_ids[i], entries[i].id, sizeof(p->asset_ids[i]));
            gtk_string_list_append(choices, entries[i].id);
        }
        gtk_drop_down_set_model(GTK_DROP_DOWN(p->asset_choice), G_LIST_MODEL(choices));
        gtk_drop_down_set_selected(GTK_DROP_DOWN(p->asset_choice), count > 0U ? 0U : GTK_INVALID_LIST_POSITION);
        g_object_unref(choices);
        for (size_t i = 0U; i < count; ++i) g_string_append_printf(text, "@{%s} — %s\n", entries[i].id, entries[i].asset.label);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->assets)), text->str, -1);
        g_string_free(text, TRUE); Message(p, "Library inventory refreshed. Re-prepare instructions after changing assets.");
    } else Message(p, "The library is busy or unavailable. Try Refresh assets when its operation finishes.");
    umi_secret_clear(entries, sizeof(*entries) * UMI_CREATIVE_LIBRARY_MAX_ASSETS); g_free(entries);
    Invalidate(p);
}
static void InsertTag(GtkButton *button, gpointer root)
{
    (void)button; StoryPanel *p = State(root); if (p == NULL || p->closed) return;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(p->asset_choice));
    if ((size_t)selected >= p->asset_count) { Message(p, "Refresh assets, then select the asset to reference."); return; }
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->story));
    if (gtk_text_buffer_get_char_count(buffer) > 1024) { Message(p, "Shorten the story before inserting an asset tag."); return; }
    GtkTextIter begin, end; gtk_text_buffer_get_bounds(buffer, &begin, &end);
    char *current = gtk_text_buffer_get_text(buffer, &begin, &end, TRUE);
    size_t length = strlen(current), removed = 0U;
    umi_secret_clear(current, length); g_free(current);
    bool selection = gtk_text_buffer_get_selection_bounds(buffer, &begin, &end);
    if (selection) {
        char *text = gtk_text_buffer_get_text(buffer, &begin, &end, TRUE);
        removed = strlen(text); umi_secret_clear(text, removed); g_free(text);
    }
    char tag[UMI_CREATIVE_ID_CAPACITY + 3U];
    g_snprintf(tag, sizeof(tag), "@{%s}", p->asset_ids[selected]);
    if (length - removed + strlen(tag) >= UMI_MEDIA_STORYBOARD_STORY_CAPACITY) {
        Message(p, "The tag would exceed the 1024-byte story limit. Nothing was changed."); return;
    }
    /* Insert identity, not a file path or payload. Final preparation resolves it
     * again, so a removed library item cannot become an unbound reference. */
    gtk_text_buffer_begin_user_action(buffer);
    if (selection) gtk_text_buffer_delete(buffer, &begin, &end);
    gtk_text_buffer_insert_at_cursor(buffer, tag, -1);
    gtk_text_buffer_end_user_action(buffer);
    gtk_widget_grab_focus(p->story);
}
static void Prepare(GtkButton *button, gpointer root)
{
    (void)button; StoryPanel *p = State(root); if (p == NULL || p->closed) return;
    if (p->dirty) { Message(p, "Apply pending scene edits before preparing instructions."); return; }
    UmiMediaStoryboard *plan = NULL; UmiCreativeExport output = {0};
    UmiStatus status = Plan(p, &plan);
    if (status == UMI_STATUS_OK) status = UmiMediaStoryboardInstructions(plan, &output);
    UmiMediaStoryboardDestroy(plan);
    if (status == UMI_STATUS_OK) {
        UmiCreativeExportFree(&p->reviewed); p->reviewed = output;
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->output)), (const char *)output.bytes, (int)output.size);
        p->ready = true;
        GtkWidget *brief = g_weak_ref_get(&p->brief);
        gtk_widget_set_sensitive(p->handoff, brief != NULL);
        g_clear_object(&brief);
        Message(p, "Instructions prepared locally. Export the text or send one scene to Create from text for further review. Assets have not been uploaded.");
    } else {
        Invalidate(p);
        Message(p, status == UMI_STATUS_NOT_FOUND ? "An asset tag was not found. Refresh assets and use the exact @{asset-id}."
            : status == UMI_STATUS_BUSY ? "The asset library is busy. Try again when it finishes."
            : "Check that scenes exist and tags use @{asset-id}, with at most eight different assets per scene. Previous instructions were kept.");
    }
}
static void Handoff(GtkButton *button, gpointer root)
{
    (void)button; StoryPanel *p = State(root);
    if (p == NULL || p->closed || !p->ready || p->dirty || p->selected >= p->draft.count) return;
    UmiMediaStoryboard *plan = NULL; UmiCreativeExport current = {0}, scene = {0};
    UmiStatus status = Plan(p, &plan);
    if (status == UMI_STATUS_OK) status = UmiMediaStoryboardInstructions(plan, &current);
    /* Recheck the bound inventory at the action boundary: a library edit may
     * have occurred on another page since the user reviewed these instructions. */
    if (status == UMI_STATUS_OK && (current.size != p->reviewed.size ||
        memcmp(current.bytes, p->reviewed.bytes, current.size) != 0)) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = UmiMediaStoryboardSceneInstructions(plan, p->selected, &scene);
    UmiMediaBriefDraft draft = {0}; draft.kind = UMI_MEDIA_BRIEF_VIDEO;
    if (status == UMI_STATUS_OK && scene.size >= sizeof(draft.source)) status = UMI_STATUS_CAPACITY_EXCEEDED;
    GtkWidget *brief = g_weak_ref_get(&p->brief);
    if (status == UMI_STATUS_OK) {
        memcpy(draft.title, p->draft.scenes[p->selected].title, sizeof(draft.title));
        memcpy(draft.source, scene.bytes, scene.size); draft.source[scene.size] = 0;
        status = brief != NULL ? UmiMediaBriefGtkSetSource(brief, &draft,
            gtk_check_button_get_active(GTK_CHECK_BUTTON(p->replace))) : UMI_STATUS_UNAVAILABLE;
    }
    if (status == UMI_STATUS_OK) { Reveal(brief); Message(p, "Scene copied to Create from text. Review and prepare its brief there."); }
    else if (status == UMI_STATUS_ALREADY_EXISTS) Message(p, "Create from text has a draft. Save it or allow replacement before copying this scene.");
    else if (status == UMI_STATUS_CAPACITY_EXCEEDED) Message(p, "This scene's instructions exceed the 2048-byte source limit. Shorten its story or export the full instructions.");
    else { Invalidate(p); Message(p, "The library changed, is busy, or the destination is unavailable. Prepare and review instructions again."); }
    gtk_check_button_set_active(GTK_CHECK_BUTTON(p->replace), FALSE);
    g_clear_object(&brief); UmiCreativeExportFree(&current); UmiCreativeExportFree(&scene);
    UmiMediaStoryboardDestroy(plan); umi_secret_clear(&draft, sizeof(draft));
}
static GtkWidget *Button(StoryPanel *p, GtkWidget *box, const char *id, const char *text, GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(text); Tag(button, id);
    g_signal_connect_object(button, "clicked", callback, p->root, 0);
    gtk_box_append(GTK_BOX(box), button); return button;
}
static GtkWidget *Text(GtkWidget *box, const char *id, bool editable)
{
    GtkWidget *view = gtk_text_view_new(), *scroll = gtk_scrolled_window_new(); Tag(view, id);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), editable);
    UmiGtk4RecordingSetPrivate(view, true);
    gtk_widget_set_vexpand(scroll, TRUE); gtk_widget_set_size_request(scroll, -1, 120);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view); gtk_box_append(GTK_BOX(box), scroll); return view;
}
static GtkWidget *Pane(GtkWidget *split, bool start)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8), *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);
    if (start) gtk_paned_set_start_child(GTK_PANED(split), scroll); else gtk_paned_set_end_child(GTK_PANED(split), scroll);
    return box;
}
static GtkWidget *Split(const char *id, int position)
{
    GtkWidget *split = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL); Tag(split, id);
    gtk_paned_set_resize_start_child(GTK_PANED(split), TRUE); gtk_paned_set_resize_end_child(GTK_PANED(split), TRUE);
    gtk_paned_set_shrink_start_child(GTK_PANED(split), TRUE); gtk_paned_set_shrink_end_child(GTK_PANED(split), TRUE);
    gtk_paned_set_position(GTK_PANED(split), position); gtk_widget_set_vexpand(split, TRUE); return split;
}
void UmiMediaStoryboardGtkRetire(GtkWidget *root)
{
    if (root == NULL) return;
    StoryPanel *p = State(root); if (p == NULL || p->closed) return;
    p->closed = true; Invalidate(p); UmiTextExportGtk4Retire(p->export); gtk_widget_set_sensitive(root, FALSE);
}
static void Release(gpointer data)
{
    StoryPanel *p = data; UmiTextExportGtk4Retire(p->export); g_clear_object(&p->export);
    g_weak_ref_clear(&p->library); g_weak_ref_clear(&p->brief); UmiCreativeExportFree(&p->reviewed);
    umi_secret_clear(&p->draft, sizeof(p->draft));
    umi_secret_clear(p->asset_ids, sizeof(p->asset_ids)); g_free(p);
}
GtkWidget *UmiMediaStoryboardGtkCreate(GtkWidget *asset_library, GtkWidget *creation_brief)
{
    if ((asset_library != NULL && !GTK_IS_WIDGET(asset_library)) ||
        (creation_brief != NULL && !GTK_IS_WIDGET(creation_brief))) return NULL;
    StoryPanel *p = g_new0(StoryPanel, 1); p->selected = SIZE_MAX;
    p->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); Tag(p->root, "media.storyboard.panel");
    g_object_set_data_full(G_OBJECT(p->root), STORY_KEY, p, Release);
    g_weak_ref_init(&p->library, asset_library); g_weak_ref_init(&p->brief, creation_brief);
    Label(p->root, "Storyboard");
    Label(p->root, "Plan scenes with images, avatars, audio and text references. Write @{asset-id} to tag an imported asset. Scenes remain in this session; export instructions before closing.");
    GtkWidget *split = Split("media.storyboard.split", 280); gtk_box_append(GTK_BOX(p->root), split);
    GtkWidget *left = Pane(split, true), *editor_split = Split("media.storyboard.editor-split", 430);
    gtk_paned_set_end_child(GTK_PANED(split), editor_split);
    GtkWidget *editor = Pane(editor_split, true), *right = Pane(editor_split, false);
    Label(left, "Assets and references");
    Button(p, left, "media.storyboard.library", "Open asset library", G_CALLBACK(Assets));
    Button(p, left, "media.storyboard.refresh", "Refresh assets", G_CALLBACK(Assets));
    Label(left, "Reference an imported asset");
    p->asset_choice = gtk_drop_down_new(NULL, NULL); Tag(p->asset_choice, "media.storyboard.asset-choice");
    gtk_box_append(GTK_BOX(left), p->asset_choice);
    Button(p, left, "media.storyboard.insert-tag", "Insert tag at cursor", G_CALLBACK(InsertTag));
    p->assets = Text(left, "media.storyboard.assets", false);
    Label(editor, "Scenes in playback order"); p->selection = gtk_drop_down_new(NULL, NULL);
    Tag(p->selection, "media.storyboard.scenes"); gtk_box_append(GTK_BOX(editor), p->selection);
    Label(editor, "Scene title"); p->title = gtk_entry_new(); Tag(p->title, "media.storyboard.title");
    UmiGtk4RecordingSetPrivate(p->title, true); gtk_box_append(GTK_BOX(editor), p->title);
    Label(editor, "Requested duration in seconds"); p->duration = gtk_spin_button_new_with_range(1.0, 600.0, 1.0);
    Tag(p->duration, "media.storyboard.duration"); gtk_box_append(GTK_BOX(editor), p->duration);
    Label(editor, "Story, camera direction, narration and tagged assets"); p->story = Text(editor, "media.storyboard.story", true);
    Button(p, editor, "media.storyboard.add", "Add as new scene", G_CALLBACK(Edit));
    Button(p, editor, "media.storyboard.apply", "Apply changes to selected scene", G_CALLBACK(Edit));
    Button(p, editor, "media.storyboard.discard", "Discard pending edits", G_CALLBACK(Edit));
    Button(p, editor, "media.storyboard.up", "Move scene earlier", G_CALLBACK(Edit));
    Button(p, editor, "media.storyboard.down", "Move scene later", G_CALLBACK(Edit));
    Button(p, editor, "media.storyboard.remove", "Remove selected scene", G_CALLBACK(Edit));
    Button(p, right, "media.storyboard.prepare", "Prepare instructions", G_CALLBACK(Prepare));
    p->output = Text(right, "media.storyboard.instructions", false);
    p->replace = gtk_check_button_new_with_label("Allow replacing the existing Create from text source on the next copy");
    Tag(p->replace, "media.storyboard.replace"); gtk_box_append(GTK_BOX(right), p->replace);
    p->handoff = Button(p, right, "media.storyboard.handoff", "Use selected scene in Create from text", G_CALLBACK(Handoff));
    p->export = UmiTextExportGtk4Create(p->output, "storyboard-instructions.txt");
    if (p->export != NULL) { g_object_ref_sink(p->export); gtk_box_append(GTK_BOX(right), p->export); }
    p->message = Label(p->root, "Enter your first scene, then choose Add as new scene."); Tag(p->message, "media.storyboard.message");
    g_signal_connect_object(p->title, "changed", G_CALLBACK(Changed), p->root, 0);
    g_signal_connect_object(p->duration, "value-changed", G_CALLBACK(DurationChanged), p->root, 0);
    g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->story)), "changed", G_CALLBACK(Changed), p->root, 0);
    g_signal_connect_object(p->selection, "notify::selected", G_CALLBACK(Selected), p->root, 0);
    Load(p); Invalidate(p); return p->root;
}
