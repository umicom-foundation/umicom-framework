/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/structured_forms/authorized_native.c
 *
 * PURPOSE:
 *   Fictional local example of host-supplied identity and protected native storage.
 *   Bootstrap grants below demonstrate composition; they are not a login system.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Fictional local example of host-supplied identity and protected native storage.
 * Bootstrap grants below demonstrate composition; they are not a login system. */
#include "umicom/ui/gtk4/authorized_form_editor.h"
#include <stdio.h>
static const UmiApplicationExperienceField fields[] = {
    {.field_id = "parcel.mass", .label = "Parcel mass",
     .help_text = "Fictional parcel mass in kilograms.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_DECIMAL,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL,
     .required = 1, .enforce_number_range = 1, .minimum_number = 0, .maximum_number = 100},
    {.field_id = "parcel.note", .label = "Review note", .help_text = "Use fictional data only.",
     .kind = UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
     .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT}
};
static const UmiStructuredFormField rules[] = {
    {"parcel.mass", "mass.kg", UMI_FORM_TEXT_PLAIN, 1, 0},
    {"parcel.note", "", UMI_FORM_TEXT_PLAIN, 1, 1}
};
static const UmiApplicationExperienceFormSchema base = {
    sizeof(base), UMI_APPLICATION_EXPERIENCE_API_VERSION, "example.parcel",
    "Fictional parcel review", "Temporary local example records", "example.save", fields, 2
};
static const UmiStructuredFormSchema schema = {&base, 1, rules, 2};
static gboolean close_requested(GtkWindow *window, gpointer data) {
    (void)window;
    return !UmiAuthorizedFormEditorGtk4CanClose(GTK_WIDGET(data));
}
static void activate(GtkApplication *application, gpointer data) {
    (void)data;
    GtkWindow *existing = gtk_application_get_active_window(application);
    if (existing) { gtk_window_present(existing); return; }
    UmiRecordHostOptions options = {.backend = UMI_RECORD_HOST_MEMORY,
        .source = "Fictional native example", .entity_prefix = "example."};
    UmiRecordHost *host = NULL;
    UmiStatus s = umi_record_host_create(&options, &host);
    UmiIdentityRecord identity = {.identity_id = "example.editor",
        .display_name = "Fictional local editor", .kind = UMI_IDENTITY_HUMAN, .enabled = 1};
    if (s == UMI_STATUS_OK) s = umi_record_host_register_identity(host, &identity);
    UmiPolicyRule policy[] = {
        {"example.editor", "record.*", "example.*", UMI_POLICY_ALLOW},
        {"example.editor", "record.purpose", "purpose/review/record/example.*", UMI_POLICY_ALLOW}
    };
    uint64_t policy_revision;
    if (s == UMI_STATUS_OK) s = umi_record_host_replace_policy(host, policy, 2, &policy_revision);
    UmiSessionRecord session = {0};
    if (s == UMI_STATUS_OK) s = umi_record_host_issue_current_session(host, identity.identity_id,
        UINT64_C(3600000000000), &session);
    GtkWidget *editor = NULL;
    UmiAuthorizedFormEditorGtk4Options editor_options = {host, &schema, session.session_id, "review"};
    if (s == UMI_STATUS_OK) s = UmiAuthorizedFormEditorGtk4New(&editor_options, &editor);
    umi_record_host_release(host);
    if (s != UMI_STATUS_OK) {
        fprintf(stderr, "Unable to open fictional form example: %s\n", umi_status_text(s)); return;
    }
    GtkWidget *window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(window), "Authorized forms — fictional local example");
    gtk_window_set_default_size(GTK_WINDOW(window), 760, 740);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *notice = gtk_label_new(
        "Fictional data only. Records last until this window closes. Enter an identifier "
        "such as example.parcel, choose New draft, edit answers, add a reason and Save draft. "
        "Close waits for saves and requires saved or explicitly discarded edits.");
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE); gtk_box_append(GTK_BOX(box), notice);
    GtkWidget *scroll = gtk_scrolled_window_new(); gtk_widget_set_vexpand(scroll, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), editor);
    gtk_box_append(GTK_BOX(box), scroll); gtk_window_set_child(GTK_WINDOW(window), box);
    g_signal_connect_object(window, "close-request", G_CALLBACK(close_requested), editor, 0);
    gtk_window_present(GTK_WINDOW(window));
}
int main(int argc, char **argv) {
    GtkApplication *application = gtk_application_new("org.umicom.example.authorizedforms", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(application, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application); return status;
}
