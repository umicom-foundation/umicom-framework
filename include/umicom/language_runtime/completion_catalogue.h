/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/completion_catalogue.h
 * PURPOSE: Retain completion choices and prepare revision-checked editor changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_COMPLETION_CATALOGUE_H
#define UMICOM_LANGUAGE_RUNTIME_COMPLETION_CATALOGUE_H
#include "umicom/editor/workspace_edit.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageCompletionCatalogue UmiLanguageCompletionCatalogue;
    typedef struct UmiLanguageCompletionPosition
    {
        uint32_t line, character;
    } UmiLanguageCompletionPosition;
    typedef struct UmiLanguageCompletionRange
    {
        UmiLanguageCompletionPosition start, end;
    } UmiLanguageCompletionRange;
    typedef struct UmiLanguageCompletionChoice
    {
        char label[256], detail[512], sort_text[256], filter_text[256];
        uint32_t kind, insert_text_format, insert_text_mode;
        size_t additional_edit_count;
        int has_text_edit, has_command;
    } UmiLanguageCompletionChoice;
    typedef enum UmiLanguageCompletionAcceptance
    {
        UMI_LANGUAGE_COMPLETION_INSERT = 1,
        UMI_LANGUAGE_COMPLETION_REPLACE = 2
    } UmiLanguageCompletionAcceptance;
    typedef struct UmiLanguageCompletionContext
    {
        const char *document_uri;
        const char *provider_id;
        uint64_t request_revision;
        UmiLanguageCompletionPosition request_position;
        /* The editor supplies its language-aware word range when the server has
     * not supplied an edit range. No guessing from punctuation happens here. */
        UmiLanguageCompletionRange fallback_range;
        UmiLanguageCompletionAcceptance acceptance;
    } UmiLanguageCompletionContext;

    /* Read the JSON result VALUE of a correlated completion response, not its
 * JSON-RPC envelope. Accept an array, CompletionList or null. The caller must
 * check the request identity before invoking this function. Own a copy, retain
 * all item JSON, and reject excess data rather than dropping choices.
 *
 * Limits: 1 MiB JSON, 131072 nodes, 64 levels and 2048 items. Display fields use
 * the capacities above, including NUL. Consumed duplicate fields are rejected.
 * Unknown metadata remains in the owned tree. Edit coordinates and replacement
 * text are checked when a choice is planned, allowing other choices to remain
 * usable. Cancellation is cooperative. Failure clears out_catalogue. */
    UmiStatus UmiLanguageCompletionCatalogueCreate(const void *result_json, size_t length,
                                                   const UmiCancellationToken *cancel,
                                                   UmiLanguageCompletionCatalogue **out_catalogue);

    /* Validate a complete JSON-RPC response and its numeric request identity before
 * reading its result. This convenience path has the same total JSON byte limit.
 * Request IDs must be positive and fit INT64_MAX. An unrelated ID returns
 * NOT_FOUND; a well-formed server error returns UNAVAILABLE. Notifications,
 * requests, malformed envelopes and a result/error combination are refused.
 * As with Create, failure clears the output and no model is changed. */
    UmiStatus UmiLanguageCompletionCatalogueReadResponse(const void *response_json, size_t length,
                                                         uint64_t expected_request_id,
                                                         const UmiCancellationToken *cancel,
                                                         UmiLanguageCompletionCatalogue **out_catalogue);
    void UmiLanguageCompletionCatalogueDestroy(UmiLanguageCompletionCatalogue *catalogue);
    size_t UmiLanguageCompletionCatalogueCount(const UmiLanguageCompletionCatalogue *catalogue);
    /* An incomplete list should be requested again as typing continues. */
    int UmiLanguageCompletionCatalogueIncomplete(const UmiLanguageCompletionCatalogue *catalogue);
    UmiStatus UmiLanguageCompletionCatalogueAt(const UmiLanguageCompletionCatalogue *catalogue, size_t index,
                                               UmiLanguageCompletionChoice *out_choice);

    /* Build an owned, resolved Editor edit set without modifying the buffer.
 * Coordinates are zero-based UTF-16 units, even when source uses UTF-8.
 * The captured request revision must still match the authoritative buffer.
 * TextEdit, InsertReplaceEdit, list editRange defaults and additionalTextEdits
 * are supported. Plain text is inserted as supplied. Snippets, indentation
 * adjustment and post-completion commands return NOT_IMPLEMENTED so that no
 * requested behavior is silently discarded. A host using this path should
 * advertise only these supported capabilities.
 *
 * All edits target context.document_uri. At most 64 additional edits and
 * 511 UTF-8 bytes per replacement/expected span are supported by this bridge.
 * Overlapping edits (including duplicate insertion positions) are refused.
 * Failure clears out_edits and leaves the buffer unchanged. The caller owns
 * the result; apply with require_matching_revision=1 after user acceptance.
 * Confine buffer access to its owning thread for both planning and applying. */
    UmiStatus UmiLanguageCompletionPlanCreate(const UmiLanguageCompletionCatalogue *catalogue, size_t index,
                                              const UmiLanguageCompletionContext *context,
                                              const UmiEditorTextBuffer *buffer,
                                              const UmiCancellationToken *cancel,
                                              UmiEditorWorkspaceEditSet **out_edits);
#ifdef __cplusplus
}
#endif
#endif
