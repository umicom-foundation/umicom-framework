/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/healthcare_access_gtk4.c
 * PURPOSE: Present explicit synthetic access transitions without clinical data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/healthcare/access_gtk4.h"
#include "umicom/healthcare/synthetic_access.h"
#include "umicom/healthcare/access_worklist.h"
#include "umicom/ui/gtk4/drop_down.h"
#ifdef UMICOM_HEALTHCARE_ARCHIVE_AVAILABLE
#include "umicom/healthcare/access_archive.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/ui/gtk4/path_picker.h"
#endif
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum AccessAction {
    ACCESS_REGISTER, ACCESS_REQUEST, ACCESS_FULL, ACCESS_PARTIAL,
    ACCESS_UNAVAILABLE, ACCESS_CLARIFY, ACCESS_NO_RESPONSE, ACCESS_ACCEPT,
    ACCESS_FUND, ACCESS_RESERVE, ACCESS_COLLECT, ACCESS_EXPIRE, ACCESS_UNMET,
    ACCESS_DECLINE, ACCESS_FOLLOWUP
} AccessAction;

typedef struct AccessWorkspace {
    size_t references;
    int closed;
    bool applying;
    bool ioBusy;
#ifdef UMICOM_HEALTHCARE_ARCHIVE_AVAILABLE
    GtkWidget *archivePath;
    GtkWidget *replaceApproval;
    GtkWidget *archiveStatus;
#endif
    UmiHealthcareAccessWorklist worklist;
    GtkWidget *filter;
    GtkWidget *ownerFilter;
    GtkWidget *workSummary;
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

/* The former insertion-index lookup is retained for review. Filtered rows now
 * resolve their stable request ID and reject a stale displayed revision. */
#if 0
/* Index selection addresses an authoritative book record, never parsed display
 * text. Later searchable worklists can keep this same stable ID boundary. */
static UmiStatus selected_record(AccessWorkspace *workspace,
    UmiHealthcareAccessRecord *record)
{
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(workspace->selector));
    if (index == GTK_INVALID_LIST_POSITION) return UMI_STATUS_NOT_FOUND;
    return UmiHealthcareAccessAt(workspace->book, (size_t)index, record);
}

#endif
static UmiStatus selected_record(AccessWorkspace *workspace,
    UmiHealthcareAccessRecord *record)
{
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(workspace->selector));
/* Map the explicit placeholder separately from authoritative request rows. The former expression remains for review. */
#if 0
    if (index == GTK_INVALID_LIST_POSITION || (size_t)index >= workspace->worklist.count)
        return UMI_STATUS_NOT_FOUND;
    const UmiHealthcareAccessRecord *displayed = &workspace->worklist.items[index].record;
#endif
    /* Row zero is an explicit non-record choice. GTK may automatically select
     * a populated model; that must never authorize an operation on its first
     * request after filtering, assignment or archive replacement. */
    if (index == GTK_INVALID_LIST_POSITION || index == 0U ||
        (size_t)(index - 1U) >= workspace->worklist.count)
        return UMI_STATUS_NOT_FOUND;
    const UmiHealthcareAccessRecord *displayed = &workspace->worklist.items[index - 1U].record;
    UmiStatus status = UmiHealthcareAccessFind(workspace->book, displayed->requestId, record);
    if (status == UMI_STATUS_OK && record->revision != displayed->revision)
        return UMI_STATUS_INVALID_STATE;
    return status;
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
        gtk_label_set_text(GTK_LABEL(workspace->details), "Select a request in this worklist, or change the filters.");
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

/* Projection never expires a reservation. The explicit expiry action owns that
 * transition. Callers hold the controller and block reentry while GTK publishes
 * models and labels; root disposal can occur at any notification boundary. */
