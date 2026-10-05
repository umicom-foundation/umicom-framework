/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/navigation_query.h
 * PURPOSE: Request complete definition and reference locations through a temporary owned language connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_NAVIGATION_QUERY_H
#define UMICOM_LANGUAGE_RUNTIME_NAVIGATION_QUERY_H
#include "umicom/language_runtime/location_catalogue.h"
#include "umicom/language_runtime/server_manager.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum UmiLanguageNavigationKind
    {
        UMI_LANGUAGE_NAVIGATION_DEFINITION,
        UMI_LANGUAGE_NAVIGATION_REFERENCES,
        /* These values extend navigation without changing existing enum values.
         * The server decides which type or implementation owns the caret symbol. */
        UMI_LANGUAGE_NAVIGATION_TYPE_DEFINITION,
        UMI_LANGUAGE_NAVIGATION_IMPLEMENTATION,
        UMI_LANGUAGE_NAVIGATION_SELECTION_RANGES,
        UMI_LANGUAGE_NAVIGATION_FOLDING_RANGES
    } UmiLanguageNavigationKind;
    typedef struct UmiLanguageNavigationRequest
    {
        const char *root_uri, *document_uri, *language_id, *source;
        size_t source_bytes, caret;
        uint32_t timeout_ms;
        UmiLanguageNavigationKind kind;
        int include_declaration;
    } UmiLanguageNavigationRequest;
    typedef struct UmiLanguageNavigationReport
    {
        int started, initialized, document_opened;
        UmiStatus query_status, close_status, shutdown_status;
        size_t locations;
    } UmiLanguageNavigationReport;
    /* Type definitions and implementations accept the same complete location
     * and location-link shapes as definitions. Their capabilities are checked
     * independently; an unavailable operation never falls back to definitions. */
    /* Selection ranges request enclosing syntax at one captured caret. The
     * result uses the captured URI for every row, ordered from the smallest
     * range through enclosing parents. All endpoints and containment are
     * checked before publication. No text selection happens until the host
     * explicitly navigates to a chosen row. There is no lexical fallback. */
    /* Folding queries discover complete-line regions for the whole captured
     * document. The caret is validated but is not sent as a folding parameter.
     * Returned ranges cover entire lines and remain in server order; they do
     * not select or collapse anything automatically. Native queries share the
     * same bounded transport and explicit process ownership described below. */
    /* Query the symbol at an exact UTF-8 caret using negotiated UTF-16 positions.
 * Copy source and settings before dispatching to another thread. Definitions
 * accept complete location links; references require an ordinary location
 * array. include_declaration must be 0 or 1 and applies only to references.
 * No returned URI is opened and no document or file is edited by this query.
 * Validate target ranges again against their current drafts before navigation.
 *
 * Native messages must fit 65536 bytes including escaped source and envelope,
 * and the existing native parser token limit. One elapsed read budget covers
 * initialization and the reply; cleanup has a separate 250 ms shutdown budget.
 * Cancellation is observed between reads, not during launch/synchronous writes.
 * Close and stop the direct child before publishing owned locations. A server
 * may inspect the project folder and start descendants outside this ownership.
 * Failure clears out_catalogue. started means initialization was attempted. */
    UmiStatus UmiLanguageNavigationQueryNative(const UmiLanguageServerProfile *profile,
                                               const char *working_directory,
                                               const UmiLanguageNavigationRequest *request,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageNavigationReport *out_report,
                                               UmiLanguageLocationCatalogue **out_catalogue);
    /* Use an exclusively owned STARTING connection. Preflight errors leave it
 * untouched; once initialization begins this call always owns its cleanup.
 * The caller still destroys the server object after this function returns. */
    UmiStatus UmiLanguageNavigationQueryOnServer(UmiLanguageRuntimeServer *server,
                                                 const UmiLanguageNavigationRequest *request,
                                                 const UmiCancellationToken *cancel,
                                                 UmiLanguageNavigationReport *out_report,
                                                 UmiLanguageLocationCatalogue **out_catalogue);
#ifdef __cplusplus
}
#endif
#endif
