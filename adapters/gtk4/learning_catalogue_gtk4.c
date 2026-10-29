/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_gtk4.c
 *
 * PURPOSE:
 *   Present shared offline catalogue actions without borrowing application state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_catalogue_private.h"
#include "umicom/base/text.h"
#include "umicom/ui/gtk4/path_picker.h"
#include <string.h>

LearningCatalogue *LearningCatalogueOwner(GtkWidget *widget)
{
    return widget != NULL && GTK_IS_WIDGET(widget) ?
        g_object_get_data(G_OBJECT(widget), UMI_LEARNING_CATALOGUE_DATA) : NULL;
}
static void Tag(GtkWidget *widget, const char *id)
{
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free);
}
static void CatalogueFree(gpointer data)
{
    LearningCatalogue *owner = data;
    /* Worker requests own independent C objects and only a weak root reference.
     * Finalizing this widget therefore never waits for a filesystem operation. */
    UmiLearningReaderDestroy(owner->reader);
    UmiLearningSearchDestroy(owner->search);
    UmiLearningShelfDestroy(owner->shelf);
    g_free(owner);
}
void LearningCatalogueMessage(LearningCatalogue *owner, UmiStatus status, const char *detail)
{
    owner->state.last_status = status;
    if (owner->state.retired) return;
    char *message = status == UMI_STATUS_OK ? g_strdup(detail) :
        g_strdup_printf("%s. %s", umi_status_text(status), detail);
    gtk_label_set_text(GTK_LABEL(owner->status), message);
    g_free(message);
}

UmiStatus LearningCatalogueRefresh(LearningCatalogue *owner, const char *query)
{
    UmiLearningSearchQuery request = {0};
    UmiStatus status = umi_text_copy(request.text, sizeof(request.text), query);
    if (status != UMI_STATUS_OK) return status;
    UmiLearningSearch *search = NULL;
    status = UmiLearningSearchCreate(owner->shelf, &request, 128U, &search);
    if (status != UMI_STATUS_OK) return status;
    UmiLearningSearchInfo info;
    status = UmiLearningSearchCopyInfo(search, &info);
    GtkStringList *rows = gtk_string_list_new(NULL);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < info.result_count; ++i) {
        UmiLearningSearchHit hit;
        status = UmiLearningSearchResultAt(search, i, &hit);
        if (status == UMI_STATUS_OK) {
            char *title = g_utf8_make_valid(hit.resource.title, -1);
            char *label = g_strdup_printf("%s — %s [%s]", title, hit.pack.id, hit.pack.locale);
            gtk_string_list_append(rows, label);
            g_free(title);
            g_free(label);
        }
    }
    if (status == UMI_STATUS_OK) {
        UmiLearningSearchDestroy(owner->search);
        owner->search = search;
        search = NULL;
        owner->state.search = info;
        owner->state.pack_count = UmiLearningShelfCount(owner->shelf);
        gtk_drop_down_set_model(GTK_DROP_DOWN(owner->results), G_LIST_MODEL(rows));
        if (!owner->state.retired)
            gtk_drop_down_set_selected(GTK_DROP_DOWN(owner->results),
                info.result_count != 0U ? 0U : GTK_INVALID_LIST_POSITION);
    }
    g_object_unref(rows);
    UmiLearningSearchDestroy(search);
    return status;
}