/* Replace implicit unselection with a non-actionable placeholder while preserving the worklist, stable IDs and revision checks. The former expression remains for review. */
#if 0
static UmiStatus rebuild_worklist(AccessWorkspace *workspace, const char *preferredId)
{
    UmiHealthcareAccessWorklist candidate;
    gint64 clockValue = g_get_real_time();
    uint64_t now = clockValue > 0 ? (uint64_t)(clockValue / G_USEC_PER_SEC) : 0U;
    UmiStatus status = UmiHealthcareAccessWorklistBuild(workspace->book,
        (UmiHealthcareAccessFilter)gtk_drop_down_get_selected(GTK_DROP_DOWN(workspace->filter)),
        gtk_editable_get_text(GTK_EDITABLE(workspace->ownerFilter)), now, &candidate);
    if (status != UMI_STATUS_OK) return status;
    GtkStringList *model = gtk_string_list_new(NULL);
    guint selection = GTK_INVALID_LIST_POSITION;
    for (size_t i = 0U; i < candidate.count; ++i) {
        gtk_string_list_append(model, candidate.items[i].record.requestId);
        if (preferredId != NULL && strcmp(candidate.items[i].record.requestId, preferredId) == 0)
            selection = (guint)i;
    }
    workspace->worklist = candidate;
    g_clear_object(&workspace->requests);
    workspace->requests = model;
    gtk_drop_down_set_model(GTK_DROP_DOWN(workspace->selector), G_LIST_MODEL(model));
    if (workspace->closed) return UMI_STATUS_OK;
    /* If assignment removes a row from the unassigned view, leave selection
     * empty rather than silently making the next request the action target. */
    gtk_drop_down_set_selected(GTK_DROP_DOWN(workspace->selector), selection);
    if (workspace->closed) return UMI_STATUS_OK;
    char summary[256];
    (void)snprintf(summary, sizeof(summary),
        "Showing %zu of %zu requests | Outstanding %zu | Follow-up %zu | Unassigned %zu | Overdue reservations %zu",
        candidate.count, candidate.totalRequests, candidate.outstandingRequests,
        candidate.followupRequests, candidate.unassignedFollowups, candidate.overdueReservations);
    gtk_label_set_text(GTK_LABEL(workspace->workSummary), summary);
    if (!workspace->closed) refresh_record(workspace);
    return UMI_STATUS_OK;
}

#endif
static UmiStatus rebuild_worklist(AccessWorkspace *workspace, const char *preferredId)
{
    UmiHealthcareAccessWorklist candidate;
    gint64 clockValue = g_get_real_time();
    uint64_t now = clockValue > 0 ? (uint64_t)(clockValue / G_USEC_PER_SEC) : 0U;
    UmiStatus status = UmiHealthcareAccessWorklistBuild(workspace->book,
        (UmiHealthcareAccessFilter)gtk_drop_down_get_selected(GTK_DROP_DOWN(workspace->filter)),
        gtk_editable_get_text(GTK_EDITABLE(workspace->ownerFilter)), now, &candidate);
    if (status != UMI_STATUS_OK) return status;
    GtkStringList *model = gtk_string_list_new(NULL);
    /* A real model row makes the no-request state portable across GTK
     * selection policies; no private GTK selection object is accessed. */
    gtk_string_list_append(model, "Select a request");
    guint selection = 0U;
    for (size_t i = 0U; i < candidate.count; ++i) {
        gtk_string_list_append(model, candidate.items[i].record.requestId);
        if (preferredId != NULL && strcmp(candidate.items[i].record.requestId, preferredId) == 0)
            selection = (guint)i + 1U;
    }
    workspace->worklist = candidate;
    g_clear_object(&workspace->requests);
    workspace->requests = model;
    gtk_drop_down_set_model(GTK_DROP_DOWN(workspace->selector), G_LIST_MODEL(model));
    if (workspace->closed) return UMI_STATUS_OK;
    /* If assignment removes a row from the unassigned view, select the
     * placeholder so the next request cannot become an implicit action target. */
    gtk_drop_down_set_selected(GTK_DROP_DOWN(workspace->selector), selection);
    if (workspace->closed) return UMI_STATUS_OK;
    char summary[256];
    (void)snprintf(summary, sizeof(summary),
        "Showing %zu of %zu requests | Outstanding %zu | Follow-up %zu | Unassigned %zu | Overdue reservations %zu",
        candidate.count, candidate.totalRequests, candidate.outstandingRequests,
        candidate.followupRequests, candidate.unassignedFollowups, candidate.overdueReservations);
    gtk_label_set_text(GTK_LABEL(workspace->workSummary), summary);
    if (!workspace->closed) refresh_record(workspace);
    return UMI_STATUS_OK;
}

