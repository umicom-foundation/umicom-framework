/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_dialog_gtk4.c
 *
 * PURPOSE:
 *   Collect settings with GTK and delegate generation to Framework services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/developer_dialog.h"
#include "umicom/base/text.h"
#include "umicom/ui/gtk4/automation.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define UMI_DEVELOPER_FORM_FIELDS 11U
struct UmiGtk4DeveloperDialog {
    GtkWindow *window;
    GtkWidget *body;
    GtkWidget *message;
    GtkWidget *fields[UMI_DEVELOPER_FORM_FIELDS];
    GtkWidget *templatePicker;
    GtkWidget *trust;
    GtkWidget *parallel;
    const UmiDeveloperProjectTemplate *templates[UMI_DEVELOPER_PROJECT_TEMPLATE_CAPACITY];
    size_t templateCount;
    UmiDeveloperProjectService *projects;
    UmiDeveloperProjectModel model;
    UmiBuildProfile profile;
    UmiGtk4ProjectCreated onCreated;
    UmiGtk4BuildSettingsApplied onApplied;
    void *context;
    int generated;
};

typedef struct UmiDeveloperProfileField {
    const char *title;
    size_t offset;
    size_t capacity;
} UmiDeveloperProfileField;
#define UMI_PROFILE_FIELD(title, field) {title, offsetof(UmiBuildProfile, field), sizeof(((UmiBuildProfile *)0)->field)}
static const UmiDeveloperProfileField PROFILE_FIELDS[] = {
    UMI_PROFILE_FIELD("Profile name", profile_id),
    UMI_PROFILE_FIELD("Project folder", source_directory),
    UMI_PROFILE_FIELD("Build folder (relative to project)", build_directory),
    UMI_PROFILE_FIELD("Generator", generator),
    UMI_PROFILE_FIELD("C compiler (blank uses CMake default)", compiler),
    UMI_PROFILE_FIELD("Configuration", configuration),
    UMI_PROFILE_FIELD("CMake preset (blank uses the fields above)", preset),
    UMI_PROFILE_FIELD("Build target (blank builds all)", build_target),
    UMI_PROFILE_FIELD("Program to run (path relative to project)", run_program),
    UMI_PROFILE_FIELD("Program argument (one argument; no shell expansion)", run_argument),
    UMI_PROFILE_FIELD("Install folder (relative to project)", install_directory)
};
#undef UMI_PROFILE_FIELD

/* Labels and entries are kept together so keyboard navigation follows reading order. */
static GtkWidget *AddEntry(UmiGtk4DeveloperDialog *dialog, const char *title,
    const char *value, const char *id)
{
    GtkWidget *label = gtk_label_new(title);
    GtkWidget *entry = gtk_entry_new();
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_editable_set_text(GTK_EDITABLE(entry), value != NULL ? value : "");
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_widget_set_tooltip_text(entry, title);
    (void)umi_gtk4_automation_tag_widget(entry, id);
    gtk_box_append(GTK_BOX(dialog->body), label);
    gtk_box_append(GTK_BOX(dialog->body), entry);
    return entry;
}

/* Closing the form does not release the owner's handle or callback context. */
static gboolean HideOnClose(GtkWindow *window, gpointer context)
{
    (void)context;
    gtk_widget_set_visible(GTK_WIDGET(window), FALSE);
    return TRUE;
}

/* Cancel has no filesystem, build-profile or trust side effects. */
static void CancelClicked(GtkButton *button, gpointer context)
{
    UmiGtk4DeveloperDialog *dialog = context;
    (void)button;
    gtk_widget_set_visible(GTK_WIDGET(dialog->window), FALSE);
}

/* Report a rejected input in the form rather than close it and lose the fields. */
static void ShowStatus(UmiGtk4DeveloperDialog *dialog, UmiStatus status, const char *detail)
{
    gtk_label_set_text(GTK_LABEL(dialog->message), detail != NULL && detail[0] != '\0'
        ? detail : umi_status_text(status));
}

