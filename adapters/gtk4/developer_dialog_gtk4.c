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
#include "umicom/developer_project/preset_catalogue.h"
#include "umicom/developer_project/target_catalogue.h"
#include "umicom/developer_project/installed_files.h"
#include "umicom/developer_project/build_cache.h"
#include "umicom/build/launch_plan.h"
#include "umicom/platform/path.h"
#include "umicom/platform/output_file.h"
#include "umicom/base/text.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/filtered_choices.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define UMI_DEVELOPER_FORM_FIELDS 11U
struct UmiGtk4DeveloperDialog {
    GtkWindow *window;
    GtkWidget *body;
    GtkWidget *root;
    GPtrArray *signalObjects;
    unsigned references;
    int closed;
    int accepting;
    GtkWidget *message;
    GtkWidget *fields[UMI_DEVELOPER_FORM_FIELDS];
    GtkWidget *templatePicker;
    GtkWidget *trust;
    GtkWidget *parallel;
    GtkWidget *arguments;
    GtkWidget *runDirectory;
    GtkWidget *configurePreset, *buildPreset, *testPreset;
    GtkWidget *presetPicker, *presetRead, *presetUse, *presetDetail;
    UmiProjectPresetCatalogue *presetCatalogue;
    char presetRoot[UMI_BUILD_PATH_CAPACITY];
    int presetLoading, presetChanging;
    GtkWidget *presetReadIncludes;
    UmiCancellationToken *presetCancel; /* Borrowed until the worker completes. */
    int presetInputsChanged;
    GtkWidget *targetRead, *targetPicker, *targetBuild, *targetProgram, *targetDetail;
    GtkWidget *targetRequest;
    GtkWidget *targetFilter; /* Its owned projection maps visible rows back to the catalogue. */
    GtkWidget *cacheRead;
    UmiProjectTargetCatalogue *targetCatalogue;
    int targetLoading, targetChanging, targetInputsChanged;
    UmiCancellationToken *targetCancel; /* Borrowed from the active read task. */
    /* Shared install review owns its snapshot until the worker and dialog release it. */
    GtkWidget *installedRead, *installedPicker, *installedProgram, *installedDetail;
    /* The shared filter owns copied labels; installedCatalogue owns file evidence. */
    GtkWidget *installedFilter;
    UmiProjectInstalledFiles *installedCatalogue;
    UmiCancellationToken *installedCancel; /* Borrowed from the active worker. */
    int installedLoading, installedChanging, installedInvalidated;
    GtkWidget *chooseProgram, *chooseWorkingDirectory, *reviewLaunch, *launchDetail;
    GCancellable *launchChooserCancel;
    int launchChoosing, launchChanging, launchInvalidated;
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

/* A callback temporarily owns the controller. The application can destroy its
 * handle from an acceptance callback or GTK notification; child controls remain
 * alive until that callback returns, and no later notification uses its context.
 * This is GTK-thread ownership, not a cross-thread reference-counting API. */
static void DeveloperRelease(UmiGtk4DeveloperDialog *dialog)
{
    if (--dialog->references != 0U) return;
    umi_developer_project_service_destroy(dialog->projects);
    UmiProjectPresetCatalogueDestroy(dialog->presetCatalogue);
    UmiProjectTargetCatalogueDestroy(dialog->targetCatalogue);
    UmiProjectInstalledFilesDestroy(dialog->installedCatalogue);
    g_clear_object(&dialog->launchChooserCancel);
    g_clear_pointer(&dialog->signalObjects, g_ptr_array_unref);
    g_clear_object(&dialog->root);
    g_clear_object(&dialog->window);
    free(dialog);
}
static int DeveloperVisible(const UmiGtk4DeveloperDialog *dialog)
{
    return dialog != NULL && !dialog->closed &&
        gtk_widget_get_visible(GTK_WIDGET(dialog->window)) &&
        gtk_window_get_child(dialog->window) == dialog->root;
}
static UmiGtk4DeveloperDialog *DeveloperAcquire(UmiGtk4DeveloperDialog *dialog)
{
    if (!DeveloperVisible(dialog)) return NULL;
    ++dialog->references;
    return dialog;
}
static void DeveloperConnect(UmiGtk4DeveloperDialog *dialog, GObject *object,
    const char *signal, GCallback callback)
{
    g_signal_connect(object, signal, callback, dialog);
    g_ptr_array_add(dialog->signalObjects, g_object_ref(object));
}

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
/* Closing now retains the controller across visibility notifications; the owner still disposes the handle.
 * The earlier implementation is retained for engineering review. */
#if 0
static gboolean HideOnClose(GtkWindow *window, gpointer context)
{
    (void)context;
    gtk_widget_set_visible(GTK_WIDGET(window), FALSE);
    return TRUE;
}
#endif
static gboolean HideOnClose(GtkWindow *window, gpointer context)
{
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return TRUE;
    gtk_widget_set_visible(GTK_WIDGET(window), FALSE);
    DeveloperRelease(dialog);
    return TRUE;
}

/* Cancel has no filesystem, build-profile or trust side effects. */
/* Cancel now owns its callback lifetime so synchronous window teardown cannot release active state.
 * The earlier implementation is retained for engineering review. */
#if 0
static void CancelClicked(GtkButton *button, gpointer context)
{
    UmiGtk4DeveloperDialog *dialog = context;
    (void)button;
    gtk_widget_set_visible(GTK_WIDGET(dialog->window), FALSE);
}
#endif
static void CancelClicked(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return;
    gtk_widget_set_visible(GTK_WIDGET(dialog->window), FALSE);
    DeveloperRelease(dialog);
}

/* Report a rejected input in the form rather than close it and lose the fields. */
static void ShowStatus(UmiGtk4DeveloperDialog *dialog, UmiStatus status, const char *detail)
{
    gtk_label_set_text(GTK_LABEL(dialog->message), detail != NULL && detail[0] != '\0'
        ? detail : umi_status_text(status));
}

/* Discovery shares the same controller lifetime as the rest of the form. */
#include "developer_presets_gtk4.inc"
#include "developer_launch_gtk4.inc"
#include "developer_targets_gtk4.inc"
#include "developer_installed_gtk4.inc"

/* Generate once. A failed adoption can be retried without generating over its files. */
/* Acceptance now holds owned form state and checks visibility after callbacks; this preserves settings and project generation while preventing use of a retired application context.
 * The earlier implementation is retained for engineering review. */
#if 0
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
#endif
static void CreateClicked(GtkButton *button, gpointer context)
{
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return;
    /* An application callback can dispatch another GTK event. Do not accept
     * the same form recursively while its current result is being adopted. */
    if (dialog->accepting) { DeveloperRelease(dialog); return; }
    dialog->accepting = 1;
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectGeneratorReport report;
    char message[512] = {0};
    UmiStatus status = UMI_STATUS_OK;
    (void)button;
    if (!dialog->generated) {
        guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(dialog->templatePicker));
        if ((size_t)selected >= dialog->templateCount) goto done;
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
            goto done;
        }
        dialog->generated = 1;
        for (size_t index = 0U; index < 5U && !dialog->closed; ++index)
            gtk_widget_set_sensitive(dialog->fields[index], FALSE);
        if (!dialog->closed) gtk_widget_set_sensitive(dialog->templatePicker, FALSE);
    }
    if (!DeveloperVisible(dialog)) goto done;
    status = dialog->onCreated(&dialog->model, &dialog->profile, dialog->context);
    if (!DeveloperVisible(dialog)) goto done;
    if (status == UMI_STATUS_OK) gtk_widget_set_visible(GTK_WIDGET(dialog->window), FALSE);
    else ShowStatus(dialog, status, "The files were created but Studio could not open the project. Close this form and use Open Folder. No existing project was overwritten.");
