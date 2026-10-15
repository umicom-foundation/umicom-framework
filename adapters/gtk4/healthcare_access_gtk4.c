/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/healthcare_access_gtk4.c
 * PURPOSE: Present explicit synthetic access transitions without clinical data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare/access_gtk4.h"
#include "umicom/healthcare/synthetic_access.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum AccessAction {
    ACCESS_REGISTER, ACCESS_REQUEST, ACCESS_FULL, ACCESS_PARTIAL,
    ACCESS_UNAVAILABLE, ACCESS_CLARIFY, ACCESS_NO_RESPONSE, ACCESS_ACCEPT,
    ACCESS_FUND, ACCESS_RESERVE, ACCESS_COLLECT, ACCESS_EXPIRE, ACCESS_UNMET,
    ACCESS_DECLINE, ACCESS_FOLLOWUP
} AccessAction;

typedef struct AccessWorkspace {
    size_t references;
    int closed;
    UmiHealthcareSyntheticRegistry *registry;
    UmiHealthcareAccessBook *book;
    GtkWidget *patient;
    GtkWidget *item;
    GtkWidget *quantity;
    GtkWidget *offered;
    GtkWidget *provider;
    GtkWidget *coordinator;
    GtkWidget *selector;
    GtkWidget *details;
    GtkWidget *feedback;
    GtkStringList *requests;
} AccessWorkspace;

/* Dispose closes the controller before GtkBox releases any children. This is
 * earlier than weak notifications or data finalizers, so model notifications
 * during child teardown cannot dereference already released sibling widgets. */
typedef struct UmiHealthcareAccessRoot {
    GtkBox parent;
    AccessWorkspace *workspace;
} UmiHealthcareAccessRoot;
typedef struct UmiHealthcareAccessRootClass { GtkBoxClass parent; } UmiHealthcareAccessRootClass;
static void workspace_close(gpointer data);
G_DEFINE_TYPE(UmiHealthcareAccessRoot, umi_healthcare_access_root, GTK_TYPE_BOX)

static void umi_healthcare_access_root_dispose(GObject *object)
{
    UmiHealthcareAccessRoot *root = (UmiHealthcareAccessRoot *)object;
    if (root->workspace != NULL) {
        workspace_close(root->workspace);
        root->workspace = NULL;
    }
    G_OBJECT_CLASS(umi_healthcare_access_root_parent_class)->dispose(object);
}

static void umi_healthcare_access_root_class_init(UmiHealthcareAccessRootClass *klass)
{
    G_OBJECT_CLASS(klass)->dispose = umi_healthcare_access_root_dispose;
}

static void umi_healthcare_access_root_init(UmiHealthcareAccessRoot *root)
{
    root->workspace = NULL;
}

/* Signals may outlive their original parent when an embedding host retains a
 * button. Each signal owns a reference, and callbacks refuse a closed owner. */
static void workspace_unref(AccessWorkspace *workspace)
{
    if (--workspace->references == 0U) free(workspace);
}

static void signal_release(gpointer data, GClosure *closure)
{
    (void)closure;
    workspace_unref(data);
}

/* The root owns the books. Retained controls keep only an inert context after
 * root destruction; they cannot read freed sibling widgets or domain state. */
static void workspace_close(gpointer data)
{
    AccessWorkspace *workspace = data;
    workspace->closed = 1;
    UmiHealthcareAccessBookDestroy(workspace->book);
    UmiHealthcareSyntheticRegistryDestroy(workspace->registry);
    g_clear_object(&workspace->requests);
    workspace_unref(workspace);
}

static void connect_owned(AccessWorkspace *workspace, GtkWidget *widget,
    const char *signal, GCallback callback)
{
    ++workspace->references;
    g_signal_connect_data(widget, signal, callback, workspace, signal_release, 0);
}

/* Index selection addresses an authoritative book record, never parsed display
 * text. Later searchable worklists can keep this same stable ID boundary. */
static UmiStatus selected_record(AccessWorkspace *workspace,
    UmiHealthcareAccessRecord *record)
{
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(workspace->selector));
    if (index == GTK_INVALID_LIST_POSITION) return UMI_STATUS_NOT_FOUND;
    return UmiHealthcareAccessAt(workspace->book, (size_t)index, record);
}

static const char *reply_text(UmiHealthcareAccessReply reply)
{
    switch (reply) {
    case UMI_HEALTHCARE_ACCESS_FULL: return "Full availability";
    case UMI_HEALTHCARE_ACCESS_PARTIAL: return "Partial availability";
    case UMI_HEALTHCARE_ACCESS_UNAVAILABLE: return "Unavailable";
    case UMI_HEALTHCARE_ACCESS_CLARIFICATION: return "Clarification needed";
    case UMI_HEALTHCARE_ACCESS_NO_RESPONSE: return "No response recorded";
    default: return "Awaiting reply";
    }
}

