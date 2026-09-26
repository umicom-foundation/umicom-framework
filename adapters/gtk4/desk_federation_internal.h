/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/desk_federation_internal.h
 *
 * PURPOSE:
 *   Share private widget state among the Framework Desk view callbacks.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_GTK4_DESK_FEDERATION_INTERNAL_H
#define UMICOM_GTK4_DESK_FEDERATION_INTERNAL_H
#include "umicom/ui/gtk4/desk_federation.h"
#define DF_GTK_KEY "umicom-desk-federation"
typedef struct DfGtk {
    UmiDeskRuntime *runtime;
    UmiDataServer *server;
    UmiDeskFederation *service;
    GtkWidget *panel; /* Strong reference keeps retained/detached content safe. */
    GtkWidget *controls;
    GtkDropDown *saved;
    GtkEntry *id, *title, *link;
    GtkLabel *status, *storage, *activityBadge;
    GtkTextBuffer *preview, *activity;
    GtkButton *confirm;
    UmiDeskWorkspace workspaces[UMI_DESK_FEDERATION_MAX_WORKSPACES];
    size_t workspaceCount;
    uint64_t previewToken;
    uint64_t activitySequence, droppedActivity;
    size_t unreadCount;
    bool invalid, updating;
} DfGtk;
DfGtk *DfGtkGet(GtkWidget *root);
void DfGtkStatus(DfGtk *ui, UmiStatus status, const char *success);
void DfGtkCancel(DfGtk *ui);
void DfGtkRefreshWorkspaces(DfGtk *ui);
void DfGtkRefreshActivity(DfGtk *ui);
void DfGtkOpenClicked(GtkButton *button, gpointer root);
void DfGtkSaveClicked(GtkButton *button, gpointer root);
void DfGtkCaptureClicked(GtkButton *button, gpointer root);
void DfGtkPreviewClicked(GtkButton *button, gpointer root);
void DfGtkLinkClicked(GtkButton *button, gpointer root);
void DfGtkConfirmClicked(GtkButton *button, gpointer root);
void DfGtkCancelClicked(GtkButton *button, gpointer root);
void DfGtkRemoveClicked(GtkButton *button, gpointer root);
void DfGtkReadClicked(GtkButton *button, gpointer root);
void DfGtkSelected(GObject *object, GParamSpec *property, gpointer root);
void DfGtkInputChanged(GtkEditable *editable, gpointer root);
#endif
