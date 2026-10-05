/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/call_query.h
 * PURPOSE: Request complete caller or callee relationships for one captured source position.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_CALL_QUERY_H
#define UMICOM_LANGUAGE_RUNTIME_CALL_QUERY_H
#include "umicom/language_runtime/call_catalogue.h"
#include "umicom/language_runtime/completion_query.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageCallRequest
    {
        UmiLanguageCompletionRequest source;
        UmiLanguageCallDirection direction;
    } UmiLanguageCallRequest;
    typedef struct UmiLanguageCallResult UmiLanguageCallResult;
    typedef struct UmiLanguageCallRelation
    {
        UmiLanguageCallItem root, related;
        size_t root_index, call_sites;
    } UmiLanguageCallRelation;
    /* Prepare the symbol at the captured caret, then request direct callers or
 * callees for every prepared root in the same temporary connection. Send each
 * complete original item, including opaque data. No recursive graph expansion,
 * persistent index, destination opening or source modification occurs.
 *
 * Accept at most 16 prepared roots, 4096 total relations and 16384 total call
 * sites. Reject excess instead of publishing a partial result. Root items with
 * no relations remain available through RootAt. Validate all ranges whose URI
 * exactly matches the captured document against its UTF-8 text; other source
 * destinations need validation by the document owner when explicitly opened.
 * Other unsaved documents are not synchronized by this operation.
 *
 * Require callHierarchyProvider and UTF-16 positions. Initialization, prepare
 * and all follow-up replies share the source.timeout_ms read budget; cleanup
 * has 250 ms. Native message and parser bounds apply to each exchange. Result
 * publication waits for didClose and shutdown. Cancellation is checked between
 * reads, not during launch or writes. Own the direct child, not descendants.
 * Inputs remain borrowed until return; failure clears output. Report.choices
 * counts relations, not roots or sites. No provider commands execute. */
    UmiStatus UmiLanguageCallQueryNative(const UmiLanguageServerProfile *profile,
                                         const char *working_directory, const UmiLanguageCallRequest *request,
                                         const UmiCancellationToken *cancel,
                                         UmiLanguageCompletionQueryReport *out_report,
                                         UmiLanguageCallResult **out_result);
    /* An exclusively owned STARTING server remains untouched on input preflight
 * failure. Initialization transfers cleanup to this call; destroy its owner
 * afterward. Do not supply a connection shared with other requests. */
    UmiStatus UmiLanguageCallQueryOnServer(UmiLanguageRuntimeServer *server,
                                           const UmiLanguageCallRequest *request,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageCompletionQueryReport *out_report,
                                           UmiLanguageCallResult **out_result);
    void UmiLanguageCallResultDestroy(UmiLanguageCallResult *result);
    size_t UmiLanguageCallResultRootCount(const UmiLanguageCallResult *result);
    size_t UmiLanguageCallResultCount(const UmiLanguageCallResult *result);
    /* Returned strings borrow result storage. Invalid input leaves outputs alone.
 * Relations keep root order and each provider's relation order. Destination
 * zero is the related symbol; one through call_sites are exact caller ranges. */
    UmiStatus UmiLanguageCallResultRootAt(const UmiLanguageCallResult *result, size_t index,
                                          UmiLanguageCallItem *out_root);
    UmiStatus UmiLanguageCallResultAt(const UmiLanguageCallResult *result, size_t index,
                                      UmiLanguageCallRelation *out_relation);
    UmiStatus UmiLanguageCallResultLocation(const UmiLanguageCallResult *result, size_t index,
                                            size_t destination, UmiLanguageSourceLocation *out_location);
#ifdef __cplusplus
}
#endif
#endif