static const char *unmet_text(UmiHealthcareAccessUnmetReason reason)
{
    switch (reason) {
    case UMI_HEALTHCARE_ACCESS_UNMET_UNAVAILABLE: return "Unavailable";
    case UMI_HEALTHCARE_ACCESS_UNMET_NO_RESPONSE: return "No response";
    case UMI_HEALTHCARE_ACCESS_UNMET_DECLINED: return "Declined";
    case UMI_HEALTHCARE_ACCESS_UNMET_EXPIRED: return "Reservation expired";
    default: return "None recorded";
    }
}

/* Show quantities independently, including the residual need after a partial
 * collection. The demonstration never presents a partial offer as full supply. */
static void refresh_record(AccessWorkspace *workspace)
{
    UmiHealthcareAccessRecord record;
    char text[1200];
    if (workspace->closed) return;
    if (selected_record(workspace, &record) != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(workspace->details), "Create a synthetic request to begin.");
        return;
    }
    (void)snprintf(text, sizeof(text),
        "%s  |  revision %" PRIu64 "\nPatient %s  |  Item %s\nProvider %s\n\n"
        "%s\nRequested: %u   Offered: %u   Accepted: %u\n"
        "Funding: %s\nReservation: %s\nExpires at UTC epoch seconds: %" PRIu64
        "\nCollected: %u   Remaining requested quantity: %u\nUnmet outcome: %s\nFollow-up owner: %s",
        record.requestId, record.revision, record.patientId, record.itemId,
        record.providerId[0] ? record.providerId : "Not selected", reply_text(record.reply),
        (unsigned int)record.requestedQuantity, (unsigned int)record.offeredQuantity,
        (unsigned int)record.acceptedQuantity,
        record.simulatedFundingReady ? "Simulated ready" : "Not recorded",
        record.unmetReason == UMI_HEALTHCARE_ACCESS_UNMET_EXPIRED ? "Expired" :
        record.collectedQuantity != 0U ? "Collected in this exercise" :
        record.reservationConfirmed ? "Confirmed in this exercise" : "Not confirmed",
        record.expiresAt, (unsigned int)record.collectedQuantity,
        (unsigned int)(record.requestedQuantity - record.collectedQuantity),
        unmet_text(record.unmetReason),
        record.followupOwnerId[0] ? record.followupOwnerId : "Not assigned");
    gtk_label_set_text(GTK_LABEL(workspace->details), text);
}

static void selection_changed(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    refresh_record(data);
}

/* The frontend selects an operation; Framework validates the current revision,
 * quantities and sequence. No button directly edits a stored domain record. */