done:
    dialog->accepting = 0;
    DeveloperRelease(dialog);
}

/* Settings are copied first and only applied after validation and owner acceptance. */
/* Acceptance now holds owned form state and checks visibility after callbacks; this preserves settings and project generation while preventing use of a retired application context.
 * The earlier implementation is retained for engineering review. */
#if 0
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
    /* Preserve the existing literal field. Users opt into a list explicitly;
     * validation reports a conflict instead of silently dropping either input. */
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(profile.run_arguments, sizeof(profile.run_arguments),
            gtk_editable_get_text(GTK_EDITABLE(dialog->arguments)));
    /* The shared profile owns stage semantics and persistence. Native controls
     * only copy user choices; opening or applying settings starts no process. */
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(profile.configure_preset, sizeof(profile.configure_preset),
            gtk_editable_get_text(GTK_EDITABLE(dialog->configurePreset)));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(profile.build_preset, sizeof(profile.build_preset),
            gtk_editable_get_text(GTK_EDITABLE(dialog->buildPreset)));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(profile.test_preset, sizeof(profile.test_preset),
            gtk_editable_get_text(GTK_EDITABLE(dialog->testPreset)));
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
#endif
static void ApplyClicked(GtkButton *button, gpointer context)
{
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return;
    /* An application callback can dispatch another GTK event. Do not accept
     * the same form recursively while its current result is being adopted. */
    /* Selecting a name only edits the form. An extension's synchronous entry
     * notification must not turn that edit into an acceptance callback. */
    /* A discovery update must not recursively accept or replace visible settings. */
    /* Cross-form operations wait until install review finishes publishing its result. */
    if (dialog->installedLoading || dialog->installedChanging) { DeveloperRelease(dialog); return; }
    /* Profile acceptance cannot observe an installed-file mapping halfway
     * through publication, even when a native notification reenters this form. */
    size_t installedSource = 0U;
    if (dialog->installedFilter != NULL &&
        UmiGtk4FilteredChoicesSelectedSource(dialog->installedFilter, &installedSource) == UMI_STATUS_BUSY)
    { DeveloperRelease(dialog); return; }
    if (dialog->targetLoading || dialog->targetChanging) { DeveloperRelease(dialog); return; }
    /* Native model publication can synchronously notify application extensions.
     * Applying the form waits until its source-index mapping is complete. */
    size_t sourceIndex = 0U;
    if (dialog->targetFilter != NULL && UmiGtk4FilteredChoicesSelectedSource(dialog->targetFilter, &sourceIndex) == UMI_STATUS_BUSY)
    { DeveloperRelease(dialog); return; }
    if (dialog->launchChoosing || dialog->launchChanging) { DeveloperRelease(dialog); return; }
    if (dialog->presetLoading || dialog->presetChanging) { DeveloperRelease(dialog); return; }
    if (dialog->accepting) { DeveloperRelease(dialog); return; }
    dialog->accepting = 1;
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
    /* Preserve the existing literal field. Users opt into a list explicitly;
     * validation reports a conflict instead of silently dropping either input. */
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(profile.run_arguments, sizeof(profile.run_arguments),
            gtk_editable_get_text(GTK_EDITABLE(dialog->arguments)));
    /* The shared profile owns stage semantics and persistence. Native controls
     * only copy user choices; opening or applying settings starts no process. */
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(profile.configure_preset, sizeof(profile.configure_preset),
            gtk_editable_get_text(GTK_EDITABLE(dialog->configurePreset)));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(profile.build_preset, sizeof(profile.build_preset),
            gtk_editable_get_text(GTK_EDITABLE(dialog->buildPreset)));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(profile.test_preset, sizeof(profile.test_preset),
            gtk_editable_get_text(GTK_EDITABLE(dialog->testPreset)));
    /* Saving the launch folder does not start the program or create folders. */
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(profile.run_working_directory, sizeof(profile.run_working_directory),
            gtk_editable_get_text(GTK_EDITABLE(dialog->runDirectory)));
    profile.parallel_jobs = (unsigned)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dialog->parallel));
    if (status == UMI_STATUS_OK) status = umi_build_profile_validate(&profile, message, sizeof(message));
    if (status == UMI_STATUS_OK) {
        message[0] = '\0';
        status = dialog->onApplied(&profile,
            gtk_check_button_get_active(GTK_CHECK_BUTTON(dialog->trust)), dialog->context);
    }
    if (!DeveloperVisible(dialog)) goto done;
    if (status == UMI_STATUS_OK) {
        dialog->profile = profile;
        gtk_widget_set_visible(GTK_WIDGET(dialog->window), FALSE);
    } else ShowStatus(dialog, status, message);
