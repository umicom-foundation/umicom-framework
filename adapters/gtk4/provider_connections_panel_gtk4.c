/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/provider_connections_panel_gtk4.c
 * PURPOSE: Present editable drafts beside saved settings and require explicit conflict review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "provider_connections_internal.h"
#include "umicom/security/gtk4/profile_keys.h"
#include "umicom/ui/gtk4/automation.h"
#include <inttypes.h>
#include <string.h>

static UmiProviderConnectionsGtk *FromRoot(gpointer root)
{
    return g_object_get_data(G_OBJECT(root), "umicom-provider-editor");
}
void UmiProviderEditorRelease(UmiProviderConnectionsGtk *panel)
{
    if (--panel->references == 0U) g_free(panel);
}
static void Message(UmiProviderConnectionsGtk *panel, const char *text)
{
    gtk_label_set_text(GTK_LABEL(panel->message), text);
}
static UmiStatus Copy(char *out, size_t capacity, const char *text)
{
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < capacity && text[length] != '\0') ++length;
    if (length == capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}
static size_t Find(UmiProviderConnectionsGtk *panel, const char *id)
{
    for (size_t i = 0U; i < panel->snapshot.count; ++i)
        if (strcmp(panel->snapshot.items[i].id, id) == 0) return i;
    return panel->snapshot.count;
}
/* Saved evidence and editable fields are separate. Reload can therefore show
 * another window's edit without overwriting this user's unsaved draft. */
