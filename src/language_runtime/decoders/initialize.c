/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/initialize.c
 *
 * PURPOSE:
 *   Decode common server capability values from initialize result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/initialize.h"
#include <string.h>
/* Preserve permissive capability coercion for engineering review. The typed
 * replacement below validates advertised forms and publishes all flags at once. */
#if 0
/* Provide the on operation used by this module and its client applications. */
static int on(const UmiLanguageRuntimeJsonDocument*d,int c,const char*k){int t=umi_language_runtime_json_object_get(d,c,k),b=0;int64_t n=0;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(t<0||umi_language_runtime_json_is_null(d,t))return 0;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(d->tokens[t].type==UMI_LANGUAGE_RUNTIME_JSON_OBJECT||d->tokens[t].type==UMI_LANGUAGE_RUNTIME_JSON_ARRAY)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_runtime_json_bool(d,t,&b)==UMI_STATUS_OK)return b;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_runtime_json_int64(d,t,&n)==UMI_STATUS_OK)return n!=0;return 0;}
/*
 * Initialise language runtime decode from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_runtime_decode_initialize(const char*j,UmiLanguageRuntimeInitializeResult*out){UmiLanguageRuntimeJsonDocument d;int r,c;UmiStatus s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);c=r>=0?umi_language_runtime_json_object_get(&d,r,"capabilities"):-1;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c<0)return UMI_STATUS_PARSE_ERROR;out->text_document_sync=on(&d,c,"textDocumentSync");out->completion=on(&d,c,"completionProvider");out->hover=on(&d,c,"hoverProvider");out->signature_help=on(&d,c,"signatureHelpProvider");out->definition=on(&d,c,"definitionProvider");out->references=on(&d,c,"referencesProvider");out->document_symbols=on(&d,c,"documentSymbolProvider");out->workspace_symbols=on(&d,c,"workspaceSymbolProvider");out->code_actions=on(&d,c,"codeActionProvider");out->formatting=on(&d,c,"documentFormattingProvider");out->rename=on(&d,c,"renameProvider");out->semantic_tokens=on(&d,c,"semanticTokensProvider");out->inlay_hints=on(&d,c,"inlayHintProvider");out->folding_ranges=on(&d,c,"foldingRangeProvider");return UMI_STATUS_OK;}
#endif

#include "umicom/language_runtime/json_tree.h"

typedef enum CapabilityForm {
    CAPABILITY_BOOLEAN_OR_OPTIONS,
    CAPABILITY_OPTIONS,
    CAPABILITY_SYNC
} CapabilityForm;

/* Absence, null and an explicit false retain the earlier disabled meaning.
 * Advertising support, however, requires a recognised shape. An array or an
 * arbitrary number must not enable a command merely because it is nonempty. */
static UmiStatus CapabilityFlag(const UmiJsonTree *tree, int capabilities,
    const char *name, CapabilityForm form, int *out)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, capabilities, name, &node);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeIsNull(tree, node)) return UMI_STATUS_OK;
    if (UmiJsonTreeKind(tree, node) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT) {
        *out = 1;
        return UMI_STATUS_OK;
    }
    int enabled = 0;
    if (UmiJsonTreeBoolean(tree, node, &enabled) == UMI_STATUS_OK) {
        if (enabled && form != CAPABILITY_BOOLEAN_OR_OPTIONS) return UMI_STATUS_PARSE_ERROR;
        *out = enabled;
        return UMI_STATUS_OK;
    }
    if (form == CAPABILITY_SYNC) {
        int64_t mode = 0;
        if (UmiJsonTreeInteger(tree, node, &mode) != UMI_STATUS_OK || mode < 0 || mode > 2)
            return UMI_STATUS_PARSE_ERROR;
        *out = mode != 0;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_PARSE_ERROR;
}

UmiStatus umi_language_runtime_decode_initialize(const char *json,
    UmiLanguageRuntimeInitializeResult *out)
{
    if (json == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes = 0U;
    while (bytes < UMI_LANGUAGE_RUNTIME_JSON_CAPACITY && json[bytes] != '\0') ++bytes;
    if (bytes == UMI_LANGUAGE_RUNTIME_JSON_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiJsonTree *tree = NULL;
    const UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    UmiStatus status = UmiJsonTreeCreate(json, bytes, &limits, NULL, &tree);
    int result = -1, capabilities = -1;
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiJsonTreeMember(tree, 0, "result", &result);
    if (status == UMI_STATUS_NOT_FOUND || (status == UMI_STATUS_OK &&
        UmiJsonTreeKind(tree, result) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiJsonTreeMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_NOT_FOUND || (status == UMI_STATUS_OK &&
        UmiJsonTreeKind(tree, capabilities) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)) status = UMI_STATUS_PARSE_ERROR;
    /* Decode into a candidate. A late invalid field or duplicate must preserve
     * the caller's last accepted capabilities, including dispatcher snapshots. */
    UmiLanguageRuntimeInitializeResult candidate = {0};
    struct CapabilityField { const char *name; CapabilityForm form; int *flag; } fields[] = {
        {"textDocumentSync", CAPABILITY_SYNC, &candidate.text_document_sync},
        {"completionProvider", CAPABILITY_OPTIONS, &candidate.completion},
        {"hoverProvider", CAPABILITY_BOOLEAN_OR_OPTIONS, &candidate.hover},
        {"signatureHelpProvider", CAPABILITY_OPTIONS, &candidate.signature_help},
        {"definitionProvider", CAPABILITY_BOOLEAN_OR_OPTIONS, &candidate.definition},
        {"referencesProvider", CAPABILITY_BOOLEAN_OR_OPTIONS, &candidate.references},
        {"documentSymbolProvider", CAPABILITY_BOOLEAN_OR_OPTIONS, &candidate.document_symbols},
        {"workspaceSymbolProvider", CAPABILITY_BOOLEAN_OR_OPTIONS, &candidate.workspace_symbols},
        {"codeActionProvider", CAPABILITY_BOOLEAN_OR_OPTIONS, &candidate.code_actions},
        {"documentFormattingProvider", CAPABILITY_BOOLEAN_OR_OPTIONS, &candidate.formatting},
        {"renameProvider", CAPABILITY_BOOLEAN_OR_OPTIONS, &candidate.rename},
        {"semanticTokensProvider", CAPABILITY_OPTIONS, &candidate.semantic_tokens},
        {"inlayHintProvider", CAPABILITY_BOOLEAN_OR_OPTIONS, &candidate.inlay_hints},
        {"foldingRangeProvider", CAPABILITY_BOOLEAN_OR_OPTIONS, &candidate.folding_ranges}
    };
    for (size_t i = 0U; status == UMI_STATUS_OK && i < sizeof(fields) / sizeof(fields[0]); ++i)
        status = CapabilityFlag(tree, capabilities, fields[i].name, fields[i].form, fields[i].flag);
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK) *out = candidate;
    return status;
}
