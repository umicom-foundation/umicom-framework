/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/location_catalogue.h
 * PURPOSE: Own complete source navigation locations and preserve link ranges without partial results.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_LOCATION_CATALOGUE_H
#define UMICOM_LANGUAGE_RUNTIME_LOCATION_CATALOGUE_H
#include "umicom/editor/text_position.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageSourceRange
    {
        UmiEditorTextPosition start, end;
    } UmiLanguageSourceRange;
    typedef struct UmiLanguageSourceLocation
    {
        const char *uri;
        UmiLanguageSourceRange target, selection, origin;
        int is_link, has_origin;
    } UmiLanguageSourceLocation;
    typedef struct UmiLanguageLocationCatalogue UmiLanguageLocationCatalogue;
    /* Read a Location, a homogeneous Location/LocationLink array, or null. A link
 * requires its complete target range and a selection contained within it.
 * Optional origin ranges remain attached to their original result.
 * Positions must be nonnegative protocol integers with ordered endpoints.
 * URI text must have a scheme, valid percent escapes and no raw whitespace,
 * control characters or backslashes. No URI is opened or resolved here.
 *
 * Own at most 4096 results, 8192 UTF-8 bytes per URI and 1 MiB JSON input.
 * Reject an invalid entry or excess capacity as a complete failure, rather
 * than dropping entries or returning a misleading truncated list.
 * Failure clears out_catalogue. Strings live until Destroy. */
    UmiStatus UmiLanguageLocationCatalogueCreate(const void *json, size_t bytes,
                                                 const UmiCancellationToken *cancel,
                                                 UmiLanguageLocationCatalogue **out_catalogue);
    /* Validate a positive numeric response ID and the JSON-RPC result/error
 * envelope before decoding. The byte limit includes the envelope. */
    UmiStatus UmiLanguageLocationCatalogueReadResponse(const void *json, size_t bytes, uint64_t expected_id,
                                                       const UmiCancellationToken *cancel,
                                                       UmiLanguageLocationCatalogue **out_catalogue);
    void UmiLanguageLocationCatalogueDestroy(UmiLanguageLocationCatalogue *catalogue);
    size_t UmiLanguageLocationCatalogueCount(const UmiLanguageLocationCatalogue *catalogue);
    /* Copy metadata and borrow the URI. Invalid arguments or indices preserve
 * output. Ranges are not checked against file contents by this reader.
 * A navigation host must resolve supported schemes and validate the target
 * against its current draft before moving the caret. */
    UmiStatus UmiLanguageLocationCatalogueAt(const UmiLanguageLocationCatalogue *catalogue, size_t index,
                                             UmiLanguageSourceLocation *out_location);
    /* Read a correlated selectionRange response for one requested position. A
 * non-null result must have exactly one selection tree. Return its complete
 * ranges from the smallest selection through each enclosing parent as ordinary
 * locations in document_uri. The first range contains position, and every
 * parent contains its child; equal ranges are retained in server order.
 *
 * Own copied URIs and metadata, bounded to 64 ranges and 1 MiB response JSON.
 * No source is read, selected or edited. Callers must validate every endpoint
 * against captured text before navigation. Null results return an empty owner;
 * malformed trees, excess depth or cancellation clear output atomically. */
    UmiStatus UmiLanguageLocationCatalogueReadSelectionResponse(const void *json, size_t bytes,
                                                                uint64_t expected_request_id,
                                                                const char *document_uri,
                                                                UmiEditorTextPosition position,
                                                                const UmiCancellationToken *cancel,
                                                                UmiLanguageLocationCatalogue **out_catalogue);
    /* Read complete-line folding regions against captured UTF-8 source. Protocol
     * lines are zero-based and inclusive. Locations cover complete source lines:
     * start is column zero; end is the following line start, or the source end.
     * This makes a selected region usable by an editor's explicit fold command.
     * A single-line region is retained but cannot hide a body below its header.
     *
     * Character offsets are type-checked but ignored, as negotiated by
     * lineFoldingOnly. Kind and collapsedText are checked as optional strings;
     * this location view does not expose categories or custom folded labels.
     * Preserve input order and duplicates. Bound results to 4096 regions,
     * 1 MiB response JSON and 16 MiB source. Failure clears output, including
     * any invalid region, missing source line, excess capacity or cancellation.
     * No file is read and no editor state changes. Captured source is borrowed
     * only until return; the result owns its URI and position metadata. */
    UmiStatus UmiLanguageLocationCatalogueReadFoldingResponse(const void *json, size_t bytes,
        uint64_t expected_request_id, const char *document_uri, const char *source, size_t source_bytes,
        const UmiCancellationToken *cancel, UmiLanguageLocationCatalogue **out_catalogue);
#ifdef __cplusplus
}
#endif
#endif
