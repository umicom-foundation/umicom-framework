/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/signature_catalogue.h
 * PURPOSE: Own function overloads, parameter descriptions and exact label spans for source assistance.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_SIGNATURE_CATALOGUE_H
#define UMICOM_LANGUAGE_RUNTIME_SIGNATURE_CATALOGUE_H
#include "umicom/language_runtime/json_tree.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum UmiLanguageSignatureDocumentationKind
    {
        UMI_LANGUAGE_SIGNATURE_PLAIN_TEXT,
        UMI_LANGUAGE_SIGNATURE_MARKDOWN
    } UmiLanguageSignatureDocumentationKind;
    typedef struct UmiLanguageSignatureDocumentation
    {
        const char *text;
        UmiLanguageSignatureDocumentationKind kind;
    } UmiLanguageSignatureDocumentation;
    typedef struct UmiLanguageSignature
    {
        const char *label;
        UmiLanguageSignatureDocumentation documentation;
        size_t parameter_count, active_parameter;
    } UmiLanguageSignature;
    typedef struct UmiLanguageSignatureParameter
    {
        const char *label;
        UmiLanguageSignatureDocumentation documentation;
        size_t start_byte, end_byte;
        int has_label_span;
    } UmiLanguageSignatureParameter;
    typedef struct UmiLanguageSignatureCatalogue UmiLanguageSignatureCatalogue;
    /* Read a complete SignatureHelp object or null. Limits are 1 MiB JSON, 256
 * overloads, 256 parameters per overload and 4096 parameters overall. Labels
 * allow 65536 UTF-8 bytes and each documentation value allows 262144 bytes.
 * All decoded labels and descriptions together allow 4 MiB, including copies
 * produced by overlapping parameter-label slices.
 * Offset labels use UTF-16 code units across the whole signature string,
 * including line endings. Split surrogate pairs and reversed spans are errors.
 * String labels are retained even when missing or ambiguous in the signature;
 * those parameters have no highlight span. No source or markup is executed.
 * Failure clears the output; success owns all strings until Destroy. */
    UmiStatus UmiLanguageSignatureCatalogueCreate(const void *json, size_t bytes,
                                                  const UmiCancellationToken *cancel,
                                                  UmiLanguageSignatureCatalogue **out_catalogue);
    UmiStatus UmiLanguageSignatureCatalogueReadResponse(const void *json, size_t bytes, uint64_t request_id,
                                                        const UmiCancellationToken *cancel,
                                                        UmiLanguageSignatureCatalogue **out_catalogue);
    void UmiLanguageSignatureCatalogueDestroy(UmiLanguageSignatureCatalogue *catalogue);
    size_t UmiLanguageSignatureCatalogueCount(const UmiLanguageSignatureCatalogue *catalogue);
    /* SIZE_MAX means there is no active overload/parameter. An omitted or
 * out-of-range index defaults to zero when choices exist. A signature's own
 * activeParameter takes precedence over the top-level value. These borrowed
 * views stay valid until Destroy; errors leave caller outputs unchanged. */
    size_t UmiLanguageSignatureCatalogueActive(const UmiLanguageSignatureCatalogue *catalogue);
    UmiStatus UmiLanguageSignatureCatalogueAt(const UmiLanguageSignatureCatalogue *catalogue, size_t index,
                                              UmiLanguageSignature *out_signature);
    UmiStatus UmiLanguageSignatureCatalogueParameter(const UmiLanguageSignatureCatalogue *catalogue,
                                                     size_t signature_index, size_t parameter_index,
                                                     UmiLanguageSignatureParameter *out_parameter);
#ifdef __cplusplus
}
#endif
#endif
