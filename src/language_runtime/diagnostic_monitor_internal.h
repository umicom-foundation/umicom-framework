/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_monitor_internal.h
 * PURPOSE: Keep the pending draft and publication transfer under one short-held mutex.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_MONITOR_INTERNAL_H
#define UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_MONITOR_INTERNAL_H
#include "diagnostic_session_internal.h"
#include "umicom/language_runtime/diagnostic_monitor.h"
#include "umicom/platform/threading.h"
typedef struct DiagnosticMonitorDraft
{
    char *text;
    size_t bytes;
    uint64_t sequence;
} DiagnosticMonitorDraft;
struct UmiLanguageDiagnosticMonitor
{
    UmiMutex *mutex;
    UmiCancellationToken *cancel;
    UmiLanguageServerProfile profile;
    char directory[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY], tools[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];
    char root[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY], uri[UMI_EDITOR_SOURCE_URI_CAPACITY],
        language[128];
    uint32_t timeout;
    bool entered;
    char *accepted;
    size_t accepted_bytes;
    DiagnosticMonitorDraft *pending;
    UmiLanguageDiagnosticCatalogue *publication;
    UmiLanguageDiagnosticMonitorSnapshot snapshot;
};
void DiagnosticMonitorDraftFree(DiagnosticMonitorDraft *draft);
UmiStatus DiagnosticMonitorRun(UmiLanguageDiagnosticMonitor *monitor,
                               UmiLanguageRuntimeServer *server);
#endif