/* Generate once. A failed adoption can be retried without generating over its files. */
static void CreateClicked(GtkButton *button, gpointer context)
{
    UmiGtk4DeveloperDialog *dialog = context;
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectGeneratorReport report;
    char message[512] = {0};
    UmiStatus status = UMI_STATUS_OK;
    (void)button;
    if (!dialog->generated) {
        guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(dialog->templatePicker));
        if ((size_t)selected >= dialog->templateCount) return;
        umi_developer_project_generation_request_init(&request);
        status = umi_text_copy(request.template_id, sizeof(request.template_id),
            dialog->templates[selected]->template_id);
#define COPY_INPUT(field, index) \
        if (status == UMI_STATUS_OK) status = umi_text_copy(request.field, sizeof(request.field), \
            gtk_editable_get_text(GTK_EDITABLE(dialog->fields[index])))
        COPY_INPUT(application_name, 0);
        COPY_INPUT(application_id, 1);
        COPY_INPUT(repository_name, 2);
        COPY_INPUT(target_name, 3);
        COPY_INPUT(project_root, 4);
#undef COPY_INPUT
        if (status == UMI_STATUS_OK)
            status = umi_developer_project_generation_request_validate(&request, message, sizeof(message));
        if (status == UMI_STATUS_OK) {
            message[0] = '\0';
            status = UmiDeveloperProjectCreateNew(dialog->projects, &request,
                &report, &dialog->model, &dialog->profile);
        }
        if (status != UMI_STATUS_OK) {
            ShowStatus(dialog, status, status == UMI_STATUS_ALREADY_EXISTS
                ? "That folder already exists. Choose a new project folder; your existing files have not been replaced."
                : message);
            return;
        }
        dialog->generated = 1;
        for (size_t index = 0U; index < 5U; ++index)
            gtk_widget_set_sensitive(dialog->fields[index], FALSE);
        gtk_widget_set_sensitive(dialog->templatePicker, FALSE);
    }
    status = dialog->onCreated(&dialog->model, &dialog->profile, dialog->context);
    if (status == UMI_STATUS_OK) gtk_widget_set_visible(GTK_WIDGET(dialog->window), FALSE);
    else ShowStatus(dialog, status, "The files were created but Studio could not open the project. Close this form and use Open Folder. No existing project was overwritten.");
}

/* Settings are copied first and only applied after validation and owner acceptance. */
static void ApplyClicked(GtkButton *button, gpointer context)
{
    UmiGtk4DeveloperDialog *dialog = context;
    UmiBuildProfile profile = dialog->profile;
    UmiStatus status = UMI_STATUS_OK;
    char message[512] = {0};
    (void)button;
    for (size_t index = 0U; index < UMI_DEVELOPER_FORM_FIELDS; ++index) {
        const UmiDeveloperProfileField *field = &PROFILE_FIELDS[index];
        status = umi_text_copy((char *)&profile + field->offset, field->capacity,
            gtk_editable_get_text(GTK_EDITABLE(dialog->fields[index])));
        if (status != UMI_STATUS_OK) break;
    }
    profile.parallel_jobs = (unsigned)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dialog->parallel));
    if (status == UMI_STATUS_OK) status = umi_build_profile_validate(&profile, message, sizeof(message));
    if (status == UMI_STATUS_OK) {
        message[0] = '\0';
        status = dialog->onApplied(&profile,
            gtk_check_button_get_active(GTK_CHECK_BUTTON(dialog->trust)), dialog->context);
    }
    if (status == UMI_STATUS_OK) {
        dialog->profile = profile;
        gtk_widget_set_visible(GTK_WIDGET(dialog->window), FALSE);
    } else ShowStatus(dialog, status, message);
}