static void SavedDetail(UmiProviderConnectionsGtk *panel)
{
    size_t index = Find(panel, gtk_editable_get_text(GTK_EDITABLE(panel->id)));
    char *text;
    if (index == panel->snapshot.count) {
        text = g_strdup_printf("Saved revision: %" PRIu64 "\nNo saved connection has this ID.", panel->snapshot.revision);
    } else {
        const UmiProviderConnection *value = &panel->snapshot.items[index];
        text = g_strdup_printf("Saved revision: %" PRIu64 "\nID: %s\nName: %s\nProvider: %s\nEndpoint: %s\nModel: %s\nCredential reference: %s\nRoute: %s · Timeout: %" PRIu32 " ms · %s",
            panel->snapshot.revision, value->id, value->label, value->provider_id, value->endpoint,
            value->model[0] != '\0' ? value->model : "Not selected",
            value->secret_reference[0] != '\0' ? value->secret_reference : "None",
            value->route == UMI_PROVIDER_CONNECTION_LOOPBACK ? "Local loopback" : "HTTPS",
            value->timeout_ms, value->enabled ? "Enabled" : "Disabled");
    }
    /* Plain text avoids treating user-entered labels as markup. */
    gtk_label_set_text(GTK_LABEL(panel->saved), text);
    g_free(text);
}
static void Sensitivity(UmiProviderConnectionsGtk *panel)
{
    gtk_widget_set_sensitive(panel->form, !panel->busy);
    gtk_widget_set_sensitive(panel->fields, panel->loaded);
    gtk_widget_set_sensitive(panel->selector, panel->loaded);
    gtk_widget_set_sensitive(panel->save, panel->loaded);
    gtk_widget_set_sensitive(panel->remove, panel->loaded && panel->editing_existing);
    gtk_widget_set_sensitive(panel->rebase, panel->loaded && panel->dirty);
    UmiProviderCheckRefresh(panel);
}
static void Changed(GtkWidget *widget, gpointer root)
{
    (void)widget;
    UmiProviderConnectionsGtk *panel = FromRoot(root);
    if (panel == NULL || panel->closed || panel->painting || panel->busy) return;
    panel->dirty = true;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm_remove), FALSE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm_review), FALSE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->discard), FALSE);
    SavedDetail(panel);
    Sensitivity(panel);
    Message(panel, "Unsaved draft. Review the fields before saving.");
}
static void RouteChanged(GObject *object, GParamSpec *property, gpointer root)
{
    (void)property; Changed(GTK_WIDGET(object), root);
}
static bool CanDiscard(UmiProviderConnectionsGtk *panel)
{
    if (!panel->dirty || gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->discard))) return true;
    Message(panel, "Your draft is unsaved. Save it, or confirm discarding it before changing selection or closing.");
    return false;
}
static void SelectWidget(UmiProviderConnectionsGtk *panel)
{
    size_t index = Find(panel, panel->selected_id);
    panel->painting = true;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->selector), index < panel->snapshot.count ? (guint)index : GTK_INVALID_LIST_POSITION);
    panel->painting = false;
}
static void Fill(UmiProviderConnectionsGtk *panel, const UmiProviderConnection *connection)
{
    panel->painting = true;
    panel->editing_existing = connection != NULL;
    (void)Copy(panel->selected_id, sizeof(panel->selected_id), connection != NULL ? connection->id : "");
    gtk_editable_set_text(GTK_EDITABLE(panel->id), connection != NULL ? connection->id : "");
    gtk_editable_set_editable(GTK_EDITABLE(panel->id), connection == NULL);
    gtk_editable_set_text(GTK_EDITABLE(panel->provider), connection != NULL ? connection->provider_id : "local-chat");
    gtk_editable_set_text(GTK_EDITABLE(panel->label), connection != NULL ? connection->label : "Local model");
    gtk_editable_set_text(GTK_EDITABLE(panel->endpoint), connection != NULL ? connection->endpoint : "http://127.0.0.1:8080/v1/chat/completions");
    gtk_editable_set_text(GTK_EDITABLE(panel->model), connection != NULL ? connection->model : "");
    gtk_editable_set_text(GTK_EDITABLE(panel->reference), connection != NULL ? connection->secret_reference : "");
    gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->route), connection != NULL && connection->route == UMI_PROVIDER_CONNECTION_HTTPS ? 0U : 1U);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->timeout), connection != NULL ? (double)connection->timeout_ms : 30000.0);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->enabled), connection != NULL && connection->enabled);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->discard), FALSE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm_remove), FALSE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm_review), FALSE);
    panel->draft_revision = panel->snapshot.revision;
    panel->dirty = false;
    panel->painting = false;
    SelectWidget(panel); SavedDetail(panel); Sensitivity(panel);
}
static void SelectionChanged(GObject *object, GParamSpec *property, gpointer root)
{
    (void)object; (void)property;
    UmiProviderConnectionsGtk *panel = FromRoot(root);
    if (panel == NULL || panel->closed || panel->painting || panel->busy || !panel->loaded) return;
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector));
    if (!CanDiscard(panel)) { SelectWidget(panel); return; }
    Fill(panel, index < panel->snapshot.count ? &panel->snapshot.items[index] : NULL);
    Message(panel, "Loaded saved settings. Editing these settings does not connect to the provider.");
}
static UmiStatus Draft(UmiProviderConnectionsGtk *panel, UmiProviderConnection *out)
{
    struct Field { GtkWidget *widget; char *out; size_t capacity; } fields[] = {
        {panel->id, out->id, sizeof(out->id)}, {panel->provider, out->provider_id, sizeof(out->provider_id)},
        {panel->label, out->label, sizeof(out->label)}, {panel->endpoint, out->endpoint, sizeof(out->endpoint)},
        {panel->model, out->model, sizeof(out->model)}, {panel->reference, out->secret_reference, sizeof(out->secret_reference)}
    };
    for (size_t i = 0U; i < sizeof(fields) / sizeof(fields[0]); ++i) {
        UmiStatus status = Copy(fields[i].out, fields[i].capacity, gtk_editable_get_text(GTK_EDITABLE(fields[i].widget)));
        if (status != UMI_STATUS_OK) return status;
    }
    guint route = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->route));
    if (route > 1U) return UMI_STATUS_INVALID_ARGUMENT;
    out->route = route == 0U ? UMI_PROVIDER_CONNECTION_HTTPS : UMI_PROVIDER_CONNECTION_LOOPBACK;
    out->timeout_ms = (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->timeout));
    out->enabled = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->enabled));
    return UmiProviderConnectionValidate(out);
}
static void Load(UmiProviderConnectionsGtk *panel)
{
    UmiProviderEditorJob *job = g_try_new0(UmiProviderEditorJob, 1);
    if (job == NULL) { Message(panel, "There is not enough memory to load settings."); return; }
    job->operation = UMI_PROVIDER_EDITOR_LOAD;
    UmiProviderEditorStart(panel, job);
}
static void ReloadClicked(GtkButton *button, gpointer root)
{
    (void)button;
    UmiProviderConnectionsGtk *panel = FromRoot(root);
    if (panel != NULL && !panel->closed && !panel->busy) Load(panel);
}
static void NewClicked(GtkButton *button, gpointer root)
{
    (void)button;
    UmiProviderConnectionsGtk *panel = FromRoot(root);
    if (panel == NULL || panel->closed || panel->busy || !panel->loaded || !CanDiscard(panel)) return;
    Fill(panel, NULL); Message(panel, "New draft. Enter an ID and review the connection settings before saving.");
}
static void SaveClicked(GtkButton *button, gpointer root)
{
    (void)button;
    UmiProviderConnectionsGtk *panel = FromRoot(root);
    if (panel == NULL || panel->closed || panel->busy || !panel->loaded) return;
    UmiProviderEditorJob *job = g_try_new0(UmiProviderEditorJob, 1);
    if (job == NULL) { Message(panel, "There is not enough memory to save this draft."); return; }
    UmiStatus status = Draft(panel, &job->draft);
    if (status != UMI_STATUS_OK) {
        g_free(job);
        Message(panel, status == UMI_STATUS_CAPACITY_EXCEEDED ? "A field is too long in UTF-8 bytes. Shorten it; the draft has not been truncated."
            : "Check the ID, provider, endpoint and credential reference. Local loopback connections require an empty credential reference.");
        return;
    }
    job->operation = UMI_PROVIDER_EDITOR_SAVE;
    job->replace_existing = panel->editing_existing;
    job->expected_revision = panel->draft_revision;
    UmiProviderEditorStart(panel, job);
}
static void RemoveClicked(GtkButton *button, gpointer root)
{
    (void)button;
    UmiProviderConnectionsGtk *panel = FromRoot(root);
    if (panel == NULL || panel->closed || panel->busy || !panel->loaded || !panel->editing_existing) return;
    if (!gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->confirm_remove))) {
        Message(panel, "Confirm removing the selected saved connection first. Its credential will remain in the separate store."); return;
    }
    if (!CanDiscard(panel)) return;
    UmiProviderEditorJob *job = g_try_new0(UmiProviderEditorJob, 1);
    if (job == NULL) { Message(panel, "There is not enough memory to prepare removal."); return; }
    job->operation = UMI_PROVIDER_EDITOR_REMOVE;
    job->expected_revision = panel->draft_revision;
    (void)Copy(job->draft.id, sizeof(job->draft.id), panel->selected_id);
    UmiProviderEditorStart(panel, job);
}
static void RebaseClicked(GtkButton *button, gpointer root)
{
    (void)button;
    UmiProviderConnectionsGtk *panel = FromRoot(root);
    if (panel == NULL || panel->closed || panel->busy || !panel->loaded || !panel->dirty) return;
    if (!gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->confirm_review))) {
        Message(panel, "Compare your draft with the saved settings, then confirm that you reviewed the saved revision."); return;
    }
    bool exists = Find(panel, gtk_editable_get_text(GTK_EDITABLE(panel->id))) < panel->snapshot.count;
    if (exists != panel->editing_existing) {
        Message(panel, exists ? "That ID was created elsewhere. Select its saved record or choose another new ID."
            : "The saved record was removed elsewhere. Preserve your draft before choosing New or another saved record."); return;
    }
    /* Explicit review advances only the draft's expected revision. The next
     * Save remains a checked transaction and may reject a newer competing edit. */
    panel->draft_revision = panel->snapshot.revision;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm_review), FALSE);
    Message(panel, "Your draft now uses the reviewed saved revision. Choose Save draft to apply it.");
}
static const char *Failure(UmiStatus status)
{
    switch (status) {
    case UMI_STATUS_BUSY: return "Saved settings changed or storage is busy. Your draft is kept. Reload, compare, and explicitly review before saving again.";
    case UMI_STATUS_ALREADY_EXISTS: return "That ID is already saved. Your draft is kept; reload and choose a different ID or the existing record.";
    case UMI_STATUS_NOT_FOUND: return "The selected record no longer exists. Your draft is kept. Reload to inspect saved settings.";
    case UMI_STATUS_PARSE_ERROR: return "Saved settings are damaged or inconsistent. Your draft is kept. Preserve the database for recovery; do not replace it with an empty file.";
    case UMI_STATUS_UNAVAILABLE: return "SQLite storage is unavailable in this build. No temporary substitute was used. Settings cannot be saved.";
    case UMI_STATUS_CAPACITY_EXCEEDED: return "A storage or connection limit was reached. Your draft is kept. Reload to review the saved records.";
    case UMI_STATUS_INVALID_STATE: return "Storage could not confirm a safe result. Your draft is kept. Reload and review before attempting another edit.";
    default: return "The storage operation failed. Your draft is kept. Check the local settings directory, then Reload.";
    }
}
void UmiProviderEditorCompleted(UmiProviderConnectionsGtk *panel, const UmiProviderEditorJob *job)
{
    panel->busy = false;
    if (job->committed) {
        panel->dirty = false;
        panel->draft_revision = job->saved_revision;
        (void)Copy(panel->selected_id, sizeof(panel->selected_id), job->operation == UMI_PROVIDER_EDITOR_SAVE ? job->draft.id : "");
    }
    if (job->status != UMI_STATUS_OK || job->reload_status != UMI_STATUS_OK) {
        panel->loaded = false;
        Sensitivity(panel);
        Message(panel, job->committed ? "The edit was saved, but the current list could not be reloaded. Choose Reload before editing again."
            : Failure(job->status != UMI_STATUS_OK ? job->status : job->reload_status));
        return;
    }
    panel->snapshot = job->snapshot;
    panel->loaded = true;
    /* The dropdown retains its own string model. Splicing copies these labels
     * without transferring or prematurely releasing model ownership. */
    const char *labels[UMI_PROVIDER_CONNECTION_LIMIT + 1U] = {0};
    for (size_t i = 0U; i < panel->snapshot.count; ++i)
        labels[i] = g_strdup_printf("%s (%s)", panel->snapshot.items[i].label, panel->snapshot.items[i].id);
    panel->painting = true;
    GtkStringList *model = GTK_STRING_LIST(gtk_drop_down_get_model(GTK_DROP_DOWN(panel->selector)));
    gtk_string_list_splice(model, 0U, g_list_model_get_n_items(G_LIST_MODEL(model)), labels);
    panel->painting = false;
    for (size_t i = 0U; i < panel->snapshot.count; ++i) g_free((gpointer)labels[i]);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm_review), FALSE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm_remove), FALSE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->discard), FALSE);
    if (!panel->dirty) {
        size_t selected = Find(panel, panel->selected_id);
        Fill(panel, selected < panel->snapshot.count ? &panel->snapshot.items[selected] : NULL);
    } else { SelectWidget(panel); SavedDetail(panel); Sensitivity(panel); }
    Message(panel, panel->dirty ? "Saved settings reloaded. Your draft is unchanged. Compare the saved values before using the reviewed revision."
        : job->committed ? "Settings saved. No provider login or network request was made."
        : "Saved settings loaded. Select a connection or prepare a new draft.");
}
/* Credential handling is shared with other products and remains outside the
 * metadata editor. Pass the same immutable scope so a stored alias cannot
 * accidentally land under a different application's profile. */
