/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/security/profile_keys_panel_gtk4.c
 * PURPOSE: Guide local key management through explicit intent and fresh profile verification.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "profile_keys_internal.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include <string.h>

static UmiProfileKeysGtk *FromRoot(gpointer root)
{
    return g_object_get_data(G_OBJECT(root), "umicom-profile-keys");
}
void UmiProfileKeysRelease(UmiProfileKeysGtk *panel)
{
    if (--panel->references == 0U) { umi_secret_clear(panel, sizeof(*panel)); g_free(panel); }
}
static void ClearFields(UmiProfileKeysGtk *panel)
{
    /* GtkPasswordEntry uses a password buffer. Removing its contents promptly
     * complements wiping our own job memory; never copy these values to a log. */
    gtk_editable_set_text(GTK_EDITABLE(panel->password), "");
    gtk_editable_set_text(GTK_EDITABLE(panel->confirmation), "");
    gtk_editable_set_text(GTK_EDITABLE(panel->value), "");
}
static void ResetIntent(UmiProfileKeysGtk *panel)
{
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm_create), FALSE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm_save), FALSE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->confirm_remove), FALSE);
}
static void Changed(GtkEditable *entry, gpointer root)
{
    (void)entry;
    UmiProfileKeysGtk *panel = FromRoot(root);
    if (panel != NULL && !panel->closed) ResetIntent(panel);
}
static UmiStatus CopyField(GtkWidget *entry, char *out, size_t capacity)
{
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    size_t length = 0U;
    while (length < capacity && text[length] != '\0') ++length;
    if (length == capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}
static void Action(GtkButton *button, gpointer root)
{
    UmiProfileKeysGtk *panel = FromRoot(root);
    if (panel == NULL || panel->closed || panel->busy) return;
    if (g_get_monotonic_time() < panel->retry_after) {
        ClearFields(panel); ResetIntent(panel);
        gtk_label_set_text(GTK_LABEL(panel->message), "Please wait before trying the profile password again."); return;
    }
    UmiProfileKeyOperation operation = (UmiProfileKeyOperation)GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "key-operation"));
    UmiProfileKeyJob *job = g_try_new0(UmiProfileKeyJob, 1);
    if (job == NULL) {
        ClearFields(panel); ResetIntent(panel);
        gtk_label_set_text(GTK_LABEL(panel->message), "There is not enough memory to start the operation."); return;
    }
    job->operation = operation;
    bool confirmed = operation == UMI_PROFILE_KEY_CHECK ||
        gtk_check_button_get_active(GTK_CHECK_BUTTON(operation == UMI_PROFILE_KEY_REGISTER ? panel->confirm_create :
            operation == UMI_PROFILE_KEY_SAVE ? panel->confirm_save : panel->confirm_remove));
    bool matches = strcmp(gtk_editable_get_text(GTK_EDITABLE(panel->password)),
        gtk_editable_get_text(GTK_EDITABLE(panel->confirmation))) == 0;
    UmiStatus status = CopyField(panel->password, job->password, sizeof(job->password));
    if (status == UMI_STATUS_OK && operation != UMI_PROFILE_KEY_REGISTER)
        status = CopyField(panel->alias, job->alias, sizeof(job->alias));
    if (status == UMI_STATUS_OK && operation == UMI_PROFILE_KEY_SAVE)
        status = CopyField(panel->value, job->value, sizeof(job->value));
    /* Capture intent before clearing fields: changes deliberately reset all
     * confirmations, so an earlier approval never applies to a new value. */
    ClearFields(panel); ResetIntent(panel);
    const char *problem = NULL;
    if (!confirmed) problem = "Select the confirmation for the operation you intend to perform, then enter the private values again.";
    else if (status != UMI_STATUS_OK) problem = "An input exceeds its byte limit. Passwords allow 256 bytes and keys allow 2048 bytes.";
    else if (strlen(job->password) < 12U) problem = "Enter a local profile password of 12 to 256 UTF-8 bytes.";
    else if (operation == UMI_PROFILE_KEY_REGISTER && !matches) problem = "The two passwords do not match. Enter them again.";
    else if (operation != UMI_PROFILE_KEY_REGISTER && job->alias[0] == '\0') problem = "Enter the non-secret alias for the stored key.";
    else if (operation == UMI_PROFILE_KEY_SAVE && job->value[0] == '\0') problem = "Enter the provider key you intend to save.";
    if (problem != NULL) {
        umi_secret_clear(job, sizeof(*job)); g_free(job);
        gtk_label_set_text(GTK_LABEL(panel->message), problem); return;
    }
    UmiProfileKeysStart(panel, job);
}
void UmiProfileKeysCompleted(UmiProfileKeysGtk *panel, const UmiProfileKeyJob *job)
{
    panel->busy = false;
    gtk_widget_set_sensitive(panel->form, TRUE);
    const char *message;
    if (job->status == UMI_STATUS_OK) {
        panel->failures = 0U; panel->retry_after = 0;
        switch (job->operation) {
        case UMI_PROFILE_KEY_REGISTER: message = "Local profile created. Re-enter its password when managing a key."; break;
        case UMI_PROFILE_KEY_SAVE: message = "Key saved locally. Use vault: followed by its alias as the connection reference. No connection settings were changed."; break;
        case UMI_PROFILE_KEY_CHECK: message = "The key is available in local storage. No remote login or validity check was performed."; break;
        case UMI_PROFILE_KEY_REMOVE: message = "Key removed from local storage. Saved references remain; revoke the key separately with its provider if needed."; break;
        default: message = "The local operation completed."; break;
        }
    } else if (job->status == UMI_STATUS_PERMISSION_DENIED) {
        if (panel->failures < 5U) ++panel->failures;
        panel->retry_after = g_get_monotonic_time() + (panel->failures >= 5U ? 30000000 : 1000000);
        message = panel->failures >= 5U ? "Access was denied. Wait 30 seconds before trying again." :
            "The profile password or local storage permission was not accepted. Wait a moment before trying again.";
    } else if (job->status == UMI_STATUS_ALREADY_EXISTS) message = "This local profile already exists. Use its existing password; creating a profile does not reset it.";
    else if (job->status == UMI_STATUS_NOT_FOUND) message = "The key alias was not found in this application and profile.";
    else if (job->status == UMI_STATUS_INVALID_ARGUMENT || job->status == UMI_STATUS_CAPACITY_EXCEEDED)
        message = "Check the input limits. Key aliases start with a lower-case letter and contain only lower-case letters, digits, dots, hyphens or underscores.";
    else if (job->status == UMI_STATUS_PARSE_ERROR) message = "A stored profile or key could not be read safely. No automatic repair was attempted.";
    else message = "The local credential operation could not be confirmed. Check availability before repeating a save; there is no plaintext fallback.";
    gtk_label_set_text(GTK_LABEL(panel->message), message);
}
static GtkWidget *Label(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE); gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(box), label); return label;
}
static GtkWidget *Field(UmiProfileKeysGtk *panel, const char *caption, const char *id, bool private_value)
{
    Label(panel->form, caption);
    GtkWidget *field = private_value ? gtk_password_entry_new() : gtk_entry_new();
    if (private_value) {
        gtk_password_entry_set_show_peek_icon(GTK_PASSWORD_ENTRY(field), TRUE);
        (void)UmiGtk4RecordingSetPrivate(field, 1);
    } else gtk_entry_set_max_length(GTK_ENTRY(field), 96);
    gtk_accessible_update_property(GTK_ACCESSIBLE(field), GTK_ACCESSIBLE_PROPERTY_LABEL, caption, -1);
    (void)umi_gtk4_automation_tag_widget(field, id);
    g_signal_connect_object(field, "changed", G_CALLBACK(Changed), panel->root, 0);
    gtk_box_append(GTK_BOX(panel->form), field); return field;
}
static GtkWidget *Check(const char *caption, const char *id)
{
    GtkWidget *check = gtk_check_button_new_with_label(caption);
    (void)umi_gtk4_automation_tag_widget(check, id);
    return check;
}
static void Button(UmiProfileKeysGtk *panel, const char *caption, const char *id, UmiProfileKeyOperation operation)
{
    GtkWidget *button = gtk_button_new_with_label(caption);
    (void)umi_gtk4_automation_tag_widget(button, id);
    g_object_set_data(G_OBJECT(button), "key-operation", GINT_TO_POINTER((int)operation));
    g_signal_connect_object(button, "clicked", G_CALLBACK(Action), panel->root, 0);
    gtk_box_append(GTK_BOX(panel->form), button);
}
UmiStatus UmiProfileKeysGtkCreate(const char *application_id, const char *profile_name, UmiProfileKeysGtk **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = UmiProfileSecretsScopeValidate(application_id, profile_name);
    if (status != UMI_STATUS_OK) return status;
    UmiProfileKeysGtk *panel = g_try_new0(UmiProfileKeysGtk, 1);
    if (panel == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    strcpy(panel->application, application_id); strcpy(panel->profile, profile_name);
    panel->references = 1U; panel->open_service = UmiProfileSecretsPlatform;
    panel->root = gtk_scrolled_window_new(); g_object_ref_sink(panel->root);
    g_object_set_data(G_OBJECT(panel->root), "umicom-profile-keys", panel);
    (void)umi_gtk4_automation_tag_widget(panel->root, "keys.panel");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(box, 16); gtk_widget_set_margin_bottom(box, 16);
    gtk_widget_set_margin_start(box, 16); gtk_widget_set_margin_end(box, 16);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(panel->root), box);
    Label(box, "Local provider keys");
    char scope[192];
    g_snprintf(scope, sizeof(scope), "Application: %s  |  Local profile: %s", application_id, profile_name);
    Label(box, scope);
    Label(box, "Windows stores keys in Credential Manager for this OS user. Your local profile password is checked for each operation; it does not encrypt the vault or protect it from other programs running as you.");
    Label(box, "Create this local profile only if it does not exist. Use a separate local password, not your provider account password. Closing clears unsaved private values.");
    panel->form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6); gtk_box_append(GTK_BOX(box), panel->form);
    /* Build confirmation controls before fields can emit their changed signal. */
    panel->confirm_create = Check("Create this new local profile", "keys.confirm-create");
    panel->confirm_save = Check("Save or replace the key at this alias", "keys.confirm-save");
    panel->confirm_remove = Check("Remove this local key; keep its saved connection references", "keys.confirm-remove");
    panel->password = Field(panel, "Local profile password", "keys.password", true);
    panel->confirmation = Field(panel, "Repeat password only when creating a profile", "keys.password-confirm", true);
    panel->alias = Field(panel, "Key alias, for example personal-key", "keys.alias", false);
    panel->value = Field(panel, "Provider API key, only when saving or replacing", "keys.value", true);
    Label(panel->form, "Enter the values first, then select the operation's confirmation. A later edit clears confirmation. The connection reference is vault: followed by the alias; do not paste the key into connection settings.");
    gtk_box_append(GTK_BOX(panel->form), panel->confirm_create);
    Button(panel, "Create local profile", "keys.create", UMI_PROFILE_KEY_REGISTER);
    gtk_box_append(GTK_BOX(panel->form), panel->confirm_save);
    Button(panel, "Save or replace key", "keys.save", UMI_PROFILE_KEY_SAVE);
    Button(panel, "Check local key availability", "keys.check", UMI_PROFILE_KEY_CHECK);
    gtk_box_append(GTK_BOX(panel->form), panel->confirm_remove);
    Button(panel, "Remove local key", "keys.remove", UMI_PROFILE_KEY_REMOVE);
    panel->message = Label(box, "No credential store has been opened. These actions do not contact a remote provider.");
    (void)umi_gtk4_automation_tag_widget(panel->message, "keys.message");
    *out = panel; return UMI_STATUS_OK;
}
GtkWidget *UmiProfileKeysGtkWidget(UmiProfileKeysGtk *panel)
{
    return panel != NULL && !panel->closed ? panel->root : NULL;
}
bool UmiProfileKeysGtkBusy(const UmiProfileKeysGtk *panel)
{
    return panel != NULL && !panel->closed && panel->busy;
}
bool UmiProfileKeysGtkCanClose(UmiProfileKeysGtk *panel)
{
    if (panel == NULL || panel->closed || !panel->busy) return true;
    gtk_label_set_text(GTK_LABEL(panel->message), "Wait for the credential operation to finish before closing.");
    return false;
}
void UmiProfileKeysGtkDestroy(UmiProfileKeysGtk *panel)
{
    if (panel == NULL || panel->closed) return;
    panel->closed = true;
    ClearFields(panel); ResetIntent(panel);
    gtk_widget_set_sensitive(panel->root, FALSE);
    g_object_set_data(G_OBJECT(panel->root), "umicom-profile-keys", NULL);
    g_clear_object(&panel->root);
    UmiProfileKeysRelease(panel);
}