/* Retain the former callback for review. The guarded replacement holds the
 * controller even when a host destroys its root during a label notification. */
#if 0
static void selection_changed(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    refresh_record(data);
}

#endif
static void selection_changed(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    AccessWorkspace *workspace = data;
    if (workspace->closed || workspace->applying) return;
    ++workspace->references;
    workspace->applying = true;
    refresh_record(workspace);
    workspace->applying = false;
    workspace_unref(workspace);
}

static void filter_refresh(AccessWorkspace *workspace)
{
    if (workspace->closed || workspace->applying) return;
    ++workspace->references;
    workspace->applying = true;
    UmiHealthcareAccessRecord selected = {0};
    (void)selected_record(workspace, &selected);
    UmiStatus status = rebuild_worklist(workspace, selected.requestId);
    if (!workspace->closed && status != UMI_STATUS_OK)
        gtk_label_set_text(GTK_LABEL(workspace->feedback),
            "Worklist unchanged. Use an empty owner filter or a fictional STF-000001 style ID.");
    workspace->applying = false;
    workspace_unref(workspace);
}
static void filter_changed(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    filter_refresh(data);
}
static void filter_clicked(GtkButton *button, gpointer data)
{
    (void)button;
    filter_refresh(data);
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
    if (workspace->closed || workspace->applying) return;
    /* Accepted archive I/O owns this session until completion; retained controls
     * must not mutate the source while an open operation could replace it. */
    if (workspace->ioBusy) return;
    /* Hold the context and block nested actions until model publication ends. */
    ++workspace->references;
    workspace->applying = true;
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
        /* Retained insertion-order presentation. Filtered worklists below now
         * rebuild the model and restore selection by request ID. */
#if 0
        if (status == UMI_STATUS_OK) {
            gtk_string_list_append(workspace->requests, record.requestId);
            gtk_drop_down_set_selected(GTK_DROP_DOWN(workspace->selector),
                g_list_model_get_n_items(G_LIST_MODEL(workspace->requests)) - 1U);
        }
#endif
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
    if (status == UMI_STATUS_OK && action != ACCESS_REGISTER)
        (void)rebuild_worklist(workspace, record.requestId);
    if (!workspace->closed) {
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
    }
    if (!workspace->closed) refresh_record(workspace);
    workspace->applying = false;
    workspace_unref(workspace);
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


#ifdef UMICOM_HEALTHCARE_ARCHIVE_AVAILABLE
/* Workers own immutable inputs and newly decoded handles, never live widget
 * state. Completion holds only the controller until its owner-thread callback;
 * disposal closes the controller early and makes late results inert. */
typedef struct AccessArchiveJob {
    AccessWorkspace *workspace;
    char *path;
    char *bytes;
    size_t length;
    bool saving;
    UmiStatus status;
    UmiHealthcareSyntheticRegistry *registry;
    UmiHealthcareAccessBook *book;
} AccessArchiveJob;

static void archive_job_free(gpointer data)
{
    AccessArchiveJob *job = data;
    g_free(job->path);
    UmiHealthcareAccessArchiveFree(job->bytes);
    UmiHealthcareAccessBookDestroy(job->book);
    UmiHealthcareSyntheticRegistryDestroy(job->registry);
    free(job);
}

static void archive_worker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    AccessArchiveJob *job = data;
    (void)source; (void)cancel;
    if (job->saving) {
        UmiOutputFile *file = NULL;
        job->status = UmiOutputFileCreate(job->path, &file);
        if (job->status == UMI_STATUS_OK) {
            job->status = UmiOutputFileWrite(file, job->bytes, job->length);
            UmiStatus closed = UmiOutputFileClose(file);
            if (job->status == UMI_STATUS_OK) job->status = closed;
        }
        UmiOutputFileDestroy(file);
    } else {
        unsigned char *bytes = NULL;
        size_t length = 0U;
        job->status = UmiInputFileRead(job->path, UMI_HEALTHCARE_ACCESS_ARCHIVE_MAX_BYTES,
            &bytes, &length);
        if (job->status == UMI_STATUS_OK)
            job->status = UmiHealthcareAccessArchiveDecode(bytes, length, &job->registry, &job->book);
        UmiInputFileFree(bytes);
    }
    g_task_return_boolean(task, TRUE);
}

