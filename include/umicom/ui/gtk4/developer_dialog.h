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
UmiStatus UmiGtk4BuildSettingsDialogCreate(GtkWindow *parent,
    const UmiBuildProfile *profile, int trusted, UmiGtk4BuildSettingsApplied onApplied,
    void *context, UmiGtk4DeveloperDialog **outDialog);
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