static void action_clicked(GtkButton *button, gpointer data)
{
    AccessWorkspace *workspace = data;
    UmiHealthcareAccessRecord record = {0};
    UmiHealthcareSyntheticPatient patient;
    UmiStatus status;
    AccessAction action;
    const char *patientId;
    uint64_t now;
    gint64 clockValue;
    if (workspace->closed) return;
    action = (AccessAction)GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "access-action"));
    patientId = gtk_editable_get_text(GTK_EDITABLE(workspace->patient));
    clockValue = g_get_real_time();
    now = clockValue > 0 ? (uint64_t)(clockValue / G_USEC_PER_SEC) : 0U;
    if (action == ACCESS_REGISTER) {
        status = UmiHealthcareSyntheticRegistryRegister(workspace->registry, patientId, &patient);
    } else if (action == ACCESS_REQUEST) {
        status = UmiHealthcareAccessRequest(workspace->book, workspace->registry, patientId,
            gtk_editable_get_text(GTK_EDITABLE(workspace->item)),
            (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(workspace->quantity)), &record);
        if (status == UMI_STATUS_OK) {
            gtk_string_list_append(workspace->requests, record.requestId);
            gtk_drop_down_set_selected(GTK_DROP_DOWN(workspace->selector),
                g_list_model_get_n_items(G_LIST_MODEL(workspace->requests)) - 1U);
        }
    } else {
        status = selected_record(workspace, &record);
        if (status == UMI_STATUS_OK) {
            const char *id = record.requestId;
            uint64_t revision = record.revision;
            uint32_t quantity = (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(workspace->offered));
            if (action >= ACCESS_FULL && action <= ACCESS_NO_RESPONSE) {
                UmiHealthcareAccessReply reply = (UmiHealthcareAccessReply)
                    ((int)action - (int)ACCESS_FULL + (int)UMI_HEALTHCARE_ACCESS_FULL);
                if (reply >= UMI_HEALTHCARE_ACCESS_UNAVAILABLE) quantity = 0U;
                status = UmiHealthcareAccessReplyRecord(workspace->book, id, revision,
                    gtk_editable_get_text(GTK_EDITABLE(workspace->provider)), reply, quantity, &record);
            } else if (action == ACCESS_ACCEPT) {
                status = UmiHealthcareAccessAccept(workspace->book, id, revision, record.offeredQuantity, &record);
            } else if (action == ACCESS_FUND) {
                status = UmiHealthcareAccessSimulateFunding(workspace->book, id, revision, &record);
            } else if (action == ACCESS_RESERVE) {
                status = now > UINT64_MAX - 300U ? UMI_STATUS_INVALID_ARGUMENT :
                    UmiHealthcareAccessReserve(workspace->book, id, revision, now, now + 300U, &record);
            } else if (action == ACCESS_COLLECT) {
                status = UmiHealthcareAccessCollect(workspace->book, id, revision, patientId, now, &record);
            } else if (action == ACCESS_EXPIRE) {
                status = UmiHealthcareAccessExpire(workspace->book, id, revision, now, &record);
            } else if (action == ACCESS_UNMET) {
                UmiHealthcareAccessUnmetReason reason = record.reply == UMI_HEALTHCARE_ACCESS_NO_RESPONSE ?
                    UMI_HEALTHCARE_ACCESS_UNMET_NO_RESPONSE : UMI_HEALTHCARE_ACCESS_UNMET_UNAVAILABLE;
                status = UmiHealthcareAccessCloseUnmet(workspace->book, id, revision, reason, &record);
            } else if (action == ACCESS_FOLLOWUP) {
                status = UmiHealthcareAccessAssignFollowup(workspace->book, id, revision,
                    gtk_editable_get_text(GTK_EDITABLE(workspace->coordinator)), &record);
            } else if (action == ACCESS_DECLINE) {
                status = UmiHealthcareAccessCloseUnmet(workspace->book, id, revision,
                    UMI_HEALTHCARE_ACCESS_UNMET_DECLINED, &record);
            } else {
                status = UMI_STATUS_INVALID_ARGUMENT;
            }
        }
    }
    gtk_label_set_text(GTK_LABEL(workspace->feedback), status == UMI_STATUS_OK ?
        "Recorded in this session." : status == UMI_STATUS_NOT_FOUND ?
        "Select a request, or register the synthetic patient before creating one." :
        status == UMI_STATUS_INVALID_ARGUMENT ?
        "Check the synthetic IDs and quantities. Full must equal requested; partial must be smaller." :
        status == UMI_STATUS_INVALID_STATE ?
        "This action is not available in the current state. Review acceptance, funding and reservation first." :
        status == UMI_STATUS_TIMEOUT ? "The reservation expired. Record its unmet outcome." :
        status == UMI_STATUS_PERMISSION_DENIED ? "The collection patient does not match the request." :
        umi_status_text(status));
    refresh_record(workspace);
}

static GtkWidget *labelled_field(GtkWidget *grid, int row, const char *label, GtkWidget *field)
{
    GtkWidget *caption = gtk_label_new(label);
    gtk_label_set_xalign(GTK_LABEL(caption), 0.0F);
    gtk_label_set_mnemonic_widget(GTK_LABEL(caption), field);
    gtk_grid_attach(GTK_GRID(grid), caption, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), field, 1, row, 1, 1);
    gtk_widget_set_hexpand(field, TRUE);
    return field;
}

static void append_action(AccessWorkspace *workspace, GtkWidget *box,
    const char *label, const char *name, AccessAction action)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    gtk_widget_set_name(button, name);
    g_object_set_data(G_OBJECT(button), "access-action", GINT_TO_POINTER((int)action));
    connect_owned(workspace, button, "clicked", G_CALLBACK(action_clicked));
    gtk_box_append(GTK_BOX(box), button);
}