static void archive_done(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source; (void)data;
    GTask *task = G_TASK(result);
    AccessArchiveJob *job = g_task_get_task_data(task);
    AccessWorkspace *workspace = job->workspace;
    (void)g_task_propagate_boolean(task, NULL);
    if (!workspace->closed) {
        workspace->applying = true;
        if (!job->saving && job->status == UMI_STATUS_OK) {
            UmiHealthcareAccessBookDestroy(workspace->book);
            UmiHealthcareSyntheticRegistryDestroy(workspace->registry);
            workspace->book = job->book;
            workspace->registry = job->registry;
            job->book = NULL;
            job->registry = NULL;
            /* Restoring never carries a row index or a filter from another book.
             * Model notifications can dispose the root; check every boundary. */
            gtk_editable_set_text(GTK_EDITABLE(workspace->ownerFilter), "");
            if (!workspace->closed)
                gtk_drop_down_set_selected(GTK_DROP_DOWN(workspace->filter), UMI_HEALTHCARE_ACCESS_ALL);
            if (!workspace->closed) (void)rebuild_worklist(workspace, NULL);
        }
        if (!workspace->closed) {
            char *message = job->status == UMI_STATUS_OK ?
                g_strdup_printf(job->saving ? "Saved captured session to %s" :
                    "Opened synthetic archive %s. Select a request to continue.", job->path) :
                g_strdup_printf(job->saving ?
                    "Save failed (%s). Existing files are not overwritten. A partial new file may remain; use a new filename." :
                    "Open failed (%s). The current session is unchanged.", umi_status_text(job->status));
            gtk_label_set_text(GTK_LABEL(workspace->archiveStatus), message);
            g_free(message);
        }
        workspace->applying = false;
    }
    workspace->ioBusy = false;
    /* Release GTK-owned context on this thread, not in the worker's eventual
     * task-data finalizer. The finalizer owns only independent allocations. */
    job->workspace = NULL;
    workspace_unref(workspace);
}

static void archive_clicked(GtkButton *button, gpointer data)
{
    AccessWorkspace *workspace = data;
    AccessArchiveJob *job = NULL;
    UmiStatus status = UMI_STATUS_OK;
    size_t count = 0U;
    bool saving = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "archive-save")) != 0;
    if (workspace->closed || workspace->applying || workspace->ioBusy) return;
    ++workspace->references;
    workspace->applying = true;
    const char *chosen = gtk_editable_get_text(GTK_EDITABLE(workspace->archivePath));
    status = UmiOutputFileValidatePath(chosen);
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(workspace->archiveStatus),
            "Choose an ordinary absolute file path in an existing trusted folder.");
        goto release;
    }
    if (!saving) {
        status = UmiHealthcareSyntheticRegistryCount(workspace->registry, &count);
        if (status != UMI_STATUS_OK) goto release;
        if (count != 0U && !gtk_check_button_get_active(GTK_CHECK_BUTTON(workspace->replaceApproval))) {
            gtk_label_set_text(GTK_LABEL(workspace->archiveStatus),
                "Save the current session first, or confirm Replace current session before opening.");
            goto release;
        }
    }
    job = calloc(1U, sizeof(*job));
    if (job == NULL) {
        gtk_label_set_text(GTK_LABEL(workspace->archiveStatus), "Unable to allocate archive operation.");
        goto release;
    }
    job->workspace = workspace;
    job->saving = saving;
    job->path = g_strdup(chosen);
    if (saving) status = UmiHealthcareAccessArchiveEncode(workspace->registry, workspace->book,
        &job->bytes, &job->length);
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(workspace->archiveStatus), "Unable to capture this session; no file was written.");
        goto release;
    }
    workspace->ioBusy = true;
    if (!saving) gtk_check_button_set_active(GTK_CHECK_BUTTON(workspace->replaceApproval), FALSE);
    if (workspace->closed) goto release;
    gtk_label_set_text(GTK_LABEL(workspace->archiveStatus), saving ?
        "Saving captured session…" : "Opening archive; the current session is retained until validation completes…");
    if (workspace->closed) goto release;
    GTask *task = g_task_new(NULL, NULL, archive_done, NULL);
    g_task_set_task_data(task, job, archive_job_free);
    workspace->applying = false;
    g_task_run_in_thread(task, archive_worker);
    g_object_unref(task);
    return; /* Completion owns the controller reference. */
