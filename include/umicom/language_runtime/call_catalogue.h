/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/call_catalogue.h
 * PURPOSE: Own complete language-server call items, related symbols and their individual call sites.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_CALL_CATALOGUE_H
#define UMICOM_LANGUAGE_RUNTIME_CALL_CATALOGUE_H
#include "umicom/language_runtime/location_catalogue.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum UmiLanguageCallDirection
    {
        UMI_LANGUAGE_CALL_INCOMING = 1,
        UMI_LANGUAGE_CALL_OUTGOING = 2
    } UmiLanguageCallDirection;
    typedef struct UmiLanguageCallItem
    {
        const char *name, *detail;
        UmiLanguageSourceLocation location;
        int32_t kind;
        int deprecated, has_data;
    } UmiLanguageCallItem;
    typedef struct UmiLanguageCallItemCatalogue UmiLanguageCallItemCatalogue;
    typedef struct UmiLanguageCallEdgeCatalogue UmiLanguageCallEdgeCatalogue;
    typedef struct UmiLanguageCallEdge
    {
        UmiLanguageCallItem related;
        size_t call_sites;
    } UmiLanguageCallEdge;
    /* Own a complete prepareCallHierarchy result, including opaque data and unknown
 * extension properties. Null and empty results are valid empty catalogues.
 * Every symbol needs a visible name, positive kind, URI and ordered ranges;
 * selectionRange must fit range. Retain unknown positive kinds and tags.
 * Bounds: 4096 items, 4096 decoded bytes per label, 8192 URI bytes, 1 MiB JSON.
 * Duplicate required fields, malformed input, capacity failure or cancellation
 * reject the whole result. No source is read or edited; output clears on error. */
    UmiStatus UmiLanguageCallItemCatalogueCreate(const void *json, size_t bytes,
                                                 const UmiCancellationToken *cancel,
                                                 UmiLanguageCallItemCatalogue **out_catalogue);
    UmiStatus UmiLanguageCallItemCatalogueReadResponse(const void *json, size_t bytes, uint64_t expected_id,
                                                       const UmiCancellationToken *cancel,
                                                       UmiLanguageCallItemCatalogue **out_catalogue);
    void UmiLanguageCallItemCatalogueDestroy(UmiLanguageCallItemCatalogue *catalogue);
    size_t UmiLanguageCallItemCatalogueCount(const UmiLanguageCallItemCatalogue *catalogue);
    /* Metadata copies borrow text from the catalogue. Invalid indices leave output
 * unchanged. ItemJson borrows the entire original object, not a reconstructed
 * subset; a host can return it to the same server session that prepared it. */
    UmiStatus UmiLanguageCallItemCatalogueAt(const UmiLanguageCallItemCatalogue *catalogue, size_t index,
                                             UmiLanguageCallItem *out_item);
    UmiStatus UmiLanguageCallItemCatalogueItemJson(const UmiLanguageCallItemCatalogue *catalogue,
                                                   size_t index, const char **out_json, size_t *out_bytes);
    /* Own incomingCalls or outgoingCalls for one prepared root. Every fromRanges
 * entry remains associated with its caller URI: related.from.uri for incoming
 * calls, root.location.uri for outgoing calls. Copy the root URI so roots may
 * be destroyed independently. Empty call-site arrays and repeated ranges remain
 * in server order; do not invent a location when no call site was returned.
 * Ordered coordinates are required; current source and identifier containment
 * are not inferred from server text. Bounds: 4096 edges and 16384 total sites.
 * Other ownership, JSON size, cancellation and failure rules match items. */
    UmiStatus UmiLanguageCallEdgeCatalogueReadResponse(const void *json, size_t bytes, uint64_t expected_id,
                                                       const UmiLanguageCallItem *root,
                                                       UmiLanguageCallDirection direction,
                                                       const UmiCancellationToken *cancel,
                                                       UmiLanguageCallEdgeCatalogue **out_catalogue);
    void UmiLanguageCallEdgeCatalogueDestroy(UmiLanguageCallEdgeCatalogue *catalogue);
    size_t UmiLanguageCallEdgeCatalogueCount(const UmiLanguageCallEdgeCatalogue *catalogue);
    UmiStatus UmiLanguageCallEdgeCatalogueAt(const UmiLanguageCallEdgeCatalogue *catalogue, size_t index,
                                             UmiLanguageCallEdge *out_edge);
    /* Destination zero is the related symbol; one through call_sites select the
 * individual call sites. A call site's target and selection are its exact range.
 * The returned URI borrows catalogue storage. This function never navigates. */
    UmiStatus UmiLanguageCallEdgeCatalogueLocation(const UmiLanguageCallEdgeCatalogue *catalogue,
                                                   size_t index, size_t destination,
                                                   UmiLanguageSourceLocation *out_location);
#ifdef __cplusplus
}
#endif
#endif