/* Construct a modal form with scrolling, so smaller displays retain the buttons. */
static UmiStatus CreateForm(GtkWindow *parent, const char *title,
    const char *instructions, GCallback accepted, const char *acceptTitle,
    UmiGtk4DeveloperDialog **outDialog)
{
    UmiGtk4DeveloperDialog *dialog;
    GtkWidget *root;
    GtkWidget *scroll;
    GtkWidget *label;
    GtkWidget *buttons;
    GtkWidget *cancel;
    GtkWidget *accept;
    if (parent == NULL || outDialog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDialog = NULL;
    dialog = calloc(1U, sizeof(*dialog));
    if (dialog == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    dialog->window = GTK_WINDOW(gtk_window_new());
    /* Retain the GTK object until the owner explicitly destroys this handle. */
    g_object_ref(dialog->window);
    gtk_window_set_title(dialog->window, title);
    gtk_window_set_transient_for(dialog->window, parent);
    gtk_window_set_destroy_with_parent(dialog->window, TRUE);
    gtk_window_set_modal(dialog->window, TRUE);
    gtk_window_set_default_size(dialog->window, 700, 640);
    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(root, 18);
    gtk_widget_set_margin_end(root, 18);
    gtk_widget_set_margin_top(root, 18);
    gtk_widget_set_margin_bottom(root, 18);
    label = gtk_label_new(instructions);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(root), label);
    dialog->body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), dialog->body);
    gtk_box_append(GTK_BOX(root), scroll);
    dialog->message = gtk_label_new("");
    gtk_label_set_wrap(GTK_LABEL(dialog->message), TRUE);
    gtk_label_set_xalign(GTK_LABEL(dialog->message), 0.0F);
    gtk_box_append(GTK_BOX(root), dialog->message);
    buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    cancel = gtk_button_new_with_label("Cancel");
    accept = gtk_button_new_with_label(acceptTitle);
    g_signal_connect(cancel, "clicked", G_CALLBACK(CancelClicked), dialog);
    g_signal_connect(accept, "clicked", accepted, dialog);
    (void)umi_gtk4_automation_tag_widget(accept, "developer.dialog.accept");
    (void)umi_gtk4_automation_tag_widget(cancel, "developer.dialog.cancel");
    gtk_box_append(GTK_BOX(buttons), cancel);
    gtk_box_append(GTK_BOX(buttons), accept);
    gtk_box_append(GTK_BOX(root), buttons);
    gtk_window_set_child(dialog->window, root);
    g_signal_connect(dialog->window, "close-request", G_CALLBACK(HideOnClose), dialog);
    *outDialog = dialog;
    return UMI_STATUS_OK;
}