static UmiStatus CanStart(LearningCatalogue *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    return owner->state.busy ? UMI_STATUS_BUSY : UMI_STATUS_OK;
}
static LearningCatalogueJob *NewJob(LearningCatalogue *owner, LearningCatalogueAction action)
{
    LearningCatalogueJob *job = g_try_new0(LearningCatalogueJob, 1U);
    if (job != NULL) {
        g_weak_ref_init(&job->root, owner->root);
        job->action = action;
        job->host = owner->host;
    }
    return job;
}
UmiStatus UmiLearningCatalogueGtk4Load(GtkWidget *widget, const char *directory)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = CanStart(owner);
    if (status != UMI_STATUS_OK) return status;
    if (directory == NULL || !umi_path_is_absolute(directory)) return UMI_STATUS_INVALID_ARGUMENT;
    LearningCatalogueJob *job = NewJob(owner, LEARNING_CATALOGUE_LOAD);
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_path_copy(job->directory, sizeof(job->directory), directory);
    if (status == UMI_STATUS_OK && !g_utf8_validate(job->directory, -1, NULL))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status != UMI_STATUS_OK) { LearningCatalogueJobFree(job); return status; }
    g_object_ref(widget);
    status = LearningCatalogueDispatch(owner, job);
    g_object_unref(widget);
    return status;
}
UmiStatus UmiLearningCatalogueGtk4Search(GtkWidget *widget, const char *query)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = CanStart(owner);
    if (status != UMI_STATUS_OK) return status;
    if (query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char text[UMI_LEARNING_SEARCH_TEXT_CAPACITY];
    status = umi_text_copy(text, sizeof(text), query);
    if (status != UMI_STATUS_OK) return status;
    if (!g_utf8_validate(text, -1, NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    g_object_ref(widget);
    owner->state.busy = 1;
    status = LearningCatalogueRefresh(owner, text);
    if (!owner->state.retired && status == UMI_STATUS_OK)
        gtk_editable_set_text(GTK_EDITABLE(owner->query), text);
    if (!owner->state.retired) {
        char *summary = g_strdup_printf("%zu matching resources; %zu displayed.%s",
            owner->state.search.matching_resources, owner->state.search.result_count,
            owner->state.search.truncated ? " Refine the search to see further results." : "");
        LearningCatalogueMessage(owner, status, summary);
        g_free(summary);
    }
    owner->state.busy = 0;
    g_object_unref(widget);
    return status;
}
/* Preserve index-only opening for review. Shared selection dispatch now also
 * accepts exact help routes while retaining the same reader ownership. */
#if 0
UmiStatus UmiLearningCatalogueGtk4Open(GtkWidget *widget, size_t index)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = CanStart(owner);
    if (status != UMI_STATUS_OK) return status;
    if (owner->search == NULL) return UMI_STATUS_NOT_FOUND;
    LearningCatalogueJob *job = NewJob(owner, LEARNING_CATALOGUE_OPEN);
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiLearningSearchResultAt(owner->search, index, &job->selected);
    UmiLearningPack *pack = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiLearningShelfCopyPack(owner->shelf, job->selected.pack.id, &pack);
    if (status == UMI_STATUS_OK) status = UmiLearningShelfCreate(&job->selection_shelf);
    if (status == UMI_STATUS_OK) status = UmiLearningShelfAdopt(job->selection_shelf, &pack);
    UmiLearningPackDestroy(pack);
    if (status != UMI_STATUS_OK) { LearningCatalogueJobFree(job); return status; }
    if (owner->reader != NULL && owner->state.reader.pack.revision == job->selected.pack.revision &&
        strcmp(owner->state.reader.pack.id, job->selected.pack.id) == 0) {
        job->continues_reader = 1;
        job->reader = owner->reader;
        owner->reader = NULL;
    }
    g_object_ref(widget);
    status = LearningCatalogueDispatch(owner, job);
    g_object_unref(widget);
    return status;
}#endif
UmiStatus UmiLearningCatalogueGtk4Open(GtkWidget *widget, size_t index)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = CanStart(owner);
    if (status != UMI_STATUS_OK) return status;
    if (owner->search == NULL) return UMI_STATUS_NOT_FOUND;
    UmiLearningSearchHit hit;
    status = UmiLearningSearchResultAt(owner->search, index, &hit);
    return status == UMI_STATUS_OK ? LearningCatalogueOpenSelection(owner, &hit.pack, &hit.resource) : status;
}

static UmiStatus Navigate(GtkWidget *widget, LearningCatalogueAction action)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    UmiStatus status = CanStart(owner);
    if (status != UMI_STATUS_OK) return status;
    if (owner->reader == NULL) return UMI_STATUS_INVALID_STATE;
    LearningCatalogueJob *job = NewJob(owner, action);
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* One worker temporarily owns the complete reader. Widgets retain only a
     * pointer-free snapshot until completion returns ownership on the GTK thread. */
    job->reader = owner->reader;
    owner->reader = NULL;
    g_object_ref(widget);
    status = LearningCatalogueDispatch(owner, job);
    g_object_unref(widget);
    return status;
}
UmiStatus UmiLearningCatalogueGtk4Back(GtkWidget *widget)
{ return Navigate(widget, LEARNING_CATALOGUE_BACK); }
UmiStatus UmiLearningCatalogueGtk4Forward(GtkWidget *widget)
{ return Navigate(widget, LEARNING_CATALOGUE_FORWARD); }
UmiStatus UmiLearningCatalogueGtk4Reload(GtkWidget *widget)
{ return Navigate(widget, LEARNING_CATALOGUE_RELOAD); }

