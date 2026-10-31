/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/developer_dialog.h
 *
 * PURPOSE:
 *   Reusable New Project and Build Settings forms for developer applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_DEVELOPER_DIALOG_H
#define UMICOM_UI_GTK4_DEVELOPER_DIALOG_H
#include <gtk/gtk.h>
#include "umicom/developer_project/new_project.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiGtk4DeveloperDialog UmiGtk4DeveloperDialog;
/** Borrowed results are valid for the callback only. Copy values to retain them. */
typedef UmiStatus (*UmiGtk4ProjectCreated)(const UmiDeveloperProjectModel *model,
    const UmiBuildProfile *profile, void *context);
typedef UmiStatus (*UmiGtk4BuildSettingsApplied)(const UmiBuildProfile *profile,
    int trusted, void *context);
/** No code is executed when opening either form. Call on the GTK thread.
 * The owner must destroy this handle before releasing the callback context. */
UmiStatus UmiGtk4NewProjectDialogCreate(GtkWindow *parent,
    UmiGtk4ProjectCreated onCreated, void *context, UmiGtk4DeveloperDialog **outDialog);
/**
 * Open a source-review form for a native application module. The user supplies
 * a parent folder, reviews rendered files, then explicitly creates a new folder.
 * Reading templates and writing files run in background workers. No Git, build,
 * install, launch or network action is performed. After creation, Open Project
 * invokes onCreated with borrowed model/profile values under the same lifetime
 * contract as the ordinary project dialog. The owner must destroy the handle
 * before releasing context. Closing during a write does not undo created files.
 */
UmiStatus UmiGtk4ApplicationModuleDialogCreate(GtkWindow *parent,
    UmiGtk4ProjectCreated onCreated, void *context, UmiGtk4DeveloperDialog **outDialog);

/** Prepare a hidden project or application form without creating files.
 * Use stack-owned output storage across construction; native observers may
 * close the host. Reacquire the host before adopting the returned handle and
 * calling Present. Destroy any unadopted handle. The callback remains borrowed
 * and is invoked only after an explicit visible-form acceptance.
 * On failure the output is NULL. No build or Git command runs. */
UmiStatus UmiGtk4NewProjectDialogPrepare(GtkWindow *parent,
    UmiGtk4ProjectCreated onCreated, void *context, UmiGtk4DeveloperDialog **outDialog);
UmiStatus UmiGtk4ApplicationModuleDialogPrepare(GtkWindow *parent,
    UmiGtk4ProjectCreated onCreated, void *context, UmiGtk4DeveloperDialog **outDialog);

UmiStatus UmiGtk4BuildSettingsDialogCreate(GtkWindow *parent,
    const UmiBuildProfile *profile, int trusted, UmiGtk4BuildSettingsApplied onApplied,
    void *context, UmiGtk4DeveloperDialog **outDialog);
/** Construct a hidden settings form. Bind optional configuration controls,
 * store the handle in the live owner, then call Present. No execution or trust
 * change occurs. The profile is copied; the callback context remains borrowed.
 * Destroy the returned handle even when later binding or presentation fails. */
UmiStatus UmiGtk4BuildSettingsDialogPrepare(GtkWindow *parent,
    const UmiBuildProfile *profile, int trusted, UmiGtk4BuildSettingsApplied onApplied,
    void *context, UmiGtk4DeveloperDialog **outDialog);

/** Build settings keep the same controls while changing visible sections.
 * Selection only changes presentation: no profile save, trust change, discovery
 * or process launch occurs. Call on the GTK thread with a live settings handle.
 * Prepared hidden forms support selection. Invalid calls leave output unchanged.
 * A selection notification may retire the owner; Select then returns CANCELLED.
 * Reentrant Select returns BUSY. New-project forms do not have these sections. */
typedef enum UmiGtk4BuildSettingsSection {
    UMI_GTK4_BUILD_SETTINGS_BUILD,
    UMI_GTK4_BUILD_SETTINGS_TOOLS,
    UMI_GTK4_BUILD_SETTINGS_PRESETS,
    UMI_GTK4_BUILD_SETTINGS_RUN,
    UMI_GTK4_BUILD_SETTINGS_TARGETS,
    UMI_GTK4_BUILD_SETTINGS_SDK,
    UMI_GTK4_BUILD_SETTINGS_INTELLIGENCE,
    UMI_GTK4_BUILD_SETTINGS_CONFIGURATIONS,
    UMI_GTK4_BUILD_SETTINGS_SECTION_COUNT
} UmiGtk4BuildSettingsSection;
UmiStatus UmiGtk4BuildSettingsSelectSection(UmiGtk4DeveloperDialog *dialog,
    UmiGtk4BuildSettingsSection section);
UmiStatus UmiGtk4BuildSettingsCurrentSection(const UmiGtk4DeveloperDialog *dialog,
    UmiGtk4BuildSettingsSection *outSection);

/** Present a live form on the GTK thread. Ownership remains with the caller.
 * Presentation can synchronously notify observers that destroy the owner. The
 * function retains internal lifetime and returns CANCELLED if the form closes.
 * Callers must reacquire their own owner before accessing it after this call. */
UmiStatus UmiGtk4DeveloperDialogPresent(UmiGtk4DeveloperDialog *dialog);
/** Accept/Cancel hides the form. The owner retains it until Destroy. */
/** Destroy may be called from an acceptance callback on the GTK thread. Clear
 * the owner's stored handle at that point; the callback's borrowed result stays
 * valid until it returns. Hidden forms and controls retained after destruction
 * cannot accept another result. Recursive acceptance is ignored. A failed
 * acceptance leaves a visible form available for correction and retry. */
void UmiGtk4DeveloperDialogDestroy(UmiGtk4DeveloperDialog *dialog);
#ifdef __cplusplus
}
#endif
#endif
