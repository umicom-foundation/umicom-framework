/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/code_action_query.h
 * PURPOSE: Request complete code actions for an exact captured source range.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_CODE_ACTION_QUERY_H
#define UMICOM_LANGUAGE_RUNTIME_CODE_ACTION_QUERY_H
#include "umicom/language_runtime/code_action_catalogue.h"
#include "umicom/language_runtime/server_manager.h"
#include "umicom/language_runtime/query_sources.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageCodeActionRequest
    {
        const char *root_uri, *document_uri, *language_id, *source;
        size_t source_bytes, range_start, range_end;
        uint32_t timeout_ms;
    } UmiLanguageCodeActionRequest;
    typedef struct UmiLanguageCodeActionReport
    {
        int started, initialized, document_opened;
        UmiStatus query_status, close_status, shutdown_status;
        size_t actions;
    } UmiLanguageCodeActionReport;
    /* Request actions for an ordered UTF-8 byte range, including an empty caret
 * range. Send the complete captured draft and convert both endpoints exactly
 * to UTF-16. The explicit context has an empty diagnostics array: this query
 * does not claim to synchronize diagnostics from an earlier editor session.
 * Some quick fixes therefore depend on server-side analysis of the range.
 *
 * Return complete owned actions, including disabled choices and commands for
 * inspection. Never execute commands, resolve lazy actions, apply workspace
 * edits or save files. A selected edit still needs exact document/version
 * checks, annotation review and approval through the document owner. Other
 * unsaved drafts are not synchronized by this temporary request.
 *
 * Keep inputs alive until return. Failure clears the catalogue output. The
 * shared native message limit is 65536 bytes including escaped source and
 * envelope; native parser bounds may be below catalogue bounds. timeout_ms
 * covers initialization and response reads, with another 250 ms for cleanup.
 * Cancellation is checked between reads, not during launch or writes. Only
 * the direct child is owned; a selected server can read the project and launch
 * descendants. Results publish only after didClose and shutdown succeed. */
    UmiStatus UmiLanguageCodeActionQueryNative(const UmiLanguageServerProfile *profile,
                                               const char *working_directory,
                                               const UmiLanguageCodeActionRequest *request,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageCodeActionReport *out_report,
                                               UmiLanguageCodeActionCatalogue **out_catalogue);
    /* Exclusively owned STARTING server. Invalid preflight leaves it untouched;
 * after initialization starts, the query owns cleanup. Destroy the server
 * object after return, including when the query fails. */
    UmiStatus UmiLanguageCodeActionQueryOnServer(UmiLanguageRuntimeServer *server,
                                                 const UmiLanguageCodeActionRequest *request,
                                                 const UmiCancellationToken *cancel,
                                                 UmiLanguageCodeActionReport *out_report,
                                                 UmiLanguageCodeActionCatalogue **out_catalogue);
    /* Opt in to diagnostic-backed actions. After opening the captured draft, wait
 * for its first matching push-diagnostic publication, validate all its ranges,
 * and include relevant original diagnostic objects in the action context.
 * Opaque diagnostic data remains in the same server session; callers cannot
 * inject cached data from a different process. Initialization, publication and
 * actions share the request's timeout. No matching publication means failure,
 * not a silent fallback to an empty context. Other action and cleanup limits
 * are identical to the basic query. This does not subscribe continuously. */
    UmiStatus UmiLanguageCodeActionQueryNativeWithDiagnostics(const UmiLanguageServerProfile *profile,
                                                              const char *working_directory,
                                                              const UmiLanguageCodeActionRequest *request,
                                                              const UmiCancellationToken *cancel,
                                                              UmiLanguageCodeActionReport *out_report,
                                                              UmiLanguageCodeActionCatalogue **out_catalogue);
    UmiStatus UmiLanguageCodeActionQueryOnServerWithDiagnostics(
        UmiLanguageRuntimeServer *server, const UmiLanguageCodeActionRequest *request,
        const UmiCancellationToken *cancel, UmiLanguageCodeActionReport *out_report,
        UmiLanguageCodeActionCatalogue **out_catalogue);

    /* Synchronize additional captured drafts before asking for actions. The
     * primary source plus additions are bounded to 64 distinct exact URIs.
     * include_diagnostics must be 0 or 1. When enabled, obtain fresh primary
     * diagnostics in this same connection after all supplied drafts are opened.
     * Other draft diagnostics are not substituted for the primary context.
     *
     * Source preflight, timeout, cancellation and complete cleanup follow the
     * existing query contracts. This call owns no editor state and grants no
     * permission to apply an action. Decode only an explicitly selected edit,
     * then validate its entire workspace proposal against the original captures
     * with the source-review service. Commands and unresolved actions remain
     * unavailable for application; no edit subset is executed automatically. */
    UmiStatus UmiLanguageCodeActionQueryNativeWithSources(
        const UmiLanguageServerProfile *profile, const char *working_directory,
        const UmiLanguageCodeActionRequest *request, const UmiLanguageQuerySource *additional_sources,
        size_t source_count, int include_diagnostics, const UmiCancellationToken *cancel,
        UmiLanguageCodeActionReport *out_report, UmiLanguageCodeActionCatalogue **out_catalogue);
    UmiStatus UmiLanguageCodeActionQueryOnServerWithSources(UmiLanguageRuntimeServer *server,
                                                            const UmiLanguageCodeActionRequest *request,
                                                            const UmiLanguageQuerySource *additional_sources,
                                                            size_t source_count, int include_diagnostics,
                                                            const UmiCancellationToken *cancel,
                                                            UmiLanguageCodeActionReport *out_report,
                                                            UmiLanguageCodeActionCatalogue **out_catalogue);
    /* Choose how this connection obtains diagnostic context. NONE sends an empty
 * context; NOTIFICATION waits for a matching publication; PULL requests a full
 * diagnostic report from the negotiated provider. Never import opaque data
 * from another connection. These modes also support explicit additional sources. */
    typedef enum UmiLanguageActionDiagnosticMode
    {
        UMI_LANGUAGE_ACTION_DIAGNOSTICS_NONE,
        UMI_LANGUAGE_ACTION_DIAGNOSTICS_NOTIFICATION,
        UMI_LANGUAGE_ACTION_DIAGNOSTICS_PULL
    } UmiLanguageActionDiagnosticMode;
    /* The pull mode requires both action and diagnostic capabilities. Every source
 * is synchronized before diagnostics are requested. Diagnostic range validation,
 * context filtering and action lookup share this connection and read budget.
 * No unsupported mode falls back silently. Apply still requires the complete
 * editor review described above; these functions do not modify documents. */
    UmiStatus UmiLanguageCodeActionQueryNativeWithContext(
        const UmiLanguageServerProfile *profile, const char *working_directory,
        const UmiLanguageCodeActionRequest *request, const UmiLanguageQuerySource *additional_sources,
        size_t source_count, UmiLanguageActionDiagnosticMode diagnostic_mode,
        const UmiCancellationToken *cancel, UmiLanguageCodeActionReport *out_report,
        UmiLanguageCodeActionCatalogue **out_catalogue);
    UmiStatus UmiLanguageCodeActionQueryOnServerWithContext(
        UmiLanguageRuntimeServer *server, const UmiLanguageCodeActionRequest *request,
        const UmiLanguageQuerySource *additional_sources, size_t source_count,
        UmiLanguageActionDiagnosticMode diagnostic_mode, const UmiCancellationToken *cancel,
        UmiLanguageCodeActionReport *out_report, UmiLanguageCodeActionCatalogue **out_catalogue);
    /* Explicit request options compose source synchronization, diagnostic context
 * and action-family selection. A zero-initialized object requests all actions
 * with empty diagnostic context and no additional drafts. */
    typedef struct UmiLanguageCodeActionQueryOptions
    {
        const UmiLanguageQuerySource *additional_sources;
        size_t source_count;
        UmiLanguageActionDiagnosticMode diagnostics;
        UmiLanguageCodeActionFilter filter;
    } UmiLanguageCodeActionQueryOptions;
    /* options is required. A specific filter sends context.only and returns only
 * complete actions in that family, including dot-delimited descendants. All
 * actions are validated before filtering. Existing APIs keep their unfiltered
 * behavior. Filtering never executes a command, resolves an action or applies
 * an edit; the full review and source-ownership rules above still apply. */
    UmiStatus UmiLanguageCodeActionQueryNativeWithOptions(const UmiLanguageServerProfile *profile,
                                                          const char *working_directory,
                                                          const UmiLanguageCodeActionRequest *request,
                                                          const UmiLanguageCodeActionQueryOptions *options,
                                                          const UmiCancellationToken *cancel,
                                                          UmiLanguageCodeActionReport *out_report,
                                                          UmiLanguageCodeActionCatalogue **out_catalogue);
    UmiStatus UmiLanguageCodeActionQueryOnServerWithOptions(UmiLanguageRuntimeServer *server,
                                                            const UmiLanguageCodeActionRequest *request,
                                                            const UmiLanguageCodeActionQueryOptions *options,
                                                            const UmiCancellationToken *cancel,
                                                            UmiLanguageCodeActionReport *out_report,
                                                            UmiLanguageCodeActionCatalogue **out_catalogue);
    /* Explicitly resolve missing edit properties before closing the original
 * query session. Requires options and a maximum_actions limit from 1 through
 * 32. Filtering occurs first. Preflight the total eligible count before any
 * resolve request; exceeding the limit fails instead of returning a subset.
 *
 * Requires the provider's resolveProvider capability, even when no eligible
 * actions remain. Eligible rows are enabled, have no edit and require no
 * command. Send their complete original objects, including opaque data, and
 * accept only replies adding edit while preserving all other metadata. Rows
 * already complete, disabled or requiring commands remain unchanged. A failed
 * resolution rejects the whole publication. No commands or edits execute.
 *
 * Initialization, diagnostics, action listing and resolution share the same
 * timeout. Source synchronization, cancellation and cleanup follow the other
 * query contracts. Results still need complete source review and approval. */
    UmiStatus UmiLanguageCodeActionQueryNativeWithResolution(
        const UmiLanguageServerProfile *profile, const char *working_directory,
        const UmiLanguageCodeActionRequest *request, const UmiLanguageCodeActionQueryOptions *options,
        size_t maximum_actions, const UmiCancellationToken *cancel, UmiLanguageCodeActionReport *out_report,
        UmiLanguageCodeActionCatalogue **out_catalogue);
    UmiStatus UmiLanguageCodeActionQueryOnServerWithResolution(
        UmiLanguageRuntimeServer *server, const UmiLanguageCodeActionRequest *request,
        const UmiLanguageCodeActionQueryOptions *options, size_t maximum_actions,
        const UmiCancellationToken *cancel, UmiLanguageCodeActionReport *out_report,
        UmiLanguageCodeActionCatalogue **out_catalogue);
#ifdef __cplusplus
}
#endif
#endif