done:
    dialog->accepting = 0;
    DeveloperRelease(dialog);
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
    dialog->references = 1U;
    dialog->signalObjects = g_ptr_array_new_with_free_func(g_object_unref);
    dialog->window = GTK_WINDOW(gtk_window_new());
    /* Retain the GTK object until the owner explicitly destroys this handle. */
    g_object_ref(dialog->window);
    gtk_window_set_title(dialog->window, title);
    gtk_window_set_transient_for(dialog->window, parent);
    gtk_window_set_destroy_with_parent(dialog->window, TRUE);
    gtk_window_set_modal(dialog->window, TRUE);
    gtk_window_set_default_size(dialog->window, 700, 640);
    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    dialog->root = g_object_ref_sink(root);
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
    /* Owned signal registrations replace the raw callback below; it is kept
     * for review of the previous lifetime contract. */
#if 0
    g_signal_connect(cancel, "clicked", G_CALLBACK(CancelClicked), dialog);
#endif
    DeveloperConnect(dialog, G_OBJECT(cancel), "clicked", G_CALLBACK(CancelClicked));
    /* Owned signal registrations replace the raw callback below; it is kept
     * for review of the previous lifetime contract. */
#if 0
    g_signal_connect(accept, "clicked", accepted, dialog);
#endif
    DeveloperConnect(dialog, G_OBJECT(accept), "clicked", accepted);
    (void)umi_gtk4_automation_tag_widget(accept, "developer.dialog.accept");
    (void)umi_gtk4_automation_tag_widget(cancel, "developer.dialog.cancel");
    gtk_box_append(GTK_BOX(buttons), cancel);
    gtk_box_append(GTK_BOX(buttons), accept);
    gtk_box_append(GTK_BOX(root), buttons);
    gtk_window_set_child(dialog->window, root);
    /* Owned signal registrations replace the raw callback below; it is kept
     * for review of the previous lifetime contract. */
