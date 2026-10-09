/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/signature_catalogue.c
 * PURPOSE: Retain complete overload information and resolve parameter labels without truncating server text.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/signature_catalogue.h"
#include "umicom/language_runtime/response_tree.h"
#include <stdlib.h>
#include <string.h>

typedef struct SignatureParameter
{
    char *label, *documentation;
    UmiLanguageSignatureParameter value;
} SignatureParameter;
typedef struct SignatureEntry
{
    char *label, *documentation;
    SignatureParameter *parameters;
    UmiLanguageSignature value;
} SignatureEntry;
struct UmiLanguageSignatureCatalogue
{
    SignatureEntry *entries;
    size_t count, active, total_parameters, owned_text_bytes;
};
static UmiStatus SignatureMember(const UmiJsonTree *tree, int object, const char *name, int required,
                                 int *out)
{
    *out = -1;
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = UmiJsonTreeMember(tree, object, name, out);
    return status == UMI_STATUS_NOT_FOUND ? (required ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK) : status;
}
static UmiStatus SignatureText(const UmiJsonTree *tree, int node, size_t limit, char **out)
{
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    const char *span;
    size_t bytes;
    UmiStatus status = UmiJsonTreeSourceSpan(tree, node, &span, &bytes);
    (void)span;
    if (status != UMI_STATUS_OK)
        return status;
    char *text = malloc(bytes + 1U);
    if (text == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiJsonTreeText(tree, node, text, bytes + 1U);
    if (status == UMI_STATUS_OK && strlen(text) > limit)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        *out = text;
    else
        free(text);
    return status;
}
/* Keep the markup kind as data. A UI can show these values literally without
 * evaluating HTML, opening links or allowing documentation to run commands. */
static UmiStatus SignatureDocumentation(const UmiJsonTree *tree, int object, char **owned,
                                        UmiLanguageSignatureDocumentation *out)
{
    int node;
    UmiStatus status = SignatureMember(tree, object, "documentation", 0, &node);
    out->text = "";
    out->kind = UMI_LANGUAGE_SIGNATURE_PLAIN_TEXT;
    if (status != UMI_STATUS_OK || node < 0)
        return status;
    if (UmiJsonTreeKind(tree, node) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int kind, value;
        char spelling[32];
        status = SignatureMember(tree, node, "kind", 1, &kind);
        if (status == UMI_STATUS_OK)
            status = SignatureMember(tree, node, "value", 1, &value);
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeText(tree, kind, spelling, sizeof(spelling));
        if (status == UMI_STATUS_OK)
        {
            if (strcmp(spelling, "markdown") == 0)
                out->kind = UMI_LANGUAGE_SIGNATURE_MARKDOWN;
            else if (strcmp(spelling, "plaintext") != 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        if (status != UMI_STATUS_OK)
            return status;
        node = value;
    }
    status = SignatureText(tree, node, 262144U, owned);
    if (status == UMI_STATUS_OK)
        out->text = *owned;
    return status;
}
static UmiStatus SignatureIndex(const UmiJsonTree *tree, int object, const char *name, size_t fallback,
                                size_t *out)
{
    int node;
    UmiStatus status = SignatureMember(tree, object, name, 0, &node);
    int64_t value;
    if (status != UMI_STATUS_OK)
        return status;
    if (node < 0)
    {
        *out = fallback;
        return UMI_STATUS_OK;
    }
    status = UmiJsonTreeInteger(tree, node, &value);
    if (status == UMI_STATUS_OK && (value < 0 || value > INT32_MAX))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        *out = (size_t)value;
    return status;
}
/* JSON decoding has already validated UTF-8. Unlike source coordinates, label
 * offsets count every code unit, so CR and LF each contribute one unit. */
static UmiStatus SignatureOffset(const char *label, size_t units, size_t *out)
{
    size_t byte = 0U, position = 0U;
    while (label[byte] != '\0' && position < units)
    {
        unsigned char first = (unsigned char)label[byte];
        size_t width = first < 0x80U ? 1U : first < 0xe0U ? 2U : first < 0xf0U ? 3U : 4U;
        size_t increment = width == 4U ? 2U : 1U;
        if (units - position < increment)
            return UMI_STATUS_PARSE_ERROR;
        byte += width;
        position += increment;
    }
    if (position != units)
        return UMI_STATUS_PARSE_ERROR;
    *out = byte;
    return UMI_STATUS_OK;
}
static UmiStatus SignatureParameterRead(const UmiJsonTree *tree, int node, const char *signature,
                                        SignatureParameter *out)
{
    int label;
    UmiStatus status = SignatureMember(tree, node, "label", 1, &label);
    if (status != UMI_STATUS_OK)
        return status;
    if (UmiJsonTreeKind(tree, label) == UMI_LANGUAGE_RUNTIME_JSON_STRING)
    {
        status = SignatureText(tree, label, 65536U, &out->label);
        if (status == UMI_STATUS_OK && out->label[0] != '\0')
        {
            const char *match = strstr(signature, out->label);
            /* Repeated names provide no unambiguous highlight. Preserve the
             * parameter text instead of guessing which occurrence was meant. */
            if (match != NULL && strstr(match + 1, out->label) == NULL)
            {
                out->value.start_byte = (size_t)(match - signature);
                out->value.end_byte = out->value.start_byte + strlen(out->label);
                out->value.has_label_span = 1;
            }
        }
    }
    else if (UmiJsonTreeKind(tree, label) == UMI_LANGUAGE_RUNTIME_JSON_ARRAY &&
             UmiJsonTreeCount(tree, label) == 2U)
    {
        int first = UmiJsonTreeFirst(tree, label);
        int64_t start, end;
        status = UmiJsonTreeInteger(tree, first, &start);
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeInteger(tree, UmiJsonTreeNext(tree, first), &end);
        /* A label range outside integer storage cannot identify a parameter.
         * Report malformed signature input rather than an allocation limit. */
        if (status == UMI_STATUS_CAPACITY_EXCEEDED)
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK && (start < 0 || end < start || end > INT32_MAX))
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
            status = SignatureOffset(signature, (size_t)start, &out->value.start_byte);
        if (status == UMI_STATUS_OK)
            status = SignatureOffset(signature, (size_t)end, &out->value.end_byte);
        if (status == UMI_STATUS_OK)
        {
            size_t bytes = out->value.end_byte - out->value.start_byte;
            out->label = malloc(bytes + 1U);
            if (out->label == NULL)
                status = UMI_STATUS_OUT_OF_MEMORY;
            else
            {
                memcpy(out->label, signature + out->value.start_byte, bytes);
                out->label[bytes] = '\0';
                out->value.has_label_span = 1;
            }
        }
    }
    else
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = SignatureDocumentation(tree, node, &out->documentation, &out->value.documentation);
    if (status == UMI_STATUS_OK)
        out->value.label = out->label;
    return status;
}
/* Offset labels can repeat a large part of the same signature. Bound their
 * owned copies as well as the wire input, preventing compact replies from
 * expanding into hundreds of megabytes of labels in an inspection window. */
static UmiStatus SignatureTextBudget(UmiLanguageSignatureCatalogue *catalogue, const char *label,
                                     const char *documentation)
{
    size_t bytes = strlen(label) + strlen(documentation);
    const size_t limit = 4U * 1024U * 1024U;
    if (bytes > limit - catalogue->owned_text_bytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->owned_text_bytes += bytes;
    return UMI_STATUS_OK;
}
/* An aggregate text budget now bounds repeated label slices as well as JSON input; the earlier unbudgeted reader is retained for review.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SignatureRead(const UmiJsonTree *tree,int node,size_t default_parameter,
    const UmiCancellationToken *cancel,UmiLanguageSignatureCatalogue *catalogue,SignatureEntry *out)
{
    int label,parameters;UmiStatus status=SignatureMember(tree,node,"label",1,&label);
    if(status==UMI_STATUS_OK) status=SignatureText(tree,label,65536U,&out->label);
    if(status==UMI_STATUS_OK) status=SignatureDocumentation(tree,node,&out->documentation,&out->value.documentation);
    if(status==UMI_STATUS_OK) status=SignatureMember(tree,node,"parameters",0,&parameters);
    if(status!=UMI_STATUS_OK) return status;
    if(parameters>=0) {
        if(UmiJsonTreeKind(tree,parameters)!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY) return UMI_STATUS_PARSE_ERROR;
        out->value.parameter_count=UmiJsonTreeCount(tree,parameters);
    }
    if(out->value.parameter_count>256U || out->value.parameter_count>4096U-catalogue->total_parameters)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->total_parameters+=out->value.parameter_count;
    size_t active;
    status=SignatureIndex(tree,node,"activeParameter",default_parameter,&active);
    if(status!=UMI_STATUS_OK) return status;
    out->value.active_parameter=out->value.parameter_count==0U?SIZE_MAX:active<out->value.parameter_count?active:0U;
    if(out->value.parameter_count!=0U) {
        out->parameters=calloc(out->value.parameter_count,sizeof(*out->parameters));
        if(out->parameters==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    int parameter=parameters>=0?UmiJsonTreeFirst(tree,parameters):-1;
    for(size_t i=0U;status==UMI_STATUS_OK && i<out->value.parameter_count;++i,parameter=UmiJsonTreeNext(tree,parameter)) {
        if(umi_cancellation_token_is_requested(cancel)) status=UMI_STATUS_CANCELLED;
        else status=SignatureParameterRead(tree,parameter,out->label,&out->parameters[i]);
    }
    if(status==UMI_STATUS_OK) out->value.label=out->label;
    return status;
}
#endif
static UmiStatus SignatureRead(const UmiJsonTree *tree, int node, size_t default_parameter,
                               const UmiCancellationToken *cancel, UmiLanguageSignatureCatalogue *catalogue,
                               SignatureEntry *out)
{
    int label, parameters;
    UmiStatus status = SignatureMember(tree, node, "label", 1, &label);
    if (status == UMI_STATUS_OK)
        status = SignatureText(tree, label, 65536U, &out->label);
    if (status == UMI_STATUS_OK)
        status = SignatureDocumentation(tree, node, &out->documentation, &out->value.documentation);
    if (status == UMI_STATUS_OK)
        status = SignatureMember(tree, node, "parameters", 0, &parameters);
    if (status == UMI_STATUS_OK)
        status = SignatureTextBudget(catalogue, out->label, out->value.documentation.text);
    if (status != UMI_STATUS_OK)
        return status;
    if (parameters >= 0)
    {
        if (UmiJsonTreeKind(tree, parameters) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            return UMI_STATUS_PARSE_ERROR;
        out->value.parameter_count = UmiJsonTreeCount(tree, parameters);
    }
    if (out->value.parameter_count > 256U || out->value.parameter_count > 4096U - catalogue->total_parameters)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->total_parameters += out->value.parameter_count;
    size_t active;
    status = SignatureIndex(tree, node, "activeParameter", default_parameter, &active);
    if (status != UMI_STATUS_OK)
        return status;
    out->value.active_parameter = out->value.parameter_count == 0U      ? SIZE_MAX
                                  : active < out->value.parameter_count ? active
                                                                        : 0U;
    if (out->value.parameter_count != 0U)
    {
        out->parameters = calloc(out->value.parameter_count, sizeof(*out->parameters));
        if (out->parameters == NULL)
            return UMI_STATUS_OUT_OF_MEMORY;
    }
    int parameter = parameters >= 0 ? UmiJsonTreeFirst(tree, parameters) : -1;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < out->value.parameter_count;
         ++i, parameter = UmiJsonTreeNext(tree, parameter))
    {
        if (umi_cancellation_token_is_requested(cancel))
            status = UMI_STATUS_CANCELLED;
        else
            status = SignatureParameterRead(tree, parameter, out->label, &out->parameters[i]);
        if (status == UMI_STATUS_OK)
            status = SignatureTextBudget(catalogue, out->parameters[i].label,
                                         out->parameters[i].value.documentation.text);
    }
    if (status == UMI_STATUS_OK)
        out->value.label = out->label;
    return status;
}
UmiStatus UmiLanguageSignatureCatalogueCreate(const void *json, size_t bytes,
                                              const UmiCancellationToken *cancel,
                                              UmiLanguageSignatureCatalogue **out_catalogue)
{
    if (out_catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 16U};
    UmiStatus status = UmiJsonTreeCreate(json, bytes, &limits, cancel, &tree);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageSignatureCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL)
    {
        UmiJsonTreeDestroy(tree);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    catalogue->active = SIZE_MAX;
    int signatures = -1;
    size_t active = 0U, parameter = 0U;
    if (!UmiJsonTreeIsNull(tree, 0))
    {
        status = SignatureMember(tree, 0, "signatures", 1, &signatures);
        if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, signatures) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
            status = SignatureIndex(tree, 0, "activeSignature", 0U, &active);
        if (status == UMI_STATUS_OK)
            status = SignatureIndex(tree, 0, "activeParameter", 0U, &parameter);
        if (status == UMI_STATUS_OK)
            catalogue->count = UmiJsonTreeCount(tree, signatures);
        if (catalogue->count > 256U)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (status == UMI_STATUS_OK && catalogue->count != 0U)
    {
        catalogue->entries = calloc(catalogue->count, sizeof(*catalogue->entries));
        if (catalogue->entries == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
            catalogue->active = active < catalogue->count ? active : 0U;
    }
    int node = signatures >= 0 ? UmiJsonTreeFirst(tree, signatures) : -1;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < catalogue->count;
         ++i, node = UmiJsonTreeNext(tree, node))
    {
        if (umi_cancellation_token_is_requested(cancel))
            status = UMI_STATUS_CANCELLED;
        else
            status = SignatureRead(tree, node, parameter, cancel, catalogue, &catalogue->entries[i]);
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK)
        *out_catalogue = catalogue;
    else
        UmiLanguageSignatureCatalogueDestroy(catalogue);
    return status;
}
UmiStatus UmiLanguageSignatureCatalogueReadResponse(const void *json, size_t bytes, uint64_t request_id,
                                                    const UmiCancellationToken *cancel,
                                                    UmiLanguageSignatureCatalogue **out_catalogue)
{
    if (out_catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    UmiJsonTree *tree = NULL;
    int result = -1;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 16U};
    UmiStatus status = UmiLanguageResponseTreeRead(json, bytes, request_id, &limits, cancel, &tree, &result);
    const char *span = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(tree, result, &span, &length);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageSignatureCatalogueCreate(span, length, cancel, out_catalogue);
    UmiJsonTreeDestroy(tree);
    return status;
}
void UmiLanguageSignatureCatalogueDestroy(UmiLanguageSignatureCatalogue *catalogue)
{
    if (catalogue == NULL)
        return;
    if (catalogue->entries != NULL)
        for (size_t i = 0U; i < catalogue->count; ++i)
        {
            SignatureEntry *entry = &catalogue->entries[i];
            if (entry->parameters != NULL)
                for (size_t j = 0U; j < entry->value.parameter_count; ++j)
                {
                    free(entry->parameters[j].label);
                    free(entry->parameters[j].documentation);
                }
            free(entry->parameters);
            free(entry->label);
            free(entry->documentation);
        }
    free(catalogue->entries);
    free(catalogue);
}
size_t UmiLanguageSignatureCatalogueCount(const UmiLanguageSignatureCatalogue *catalogue)
{
    return catalogue == NULL ? 0U : catalogue->count;
}
size_t UmiLanguageSignatureCatalogueActive(const UmiLanguageSignatureCatalogue *catalogue)
{
    return catalogue == NULL ? SIZE_MAX : catalogue->active;
}
UmiStatus UmiLanguageSignatureCatalogueAt(const UmiLanguageSignatureCatalogue *catalogue, size_t index,
                                          UmiLanguageSignature *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->entries[index].value;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageSignatureCatalogueParameter(const UmiLanguageSignatureCatalogue *catalogue,
                                                 size_t signature_index, size_t parameter_index,
                                                 UmiLanguageSignatureParameter *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (signature_index >= catalogue->count ||
        parameter_index >= catalogue->entries[signature_index].value.parameter_count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->entries[signature_index].parameters[parameter_index].value;
    return UMI_STATUS_OK;
}
