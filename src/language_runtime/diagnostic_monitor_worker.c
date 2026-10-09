/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_monitor_worker.c
 * PURPOSE: Keep language-server I/O on one worker while publishing only the latest accepted draft.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "diagnostic_monitor_internal.h"
#include <stdlib.h>
static DiagnosticMonitorDraft *DiagnosticMonitorNext(UmiLanguageDiagnosticMonitor *monitor)
{
    (void)umi_mutex_lock(monitor->mutex);
    DiagnosticMonitorDraft *draft = monitor->pending;
    monitor->pending = NULL;
    (void)umi_mutex_unlock(monitor->mutex);
    return draft;
}
static void DiagnosticMonitorState(UmiLanguageDiagnosticMonitor *monitor,
                                   UmiLanguageDiagnosticSession *session, uint64_t sent,
                                   UmiLanguageDiagnosticCatalogue **publication)
{
    UmiLanguageDiagnosticSessionSnapshot snapshot;
    (void)UmiLanguageDiagnosticSessionRead(session, &snapshot);
    UmiLanguageDiagnosticCatalogue *old = NULL;
    (void)umi_mutex_lock(monitor->mutex);
    monitor->snapshot.session = snapshot;
    monitor->snapshot.ready = snapshot.ready;
    monitor->snapshot.sent_sequence = sent;
    if (*publication != NULL && !monitor->snapshot.stop_requested &&
        sent == monitor->snapshot.accepted_sequence)
    {
        old = monitor->publication;
        monitor->publication = *publication;
        *publication = NULL;
        monitor->snapshot.publication_sequence = sent;
    }
    (void)umi_mutex_unlock(monitor->mutex);
    UmiLanguageDiagnosticCatalogueDestroy(old);
}
UmiStatus DiagnosticMonitorRun(UmiLanguageDiagnosticMonitor *monitor,
                               UmiLanguageRuntimeServer *server)
{
    if (monitor == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(monitor->mutex);
    if (monitor->entered)
    {
        (void)umi_mutex_unlock(monitor->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    monitor->entered = true;
    monitor->snapshot.started = 1;
    (void)umi_mutex_unlock(monitor->mutex);
    DiagnosticMonitorDraft *draft = DiagnosticMonitorNext(monitor);
    UmiStatus status = draft == NULL ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
    UmiLanguageDiagnosticSession *session = NULL;
    uint64_t sent = draft == NULL ? 0U : draft->sequence;
    if (status == UMI_STATUS_OK)
    {
        UmiLanguageDiagnosticRequest request = {monitor->root, monitor->uri, monitor->language,
                                                draft->text,   draft->bytes, monitor->timeout};
        status = server == NULL
                     ? UmiLanguageDiagnosticSessionStartNative(&monitor->profile,
                                                               monitor->directory, monitor->tools,
                                                               &request, monitor->cancel, &session)
                     : UmiLanguageDiagnosticSessionStartOnServer(server, &request, monitor->cancel,
                                                                 &session);
    }
    DiagnosticMonitorDraftFree(draft);
    UmiLanguageDiagnosticCatalogue *publication = NULL;
    while (status == UMI_STATUS_OK)
    {
        if (umi_cancellation_token_is_requested(monitor->cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        draft = DiagnosticMonitorNext(monitor);
        if (draft != NULL)
        {
            UmiLanguageDiagnosticSessionSnapshot snapshot;
            (void)UmiLanguageDiagnosticSessionRead(session, &snapshot);
            int32_t version = snapshot.document_version;
            status = UmiLanguageDiagnosticSessionUpdate(session, draft->text, draft->bytes, version,
                                                        &version);
            if (status == UMI_STATUS_OK)
                sent = draft->sequence;
            DiagnosticMonitorDraftFree(draft);
            if (status != UMI_STATUS_OK)
                break;
        }
        DiagnosticMonitorState(monitor, session, sent, &publication);
        status = UmiLanguageDiagnosticSessionPoll(session, 50U, monitor->cancel, &publication);
        if (status == UMI_STATUS_OK)
            DiagnosticMonitorState(monitor, session, sent, &publication);
        UmiLanguageDiagnosticCatalogueDestroy(publication);
        publication = NULL;
        if (status == UMI_STATUS_NOT_FOUND)
            status = UMI_STATUS_OK;
    }
    if (session != NULL)
    {
        UmiStatus cleanup = UmiLanguageDiagnosticSessionClose(session);
        DiagnosticMonitorState(monitor, session, sent, &publication);
        if (status == UMI_STATUS_OK)
            status = cleanup;
        UmiLanguageDiagnosticSessionDestroy(session);
    }
    (void)umi_mutex_lock(monitor->mutex);
    monitor->snapshot.status = status;
    monitor->snapshot.ready = 0;
    monitor->snapshot.completed = 1;
    publication = monitor->publication;
    monitor->publication = NULL;
    monitor->snapshot.publication_sequence = 0U;
    draft = monitor->pending;
    monitor->pending = NULL;
    (void)umi_mutex_unlock(monitor->mutex);
    UmiLanguageDiagnosticCatalogueDestroy(publication);
    DiagnosticMonitorDraftFree(draft);
    return status;
}
UmiStatus UmiLanguageDiagnosticMonitorRun(UmiLanguageDiagnosticMonitor *monitor)
{
    return DiagnosticMonitorRun(monitor, NULL);
}
UmiStatus UmiLanguageDiagnosticMonitorRunOnServer(UmiLanguageDiagnosticMonitor *monitor,
                                                  UmiLanguageRuntimeServer *server)
{
    if (server == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return DiagnosticMonitorRun(monitor, server);
}