UmiStatus UmiLearningCatalogueGtk4CopyState(GtkWidget *widget, UmiLearningCatalogueGtkState *out)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = owner->state;
    return UMI_STATUS_OK;
}
void UmiLearningCatalogueGtk4Retire(GtkWidget *widget)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL || owner->state.retired) return;
    g_object_ref(widget);
    owner->state.retired = 1;
    gtk_widget_set_sensitive(owner->controls, FALSE);
    g_object_unref(widget);
}

static void Action(GtkButton *button, gpointer data)
{
    GtkWidget *widget = data;
    g_object_ref(widget);
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (CanStart(owner) == UMI_STATUS_OK) {
        const char *action = g_object_get_data(G_OBJECT(button), "learning-catalogue-action");
        UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(action, "load") == 0)
            status = UmiLearningCatalogueGtk4Load(widget, gtk_editable_get_text(GTK_EDITABLE(owner->path)));
        else if (strcmp(action, "search") == 0)
            status = UmiLearningCatalogueGtk4Search(widget, gtk_editable_get_text(GTK_EDITABLE(owner->query)));
        else if (strcmp(action, "open") == 0) {
            guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->results));
            status = selected == GTK_INVALID_LIST_POSITION ? UMI_STATUS_NOT_FOUND :
                UmiLearningCatalogueGtk4Open(widget, (size_t)selected);
        } else if (strcmp(action, "back") == 0) status = UmiLearningCatalogueGtk4Back(widget);
        else if (strcmp(action, "forward") == 0) status = UmiLearningCatalogueGtk4Forward(widget);
        else if (strcmp(action, "reload") == 0) status = UmiLearningCatalogueGtk4Reload(widget);
        else if (strcmp(action, "example") == 0) {
            UmiLearningPackResource example;
            status = UmiLearningCatalogueGtk4PreviewExample(widget, &example);
            if (status == UMI_STATUS_OK) {
                char *title = g_utf8_make_valid(example.title, -1);
                char *detail = g_strdup_printf("Example: %s [%s], source %s. "
                    "Choose a NEW destination filename, then explicitly copy.", title, example.id, example.path);
                LearningCatalogueMessage(owner, status, detail);
                g_free(detail);
                g_free(title);
            }
        } else if (strcmp(action, "copy") == 0) {
            const char *filename = gtk_editable_get_text(GTK_EDITABLE(owner->destination));
            char directory[UMI_PATH_CAPACITY], name[UMI_PATH_CAPACITY];
            status = umi_path_is_absolute(filename) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
            if (status == UMI_STATUS_OK) status = umi_path_parent(filename, directory, sizeof(directory));
            if (status == UMI_STATUS_OK) status = umi_path_basename(filename, name, sizeof(name));
            if (status == UMI_STATUS_OK)
                status = UmiLearningCatalogueGtk4CopyExample(widget, directory, name);
        }

        if (status != UMI_STATUS_OK && !owner->state.retired)
            LearningCatalogueMessage(owner, status, "Check the selected pack, result and local path.");
    }
    g_object_unref(widget);
}
static GtkWidget *Caption(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(box), label);
    return label;
}
static GtkWidget *Button(LearningCatalogue *owner, GtkWidget *box,
    const char *text, const char *action)
{
    GtkWidget *button = gtk_button_new_with_label(text);
    char *id = g_strdup_printf("learning.catalogue.%s", action);
    Tag(button, id);
    g_free(id);
    g_object_set_data(G_OBJECT(button), "learning-catalogue-action", (gpointer)action);
    /* A retained button has no borrowed controller pointer. GLib disconnects
     * it when the root dies; explicit retirement also refuses its operation. */
    g_signal_connect_object(button, "clicked", G_CALLBACK(Action), owner->root, 0);
    gtk_box_append(GTK_BOX(box), button);
    return button;
}