release:
    workspace->ioBusy = false;
    workspace->applying = false;
    if (job != NULL) archive_job_free(job);
    workspace_unref(workspace);
}

static void append_archives(AccessWorkspace *workspace, GtkWidget *root)
{
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    workspace->archivePath = gtk_entry_new();
    gtk_widget_set_name(workspace->archivePath, "healthcare-archive-path");
    gtk_entry_set_placeholder_text(GTK_ENTRY(workspace->archivePath), "Choose an archive path");
    gtk_widget_set_hexpand(workspace->archivePath, TRUE);
    gtk_box_append(GTK_BOX(row), workspace->archivePath);
    GtkWidget *browseOpen = UmiPathPickerGtk4Create(GTK_ENTRY(workspace->archivePath),
        UMI_PATH_PICKER_EXISTING_FILE, "Choose a synthetic access archive", NULL);
    GtkWidget *browseSave = UmiPathPickerGtk4Create(GTK_ENTRY(workspace->archivePath),
        UMI_PATH_PICKER_NEW_FILE, "Choose a new synthetic access archive", "synthetic-access.json");
    /* The shared picker owns a box containing its button and status. Keep the
     * superseded direct button casts for review; use its supplied labels so
     * hosts do not depend on the picker's private widget hierarchy. */
#if 0
    gtk_button_set_label(GTK_BUTTON(browseOpen), "Choose existing…");
    gtk_button_set_label(GTK_BUTTON(browseSave), "Choose new…");
#endif
    gtk_box_append(GTK_BOX(row), browseOpen);
    gtk_box_append(GTK_BOX(row), browseSave);
    gtk_box_append(GTK_BOX(root), row);
    workspace->replaceApproval = gtk_check_button_new_with_label("Replace current session when opening");
    gtk_widget_set_name(workspace->replaceApproval, "healthcare-archive-replace");
    gtk_box_append(GTK_BOX(root), workspace->replaceApproval);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *save = gtk_button_new_with_label("Save new archive");
    GtkWidget *open = gtk_button_new_with_label("Open archive");
    gtk_widget_set_name(save, "healthcare-archive-save");
    gtk_widget_set_name(open, "healthcare-archive-open");
    g_object_set_data(G_OBJECT(save), "archive-save", GINT_TO_POINTER(1));
    connect_owned(workspace, save, "clicked", G_CALLBACK(archive_clicked));
    connect_owned(workspace, open, "clicked", G_CALLBACK(archive_clicked));
    gtk_box_append(GTK_BOX(row), save);
    gtk_box_append(GTK_BOX(row), open);
    gtk_box_append(GTK_BOX(root), row);
    workspace->archiveStatus = gtk_label_new("Archives contain fictional IDs and exercise state only. Save changes before closing.");
    gtk_widget_set_name(workspace->archiveStatus, "healthcare-archive-status");
    gtk_label_set_wrap(GTK_LABEL(workspace->archiveStatus), TRUE);
    gtk_label_set_xalign(GTK_LABEL(workspace->archiveStatus), 0.0F);
    gtk_box_append(GTK_BOX(root), workspace->archiveStatus);
}
#endif