UmiStatus UmiHealthcareAccessWorkspaceNew(GtkWidget **outWidget)
{
    AccessWorkspace *workspace;
    GtkWidget *root, *grid, *row, *title, *notice;
    UmiStatus status;
    if (outWidget == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workspace = calloc(1U, sizeof(*workspace));
    if (workspace == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    workspace->references = 1U;
    status = UmiHealthcareSyntheticRegistryCreate(&workspace->registry);
    if (status == UMI_STATUS_OK) status = UmiHealthcareAccessBookCreate(&workspace->book);
    if (status != UMI_STATUS_OK) { workspace_close(workspace); return status; }
    root = g_object_new(umi_healthcare_access_root_get_type(),
        "orientation", GTK_ORIENTATION_VERTICAL, "spacing", 12, NULL);
    ((UmiHealthcareAccessRoot *)root)->workspace = workspace;
    gtk_widget_set_margin_start(root, 20); gtk_widget_set_margin_end(root, 20);
    gtk_widget_set_margin_top(root, 20); gtk_widget_set_margin_bottom(root, 20);
    title = gtk_label_new("Umicom Healthcare — Access requests");
    gtk_widget_add_css_class(title, "title-1");
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_box_append(GTK_BOX(root), title);
    notice = gtk_label_new("Synthetic exercise • Session memory only\nUse fictional IDs only. No real patients, prescriptions, stock reservations or payments. Closing discards this session.");
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE);
    gtk_label_set_xalign(GTK_LABEL(notice), 0.0F);
    gtk_box_append(GTK_BOX(root), notice);
    grid = gtk_grid_new(); gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    gtk_box_append(GTK_BOX(root), grid);
    workspace->patient = labelled_field(grid, 0, "Synthetic patient", gtk_entry_new());
    gtk_editable_set_text(GTK_EDITABLE(workspace->patient), "SYN-000001");
    workspace->item = labelled_field(grid, 1, "Synthetic item", gtk_entry_new());
    gtk_editable_set_text(GTK_EDITABLE(workspace->item), "ITM-000001");
    workspace->quantity = labelled_field(grid, 2, "Requested quantity", gtk_spin_button_new_with_range(1, UMI_HEALTHCARE_ACCESS_QUANTITY_LIMIT, 1));
    workspace->provider = labelled_field(grid, 3, "Synthetic provider", gtk_entry_new());
    gtk_editable_set_text(GTK_EDITABLE(workspace->provider), "PRV-000001");
    workspace->offered = labelled_field(grid, 4, "Offered quantity", gtk_spin_button_new_with_range(1, UMI_HEALTHCARE_ACCESS_QUANTITY_LIMIT, 1));
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(root), row);
    append_action(workspace, row, "Register patient", "healthcare-register", ACCESS_REGISTER);
    append_action(workspace, row, "Create request", "healthcare-request", ACCESS_REQUEST);
    workspace->coordinator = labelled_field(grid, 5, "Synthetic follow-up owner", gtk_entry_new());
    gtk_editable_set_text(GTK_EDITABLE(workspace->coordinator), "STF-000001");
    workspace->requests = gtk_string_list_new(NULL);
    /* GtkDropDown takes its own model reference; the owner retains one for append. */
    workspace->selector = gtk_drop_down_new(G_LIST_MODEL(g_object_ref(workspace->requests)), NULL);
    gtk_widget_set_name(workspace->selector, "healthcare-requests");
    gtk_box_append(GTK_BOX(root), workspace->selector);
    connect_owned(workspace, workspace->selector, "notify::selected", G_CALLBACK(selection_changed));
    workspace->details = gtk_label_new("");
    gtk_widget_set_name(workspace->details, "healthcare-details");
    gtk_label_set_xalign(GTK_LABEL(workspace->details), 0.0F);
    gtk_label_set_selectable(GTK_LABEL(workspace->details), TRUE);
    gtk_label_set_wrap(GTK_LABEL(workspace->details), TRUE);
    gtk_box_append(GTK_BOX(root), workspace->details);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(root), row);
    append_action(workspace, row, "Full offer", "healthcare-full", ACCESS_FULL);
    append_action(workspace, row, "Partial offer", "healthcare-partial", ACCESS_PARTIAL);
    append_action(workspace, row, "Unavailable", "healthcare-unavailable", ACCESS_UNAVAILABLE);
    append_action(workspace, row, "Clarification", "healthcare-clarify", ACCESS_CLARIFY);
    append_action(workspace, row, "No response", "healthcare-no-response", ACCESS_NO_RESPONSE);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(root), row);
    append_action(workspace, row, "Accept offer", "healthcare-accept", ACCESS_ACCEPT);
    append_action(workspace, row, "Simulate funding", "healthcare-funding", ACCESS_FUND);
    append_action(workspace, row, "Reserve for 5 minutes", "healthcare-reserve", ACCESS_RESERVE);
    append_action(workspace, row, "Record collection", "healthcare-collect", ACCESS_COLLECT);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(root), row);
    append_action(workspace, row, "Expire reservation", "healthcare-expire", ACCESS_EXPIRE);
    append_action(workspace, row, "Close unmet", "healthcare-unmet", ACCESS_UNMET);
    append_action(workspace, row, "Decline offer", "healthcare-decline", ACCESS_DECLINE);
    append_action(workspace, row, "Assign follow-up", "healthcare-followup", ACCESS_FOLLOWUP);
    workspace->feedback = gtk_label_new("Register a synthetic patient, then create a request.");
    gtk_widget_set_name(workspace->feedback, "healthcare-feedback");
    gtk_label_set_wrap(GTK_LABEL(workspace->feedback), TRUE);
    gtk_label_set_xalign(GTK_LABEL(workspace->feedback), 0.0F);
    gtk_box_append(GTK_BOX(root), workspace->feedback);
    refresh_record(workspace);
    *outWidget = root;
    return UMI_STATUS_OK;
}