/* Template metadata and file recipes come from the existing Framework catalogue. */
UmiStatus UmiGtk4NewProjectDialogCreate(GtkWindow *parent,
    UmiGtk4ProjectCreated onCreated, void *context, UmiGtk4DeveloperDialog **outDialog)
{
    UmiGtk4DeveloperDialog *dialog = NULL;
    const char *titles[UMI_DEVELOPER_PROJECT_TEMPLATE_CAPACITY + 1U] = {0};
    char *destination;
    UmiStatus status;
    if (onCreated == NULL || outDialog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDialog = NULL;
    status = CreateForm(parent, "New Umicom Project",
        "Choose a CMake starter and a new folder. Studio will create source files, build settings and a README. It will not download dependencies or run code.",
        G_CALLBACK(CreateClicked), "Create Project", &dialog);
    if (status != UMI_STATUS_OK) return status;
    status = umi_developer_project_service_create(&dialog->projects);
    if (status != UMI_STATUS_OK) { UmiGtk4DeveloperDialogDestroy(dialog); return status; }
    dialog->onCreated = onCreated;
    dialog->context = context;
    for (size_t index = 0U; index < umi_developer_project_builtin_template_count(); ++index) {
        const UmiDeveloperProjectTemplate *recipe = umi_developer_project_builtin_template_at(index);
        if (recipe->build_system != UMI_DEVELOPER_PROJECT_BUILD_CMAKE) continue;
        if (dialog->templateCount >= UMI_DEVELOPER_PROJECT_TEMPLATE_CAPACITY) break;
        titles[dialog->templateCount] = recipe->title;
        dialog->templates[dialog->templateCount++] = recipe;
    }
    dialog->templatePicker = gtk_drop_down_new_from_strings(titles);
    gtk_widget_set_tooltip_text(dialog->templatePicker, "CMake project templates supplied by Umicom Framework");
    (void)umi_gtk4_automation_tag_widget(dialog->templatePicker, "developer.project.template");
    gtk_box_append(GTK_BOX(dialog->body), dialog->templatePicker);
    dialog->fields[0] = AddEntry(dialog, "Application name", "Umicom Notes", "developer.project.name");
    dialog->fields[1] = AddEntry(dialog, "Application identifier", "org.umicom.notes", "developer.project.id");
    dialog->fields[2] = AddEntry(dialog, "Repository name", "umicom-notes", "developer.project.repository");
    dialog->fields[3] = AddEntry(dialog, "CMake target", "umicom_notes", "developer.project.target");
    destination = g_build_filename(g_get_home_dir(), "umicom-projects", "UmicomNotes", NULL);
    dialog->fields[4] = AddEntry(dialog, "New project folder (full path)", destination, "developer.project.folder");
    g_free(destination);
    *outDialog = dialog;
    gtk_window_present(dialog->window);
    return UMI_STATUS_OK;
}

/* This form edits a value copy; the application owns the eventual trust decision. */
UmiStatus UmiGtk4BuildSettingsDialogCreate(GtkWindow *parent,
    const UmiBuildProfile *profile, int trusted, UmiGtk4BuildSettingsApplied onApplied,
    void *context, UmiGtk4DeveloperDialog **outDialog)
{
    UmiGtk4DeveloperDialog *dialog;
    UmiStatus status;
    if (onApplied == NULL || outDialog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDialog = NULL;
    status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    status = CreateForm(parent, "Project Build Settings",
        "Review the build folder, program and install folder. A CMake preset controls configuration when its field is filled. Building a project can execute its scripts: trust only code you have reviewed.",
        G_CALLBACK(ApplyClicked), "Apply Settings", &dialog);
    if (status != UMI_STATUS_OK) return status;
    dialog->profile = *profile;
    dialog->onApplied = onApplied;
    dialog->context = context;
    for (size_t index = 0U; index < UMI_DEVELOPER_FORM_FIELDS; ++index) {
        char id[80];
        const UmiDeveloperProfileField *field = &PROFILE_FIELDS[index];
        g_snprintf(id, sizeof(id), "developer.build.field.%zu", index);
        dialog->fields[index] = AddEntry(dialog, field->title,
            (const char *)profile + field->offset, id);
    }
    gtk_editable_set_editable(GTK_EDITABLE(dialog->fields[1]), FALSE);
    dialog->parallel = gtk_spin_button_new_with_range(1.0, 32.0, 1.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(dialog->parallel), (double)profile->parallel_jobs);
    gtk_widget_set_tooltip_text(dialog->parallel, "Maximum parallel build jobs");
    gtk_box_append(GTK_BOX(dialog->body), gtk_label_new("Parallel build jobs"));
    gtk_box_append(GTK_BOX(dialog->body), dialog->parallel);
    dialog->trust = gtk_check_button_new_with_label("I trust the code and build scripts in this project folder");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(dialog->trust), trusted != 0);
    (void)umi_gtk4_automation_tag_widget(dialog->trust, "developer.build.trusted");
    gtk_box_append(GTK_BOX(dialog->body), dialog->trust);
    *outDialog = dialog;
    gtk_window_present(dialog->window);
    return UMI_STATUS_OK;
}

/* Drop callbacks and destroy the transient window before the owner disappears. */
void UmiGtk4DeveloperDialogDestroy(UmiGtk4DeveloperDialog *dialog)
{
    if (dialog == NULL) return;
    if (dialog->window != NULL) {
        gtk_window_destroy(dialog->window);
        g_object_unref(dialog->window);
    }
    umi_developer_project_service_destroy(dialog->projects);
    free(dialog);
}