/* Hosts can defer their own close prompt while a live workspace is busy.
 * Once disposed, the workspace is inert even if an accepted write finishes. */
bool UmiHealthcareAccessWorkspaceBusy(GtkWidget *widget)
{
    if (widget == NULL || !G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_healthcare_access_root_get_type()))
        return false;
    AccessWorkspace *workspace = ((UmiHealthcareAccessRoot *)widget)->workspace;
    return workspace != NULL && !workspace->closed && (workspace->applying || workspace->ioBusy);
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
#ifdef UMICOM_HEALTHCARE_ARCHIVE_AVAILABLE
    notice = gtk_label_new("Synthetic exercise • Manual archive storage\nUse fictional IDs only. No real patients, prescriptions, stock reservations or payments. Unsaved changes are lost on closing.");
#else
    /* The original memory-only notice remains correct for minimal hosts that
     * do not link the optional archive component. */
    notice = gtk_label_new("Synthetic exercise • Session memory only\nUse fictional IDs only. No real patients, prescriptions, stock reservations or payments. Closing discards this session.");
#endif
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
    static const char *const filters[] = {
        "All requests", "Outstanding quantity", "Needs follow-up", "Unassigned follow-up",
        "Reserved", "Overdue reservations", "Collected", NULL
    };
    workspace->filter = labelled_field(grid, 6, "Worklist", gtk_drop_down_new_from_strings(filters));
    gtk_widget_set_name(workspace->filter, "healthcare-worklist-filter");
    workspace->ownerFilter = labelled_field(grid, 7, "Filter by follow-up owner", gtk_entry_new());
    gtk_entry_set_placeholder_text(GTK_ENTRY(workspace->ownerFilter), "All owners, or STF-000001");
    gtk_widget_set_name(workspace->ownerFilter, "healthcare-owner-filter");
    gtk_widget_set_name(workspace->coordinator, "healthcare-coordinator");
    GtkWidget *refresh = gtk_button_new_with_label("Refresh worklist");
    gtk_widget_set_name(refresh, "healthcare-worklist-refresh");
    gtk_box_append(GTK_BOX(root), refresh);
    connect_owned(workspace, refresh, "clicked", G_CALLBACK(filter_clicked));
    connect_owned(workspace, workspace->filter, "notify::selected", G_CALLBACK(filter_changed));
    workspace->workSummary = gtk_label_new("No requests");
    gtk_widget_set_name(workspace->workSummary, "healthcare-worklist-summary");
    gtk_label_set_wrap(GTK_LABEL(workspace->workSummary), TRUE);
    gtk_label_set_xalign(GTK_LABEL(workspace->workSummary), 0.0F);
    gtk_box_append(GTK_BOX(root), workspace->workSummary);
    workspace->requests = gtk_string_list_new(NULL);
    gtk_string_list_append(workspace->requests, "Select a request");
    /* Retain the direct construction for ownership review. The shared helper
     * below now makes the consumed reference explicit for every application. */
#if 0
    /* GtkDropDown takes its own model reference; the owner retains one for append. */
    workspace->selector = gtk_drop_down_new(G_LIST_MODEL(g_object_ref(workspace->requests)), NULL);
#endif
    /* The workspace retains its list reference for appends. Transfer a separate
     * reference to the dropdown so either owner can release its lifetime safely. */
    workspace->selector = umi_ui_gtk4_drop_down_new_take_string_list(
        g_object_ref(workspace->requests));
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
    #ifdef UMICOM_HEALTHCARE_ARCHIVE_AVAILABLE
    append_archives(workspace, root);
    #endif
    refresh_record(workspace);
    *outWidget = root;
    return UMI_STATUS_OK;
}
