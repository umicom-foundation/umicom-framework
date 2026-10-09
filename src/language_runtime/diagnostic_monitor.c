/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_monitor.c
 * PURPOSE: Copy editor drafts and retire superseded diagnostics without sharing mutable editor state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "diagnostic_monitor_internal.h"
#include "server_profile_internal.h"
#include "umicom/base/text.h"
#include "umicom/platform/path.h"
#include <stdlib.h>
#include <string.h>
void DiagnosticMonitorDraftFree(DiagnosticMonitorDraft *draft)
{
    if (draft != NULL)
    {
        free(draft->text);
        free(draft);
    }
}
UmiStatus UmiLanguageDiagnosticMonitorCreate(const UmiLanguageDiagnosticMonitorConfig *config,
                                             UmiLanguageDiagnosticMonitor **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (config == NULL || LanguageProfileText(&config->profile) != UMI_STATUS_OK ||
        !umi_path_is_absolute(config->working_directory) ||
        (config->tool_directory != NULL && config->tool_directory[0] != '\0' &&
         !umi_path_is_absolute(config->tool_directory)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!config->profile.enabled)
        return UMI_STATUS_UNAVAILABLE;
    UmiLanguageDiagnosticSession *prepared = NULL;
    DiagnosticSessionWire *wire = NULL;
    UmiStatus status = DiagnosticSessionPrepare(&config->document, &prepared, &wire);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageDiagnosticMonitor *monitor = calloc(1U, sizeof *monitor);
    if (monitor == NULL)
    {
        free(prepared->source);
        free(prepared);
        free(wire);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    monitor->profile = config->profile;
    monitor->timeout = config->document.timeout_ms;
    strcpy(monitor->root, prepared->root);
    strcpy(monitor->uri, prepared->uri);
    strcpy(monitor->language, prepared->language);
    status =
        umi_text_copy(monitor->directory, sizeof monitor->directory, config->working_directory);
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(monitor->tools, sizeof monitor->tools,
                               config->tool_directory == NULL ? "" : config->tool_directory);
    if (status == UMI_STATUS_OK)
        status = umi_mutex_create(&monitor->mutex);
    if (status == UMI_STATUS_OK)
        status = umi_cancellation_token_create(&monitor->cancel);
    uint64_t sequence = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageDiagnosticMonitorSubmit(monitor, prepared->source, prepared->bytes,
                                                    &sequence);
    free(prepared->source);
    free(prepared);
    free(wire);
    if (status != UMI_STATUS_OK)
    {
        (void)UmiLanguageDiagnosticMonitorDestroy(&monitor);
        return status;
    }
    *out = monitor;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageDiagnosticMonitorSubmit(UmiLanguageDiagnosticMonitor *monitor,
                                             const char *source, size_t bytes,
                                             uint64_t *out_sequence)
{
    if (monitor == NULL || source == NULL || out_sequence == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preparing a source never holds the publication mutex. Large escaped
     * documents can be refused without stalling the protocol worker. */
    UmiLanguageDiagnosticRequest request = {monitor->root, monitor->uri, monitor->language,
                                            source,        bytes,        monitor->timeout};
    UmiLanguageDiagnosticSession *prepared = NULL;
    DiagnosticSessionWire *wire = NULL;
    UmiStatus status = DiagnosticSessionPrepare(&request, &prepared, &wire);
    if (status != UMI_STATUS_OK)
        return status;
    free(wire);
    DiagnosticMonitorDraft *draft = calloc(1U, sizeof *draft);
    char *accepted = malloc(bytes + 1U);
    if (draft == NULL || accepted == NULL)
    {
        free(draft);
        free(accepted);
        free(prepared->source);
        free(prepared);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    draft->text = prepared->source;
    draft->bytes = bytes;
    prepared->source = NULL;
    free(prepared);
    memcpy(accepted, source, bytes);
    accepted[bytes] = '\0';
    DiagnosticMonitorDraft *old = NULL;
    UmiLanguageDiagnosticCatalogue *old_publication = NULL;
    char *old_accepted = NULL;
    (void)umi_mutex_lock(monitor->mutex);
    if (monitor->snapshot.completed || monitor->snapshot.stop_requested)
        status = UMI_STATUS_INVALID_STATE;
    else if (monitor->accepted != NULL && bytes == monitor->accepted_bytes &&
             memcmp(source, monitor->accepted, bytes) == 0)
        *out_sequence = monitor->snapshot.accepted_sequence;
    else if (monitor->snapshot.accepted_sequence == UINT64_MAX)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    else
    {
        draft->sequence = ++monitor->snapshot.accepted_sequence;
        old = monitor->pending;
        monitor->pending = draft;
        draft = NULL;
        old_accepted = monitor->accepted;
        monitor->accepted = accepted;
        accepted = NULL;
        monitor->accepted_bytes = bytes;
        old_publication = monitor->publication;
        monitor->publication = NULL;
        monitor->snapshot.publication_sequence = 0U;
        if (old != NULL && monitor->snapshot.coalesced_updates != UINT64_MAX)
            ++monitor->snapshot.coalesced_updates;
        *out_sequence = monitor->snapshot.accepted_sequence;
    }
    (void)umi_mutex_unlock(monitor->mutex);
    DiagnosticMonitorDraftFree(old);
    DiagnosticMonitorDraftFree(draft);
    free(old_accepted);
    free(accepted);
    UmiLanguageDiagnosticCatalogueDestroy(old_publication);
    return status;
}
UmiStatus UmiLanguageDiagnosticMonitorTake(UmiLanguageDiagnosticMonitor *monitor,
                                           uint64_t *out_sequence,
                                           UmiLanguageDiagnosticCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (monitor == NULL || out_sequence == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(monitor->mutex);
    UmiStatus status = monitor->publication == NULL ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
    {
        *out = monitor->publication;
        monitor->publication = NULL;
        *out_sequence = monitor->snapshot.publication_sequence;
    }
    (void)umi_mutex_unlock(monitor->mutex);
    return status;
}
UmiStatus UmiLanguageDiagnosticMonitorRead(UmiLanguageDiagnosticMonitor *monitor,
                                           UmiLanguageDiagnosticMonitorSnapshot *out)
{
    if (monitor == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(monitor->mutex);
    *out = monitor->snapshot;
    (void)umi_mutex_unlock(monitor->mutex);
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageDiagnosticMonitorStop(UmiLanguageDiagnosticMonitor *monitor)
{
    if (monitor == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(monitor->mutex);
    monitor->snapshot.stop_requested = 1;
    UmiLanguageDiagnosticCatalogue *publication = monitor->publication;
    monitor->publication = NULL;
    monitor->snapshot.publication_sequence = 0U;
    umi_cancellation_token_request(monitor->cancel);
    (void)umi_mutex_unlock(monitor->mutex);
    UmiLanguageDiagnosticCatalogueDestroy(publication);
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageDiagnosticMonitorDestroy(UmiLanguageDiagnosticMonitor **owner)
{
    if (owner == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageDiagnosticMonitor *monitor = *owner;
    if (monitor == NULL)
        return UMI_STATUS_OK;
    if (monitor->mutex != NULL)
    {
        (void)umi_mutex_lock(monitor->mutex);
        bool active = monitor->entered && !monitor->snapshot.completed;
        (void)umi_mutex_unlock(monitor->mutex);
        if (active)
            return UMI_STATUS_BUSY;
    }
    DiagnosticMonitorDraftFree(monitor->pending);
    free(monitor->accepted);
    UmiLanguageDiagnosticCatalogueDestroy(monitor->publication);
    umi_cancellation_token_destroy(monitor->cancel);
    umi_mutex_destroy(monitor->mutex);
    free(monitor);
    *owner = NULL;
    return UMI_STATUS_OK;
}