static void KeysClicked(GtkButton *button, gpointer root)
{
    (void)button;
    UmiProviderConnectionsGtk *panel = FromRoot(root);
    if (panel == NULL || panel->closed || panel->busy) return;
    GtkRoot *parent = gtk_widget_get_root(panel->root);
    if (parent == NULL || !GTK_IS_WINDOW(parent)) {
        Message(panel, "Open connection settings in an application window to manage local keys."); return;
    }
    UmiStatus status = UmiProfileKeysGtkPresent(GTK_WINDOW(parent), panel->application, panel->profile);
    if (status != UMI_STATUS_OK)
        Message(panel, "The key panel could not open. Its profile must be a lower-case local profile name of 3 to 48 letters, digits, hyphens or underscores, beginning with a letter.");
}
static GtkWidget *Label(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE); gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(box), label); return label;
}
static GtkWidget *Entry(UmiProviderConnectionsGtk *panel, const char *caption, const char *id, int max_length)
{
    GtkWidget *label = Label(panel->fields, caption);
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entry), max_length);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), entry);
    gtk_accessible_update_property(GTK_ACCESSIBLE(entry), GTK_ACCESSIBLE_PROPERTY_LABEL, caption, -1);
    (void)umi_gtk4_automation_tag_widget(entry, id);
    g_signal_connect_object(entry, "changed", G_CALLBACK(Changed), panel->root, 0);
    gtk_box_append(GTK_BOX(panel->fields), entry); return entry;
}
static GtkWidget *Button(UmiProviderConnectionsGtk *panel, GtkWidget *box, const char *caption, const char *id, GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(caption);
    (void)umi_gtk4_automation_tag_widget(button, id);
    g_signal_connect_object(button, "clicked", callback, panel->root, 0);
    gtk_box_append(GTK_BOX(box), button); return button;
}
static GtkWidget *Check(GtkWidget *box, const char *caption, const char *id)
{
    GtkWidget *check = gtk_check_button_new_with_label(caption);
    (void)umi_gtk4_automation_tag_widget(check, id);
    gtk_box_append(GTK_BOX(box), check); return check;
}
UmiStatus UmiProviderConnectionsGtkCreate(const UmiProviderConnectionsGtkConfig *config, UmiProviderConnectionsGtk **out_panel)
{
    if (config == NULL || out_panel == NULL || config->database_path == NULL ||
        !g_path_is_absolute(config->database_path)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderConnection scope = {0};
    if (Copy(scope.id, sizeof(scope.id), config->application_id) != UMI_STATUS_OK ||
        Copy(scope.provider_id, sizeof(scope.provider_id), config->profile_id) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    strcpy(scope.label, "Scope"); strcpy(scope.endpoint, "http://127.0.0.1:8080");
    scope.route = UMI_PROVIDER_CONNECTION_LOOPBACK; scope.timeout_ms = 30000U;
    if (UmiProviderConnectionValidate(&scope) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderConnectionsGtk *panel = g_try_new0(UmiProviderConnectionsGtk, 1);
    if (panel == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = Copy(panel->path, sizeof(panel->path), config->database_path);
    if (status != UMI_STATUS_OK) { g_free(panel); return status; }
    (void)Copy(panel->application, sizeof(panel->application), config->application_id);
    (void)Copy(panel->profile, sizeof(panel->profile), config->profile_id);
    panel->references = 1U;
    panel->root = gtk_scrolled_window_new(); g_object_ref_sink(panel->root);
    g_object_set_data(G_OBJECT(panel->root), "umicom-provider-editor", panel);
    (void)umi_gtk4_automation_tag_widget(panel->root, "connections.editor");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(box, 16); gtk_widget_set_margin_bottom(box, 16);
    gtk_widget_set_margin_start(box, 16); gtk_widget_set_margin_end(box, 16);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(panel->root), box);
    GtkWidget *title = Label(box, "Provider connections"); gtk_widget_add_css_class(title, "title-2");
    Label(box, "Save local settings for provider adapters. API keys are not shown or edited here. Saving settings does not sign in or change the active AI provider.");
    panel->form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); gtk_box_append(GTK_BOX(box), panel->form);
    Label(panel->form, "Saved connections");
    const char *empty[] = {NULL};
    panel->selector = gtk_drop_down_new_from_strings(empty);
    (void)umi_gtk4_automation_tag_widget(panel->selector, "connections.select");
    gtk_accessible_update_property(GTK_ACCESSIBLE(panel->selector), GTK_ACCESSIBLE_PROPERTY_LABEL, "Saved connections", -1);
    g_signal_connect_object(panel->selector, "notify::selected", G_CALLBACK(SelectionChanged), panel->root, 0);
    gtk_box_append(GTK_BOX(panel->form), panel->selector);
    GtkWidget *navigation = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(panel->form), navigation);
    Button(panel, navigation, "New", "connections.new", G_CALLBACK(NewClicked));
    Button(panel, navigation, "Reload saved settings", "connections.reload", G_CALLBACK(ReloadClicked));
    Button(panel, navigation, "Manage local keys…", "connections.keys", G_CALLBACK(KeysClicked));
    panel->fields = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); gtk_box_append(GTK_BOX(panel->form), panel->fields);
    panel->id = Entry(panel, "Connection ID", "connections.id", 48);
    panel->provider = Entry(panel, "Provider alias", "connections.provider", 48);
    panel->label = Entry(panel, "Display name", "connections.label", 160);
    panel->endpoint = Entry(panel, "Endpoint (no key or password in the URL)", "connections.endpoint", 512);
    panel->model = Entry(panel, "Model identifier (optional)", "connections.model", 128);
    panel->reference = Entry(panel, "Credential reference, such as vault:personal-key (optional)", "connections.reference", 192);
    Label(panel->fields, "Connection route");
    const char *routes[] = {"Remote HTTPS", "Local loopback (no credential)", NULL};
    panel->route = gtk_drop_down_new_from_strings(routes);
    (void)umi_gtk4_automation_tag_widget(panel->route, "connections.route");
    gtk_accessible_update_property(GTK_ACCESSIBLE(panel->route), GTK_ACCESSIBLE_PROPERTY_LABEL, "Connection route", -1);
    g_signal_connect_object(panel->route, "notify::selected", G_CALLBACK(RouteChanged), panel->root, 0);
    gtk_box_append(GTK_BOX(panel->fields), panel->route);
    Label(panel->fields, "Timeout in milliseconds");
    panel->timeout = gtk_spin_button_new_with_range(100.0, 600000.0, 100.0);
    (void)umi_gtk4_automation_tag_widget(panel->timeout, "connections.timeout");
    gtk_accessible_update_property(GTK_ACCESSIBLE(panel->timeout), GTK_ACCESSIBLE_PROPERTY_LABEL, "Timeout in milliseconds", -1);
    g_signal_connect_object(panel->timeout, "value-changed", G_CALLBACK(Changed), panel->root, 0);
    gtk_box_append(GTK_BOX(panel->fields), panel->timeout);
    panel->enabled = Check(panel->fields, "Enable this saved configuration for a compatible adapter", "connections.enabled");
    g_signal_connect_object(panel->enabled, "toggled", G_CALLBACK(Changed), panel->root, 0);
    panel->saved = Label(panel->form, "Saved settings have not loaded yet.");
    gtk_label_set_selectable(GTK_LABEL(panel->saved), TRUE);
    (void)umi_gtk4_automation_tag_widget(panel->saved, "connections.saved");
    panel->confirm_review = Check(panel->form, "I reviewed the saved settings shown above", "connections.confirm-review");
    panel->rebase = Button(panel, panel->form, "Keep my draft using the reviewed revision", "connections.rebase", G_CALLBACK(RebaseClicked));
    panel->discard = Check(panel->form, "Discard my unsaved draft when changing selection, removing or closing", "connections.discard");
    panel->confirm_remove = Check(panel->form, "Remove the selected saved connection; keep its credential", "connections.confirm-remove");
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(panel->form), actions);
    panel->save = Button(panel, actions, "Save draft", "connections.save", G_CALLBACK(SaveClicked));
    panel->remove = Button(panel, actions, "Remove selected", "connections.remove", G_CALLBACK(RemoveClicked));
    panel->message = Label(box, "Loading saved connection settings…");
    (void)umi_gtk4_automation_tag_widget(panel->message, "connections.message");
    UmiProviderChatContextControls(panel, box);
    UmiProviderCheckControls(panel, box);
    Fill(panel, NULL);
    *out_panel = panel;
    Load(panel);
    return UMI_STATUS_OK;
}
GtkWidget *UmiProviderConnectionsGtkWidget(UmiProviderConnectionsGtk *panel)
{
    return panel != NULL && !panel->closed ? panel->root : NULL;
}
bool UmiProviderConnectionsGtkBusy(const UmiProviderConnectionsGtk *panel)
{
    return panel != NULL && !panel->closed && panel->busy;
}
bool UmiProviderConnectionsGtkCanClose(UmiProviderConnectionsGtk *panel)
{
    if (panel == NULL || panel->closed) return true;
    if (panel->busy) { Message(panel, "Wait for the settings operation to finish before closing."); return false; }
    return CanDiscard(panel);
}
void UmiProviderConnectionsGtkDestroy(UmiProviderConnectionsGtk *panel)
{
    if (panel == NULL || panel->closed) return;
    panel->closed = true;
    UmiProviderCheckRetire(panel);
    UmiProviderChatContextRetire(panel);
    gtk_widget_set_sensitive(panel->root, FALSE);
    g_object_set_data(G_OBJECT(panel->root), "umicom-provider-editor", NULL);
    g_clear_object(&panel->root);
    UmiProviderEditorRelease(panel);
}