GtkWidget *UmiLearningCatalogueGtk4Create(UmiVersion host)
{
    LearningCatalogue *owner = g_try_new0(LearningCatalogue, 1U);
    if (owner == NULL) return NULL;
    if (UmiLearningShelfCreate(&owner->shelf) != UMI_STATUS_OK) { g_free(owner); return NULL; }
    owner->host = host;
    owner->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    g_object_set_data_full(G_OBJECT(owner->root), UMI_LEARNING_CATALOGUE_DATA, owner, CatalogueFree);
    Tag(owner->root, "learning.catalogue");
    Caption(owner->root, "Offline product guides and source examples");
    Caption(owner->root, "Choose a folder containing learning-pack.json, then load it. "
        "Only declared, verified resources are read. No source example is run.");
    owner->controls = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    /* A long catalogue should not force the reading window taller than the
     * available display. Preserve the earlier direct placement for review;
     * controls now scroll independently while the document remains visible. */
#if 0
    gtk_box_append(GTK_BOX(owner->root), owner->controls);
#endif
    GtkWidget *control_scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(control_scroll),
        GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(control_scroll), 160);
    gtk_scrolled_window_set_max_content_height(GTK_SCROLLED_WINDOW(control_scroll), 340);
    gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(control_scroll), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(control_scroll), owner->controls);
    gtk_box_append(GTK_BOX(owner->root), control_scroll);
    Caption(owner->controls, "Learning pack folder");
    owner->path = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(owner->path), (int)UMI_PATH_CAPACITY - 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(owner->path), "Absolute local folder");
    Tag(owner->path, "learning.catalogue.path");
    gtk_box_append(GTK_BOX(owner->controls), owner->path);
    GtkWidget *browse = UmiPathPickerGtk4Create(GTK_ENTRY(owner->path),
        UMI_PATH_PICKER_DIRECTORY, "Choose a learning pack folder", NULL);
    if (browse != NULL) gtk_box_append(GTK_BOX(owner->controls), browse);
    Button(owner, owner->controls, "Load learning pack", "load");
    Caption(owner->controls, "Search resource titles, topics and identifiers");
    owner->query = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(owner->query), (int)UMI_LEARNING_SEARCH_TEXT_CAPACITY - 1);
    Tag(owner->query, "learning.catalogue.query");
    gtk_box_append(GTK_BOX(owner->controls), owner->query);
    Button(owner, owner->controls, "Search catalogue", "search");
    owner->results = gtk_drop_down_new(G_LIST_MODEL(gtk_string_list_new(NULL)), NULL);
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(owner->results), TRUE);
    Tag(owner->results, "learning.catalogue.results");
    gtk_box_append(GTK_BOX(owner->controls), owner->results);
    Button(owner, owner->controls, "Open selected", "open");
    GtkWidget *navigation = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_append(GTK_BOX(owner->controls), navigation);
    Button(owner, navigation, "Back", "back");
    Button(owner, navigation, "Forward", "forward");
    Button(owner, navigation, "Reload and verify", "reload");
    Caption(owner->controls, "Copy one example from the current reading selection");
    Button(owner, owner->controls, "Preview example details", "example");
    owner->destination = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(owner->destination), (int)UMI_PATH_CAPACITY - 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(owner->destination), "Absolute NEW destination filename");
    Tag(owner->destination, "learning.catalogue.destination");
    gtk_box_append(GTK_BOX(owner->controls), owner->destination);
    GtkWidget *choose_destination = UmiPathPickerGtk4Create(GTK_ENTRY(owner->destination),
        UMI_PATH_PICKER_NEW_FILE, "Choose a new practice filename", NULL);
    if (choose_destination != NULL) gtk_box_append(GTK_BOX(owner->controls), choose_destination);
    Caption(owner->controls, "The parent folder must exist. Copying never overwrites a file, "
        "opens a project or runs code. Closing does not undo an accepted copy.");
    Button(owner, owner->controls, "Copy example to new file", "copy");
    GtkWidget *receipt = gtk_expander_new("Last example copy");
    GtkWidget *receipt_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    owner->copy_status = Caption(receipt_box, "No example copy has completed.");
    Tag(owner->copy_status, "learning.catalogue.copy-result");
    gtk_expander_set_child(GTK_EXPANDER(receipt), receipt_box);
    gtk_box_append(GTK_BOX(owner->controls), receipt);
    owner->heading = Caption(owner->root, "No resource opened");
    Tag(owner->heading, "learning.catalogue.heading");
    owner->status = Caption(owner->root, "No learning pack loaded.");
    Tag(owner->status, "learning.catalogue.status");
    owner->content = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(owner->content), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(owner->content), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(owner->content), TRUE);
    Tag(owner->content, "learning.catalogue.content");
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), owner->content);
    gtk_widget_set_size_request(scroll, -1, 280);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(owner->root), scroll);
    return owner->root;
}
