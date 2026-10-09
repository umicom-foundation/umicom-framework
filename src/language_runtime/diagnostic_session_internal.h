/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_session_internal.h
 * PURPOSE: Share bounded session state between synchronization, protocol and diagnostic handling.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_SESSION_INTERNAL_H
#define UMICOM_LANGUAGE_RUNTIME_DIAGNOSTIC_SESSION_INTERNAL_H
/* Session URI storage uses the shared editor source-location contract. Include
 * its owner directly so this internal header keeps the established bound without
 * relying on an unrelated consumer to include it first. */
#include "umicom/editor/source_location.h"
#include "umicom/editor/text_position.h"
#include "umicom/language_runtime/diagnostic_session.h"
#include "umicom/platform/clock.h"
struct UmiLanguageDiagnosticSession
{
    UmiLanguageRuntimeServer *server;
    int owns_server, sync_kind;
    char root[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];
    char uri[UMI_EDITOR_SOURCE_URI_CAPACITY], language[128];
    char *source;
    size_t bytes;
    UmiLanguageDiagnosticSessionSnapshot snapshot;
};
typedef struct DiagnosticSessionWire
{
    char initialize[8192], document[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY];
} DiagnosticSessionWire;
UmiStatus DiagnosticSessionSource(const char *text, size_t bytes, UmiEditorTextPosition *end);
UmiStatus DiagnosticSessionDocument(const UmiLanguageDiagnosticSession *session, const char *text,
                                    size_t bytes, int32_t version, int opening, char *out);
UmiStatus DiagnosticSessionPrepare(const UmiLanguageDiagnosticRequest *request,
                                   UmiLanguageDiagnosticSession **out,
                                   DiagnosticSessionWire **wire);
UmiStatus DiagnosticSessionInitialize(UmiLanguageDiagnosticSession *session,
                                      const DiagnosticSessionWire *wire, uint32_t timeout,
                                      const UmiCancellationToken *cancel);
UmiStatus DiagnosticSessionFail(UmiLanguageDiagnosticSession *session, UmiStatus status);
#endif