#if 0
    g_signal_connect(dialog->window, "close-request", G_CALLBACK(HideOnClose), dialog);
#endif
    DeveloperConnect(dialog, G_OBJECT(dialog->window), "close-request", G_CALLBACK(HideOnClose));
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
    dialog->configurePreset = AddEntry(dialog, "Configure preset (separate stage)",
        profile->configure_preset, "developer.build.configure-preset");
    dialog->buildPreset = AddEntry(dialog, "Build preset (separate stage)",
        profile->build_preset, "developer.build.build-preset");
    dialog->testPreset = AddEntry(dialog, "Test preset (separate stage)",
        profile->test_preset, "developer.build.test-preset");
    GtkWidget *presetHelp = gtk_label_new(
        "Clear the shared CMake preset above when using separate stage names. A blank stage uses the ordinary profile fields. Explicit presets control their own options. Set the build folder to the preset's binary directory for diagnostics, install, package and debugger paths.");
    gtk_label_set_wrap(GTK_LABEL(presetHelp), TRUE);
    gtk_label_set_xalign(GTK_LABEL(presetHelp), 0.0F);
    gtk_box_append(GTK_BOX(dialog->body), presetHelp);
    DeveloperPresetControls(dialog);
    dialog->runDirectory = AddEntry(dialog,
        "Program working folder (blank uses the project folder)",
        profile->run_working_directory, "developer.build.run-directory");
    gtk_widget_set_tooltip_text(dialog->runDirectory,
        "Run and native Debug use this folder. Relative paths start at the project folder; the executable path still starts at the project folder.");
    dialog->arguments = AddEntry(dialog,
        "Argument list (clear the single argument above before using this field)",
        profile->run_arguments, "developer.build.arguments");
    gtk_entry_set_placeholder_text(GTK_ENTRY(dialog->arguments), "--file \"notes for review.txt\" --mode preview");
    GtkWidget *argumentHelp = gtk_label_new(
        "Spaces separate arguments. Quotes keep spaces inside one argument; \"\" passes an empty argument. Backslashes escape quotes, backslashes or spaces. No shell commands or variables are expanded.");
    gtk_label_set_wrap(GTK_LABEL(argumentHelp), TRUE);
    gtk_label_set_xalign(GTK_LABEL(argumentHelp), 0.0F);
    gtk_box_append(GTK_BOX(dialog->body), argumentHelp);
    DeveloperLaunchControls(dialog);
    DeveloperTargetControls(dialog);
    DeveloperInstalledControls(dialog);
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
/* Disposal now revokes callbacks before destroying widgets and defers release until any active acceptance callback returns.
 * The earlier implementation is retained for engineering review. */
#if 0
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
#endif
void UmiGtk4DeveloperDialogDestroy(UmiGtk4DeveloperDialog *dialog)
{
    if (dialog == NULL || dialog->closed) return;
    dialog->closed = 1;
    umi_cancellation_token_request(dialog->targetCancel);
    umi_cancellation_token_request(dialog->installedCancel);
    umi_cancellation_token_request(dialog->presetCancel);
    if (dialog->launchChooserCancel != NULL) g_cancellable_cancel(dialog->launchChooserCancel);
    dialog->onApplied = NULL; dialog->onCreated = NULL; dialog->context = NULL;
    for (guint i = 0U; i < dialog->signalObjects->len; ++i)
        g_signal_handlers_disconnect_by_data(g_ptr_array_index(dialog->signalObjects, i), dialog);
    gtk_window_destroy(dialog->window);
    DeveloperRelease(dialog);
}
