/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/symbol_catalogue.h
 * PURPOSE: Own hierarchical document symbols and flat symbol information for source outlines.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_SYMBOL_CATALOGUE_H
#define UMICOM_LANGUAGE_RUNTIME_SYMBOL_CATALOGUE_H
#include "umicom/language_runtime/location_catalogue.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageSymbol
    {
        const char *name, *detail, *container;
        UmiLanguageSourceLocation location;
        size_t parent, depth;
        int32_t kind;
        int deprecated, hierarchical;
    } UmiLanguageSymbol;
    typedef struct UmiLanguageSymbolCatalogue UmiLanguageSymbolCatalogue;
    /* Read DocumentSymbol[] or SymbolInformation[] without mixing the two shapes.
 * Preserve explicit children in preorder, parent=SIZE_MAX for roots. Never
 * infer a hierarchy from a flat row's container name or overlapping ranges.
 * Hierarchical ranges use document_uri; flat rows retain their own URI.
 * Selection must fit the enclosing range. Unknown positive kinds are retained
 * for a host fallback, and unknown positive tags are ignored.
 *
 * Own at most 4096 symbols, 32 hierarchy levels, 4096 UTF-8 bytes per name,
 * detail or container, 8192 URI bytes, and 1 MiB JSON input. A nonempty name
 * must contain a non-whitespace Unicode scalar. Reject malformed or oversized
 * input as a complete failure; no partial outline is published. No file I/O.
 * document_uri must be valid even when the result is empty. Output is cleared
 * on failure. Cancellation is checked between entries and while parsing. */
    UmiStatus UmiLanguageSymbolCatalogueCreate(const void *json, size_t bytes, const char *document_uri,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageSymbolCatalogue **out_catalogue);
    UmiStatus UmiLanguageSymbolCatalogueReadResponse(const void *json, size_t bytes, uint64_t expected_id,
                                                     const char *document_uri,
                                                     const UmiCancellationToken *cancel,
                                                     UmiLanguageSymbolCatalogue **out_catalogue);
    void UmiLanguageSymbolCatalogueDestroy(UmiLanguageSymbolCatalogue *catalogue);
    size_t UmiLanguageSymbolCatalogueCount(const UmiLanguageSymbolCatalogue *catalogue);
    /* Metadata is copied; strings and location.uri remain borrowed until Destroy.
 * Invalid arguments and indices leave the caller's output unchanged. */
    UmiStatus UmiLanguageSymbolCatalogueAt(const UmiLanguageSymbolCatalogue *catalogue, size_t index,
                                           UmiLanguageSymbol *out_symbol);
#ifdef __cplusplus
}
#endif
#endif
