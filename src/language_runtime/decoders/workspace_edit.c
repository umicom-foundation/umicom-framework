/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/workspace_edit.c
 *
 * PURPOSE:
 *   Decode common WorkspaceEdit.changes maps for rename/refactor preview.
 *
 * MEMORY POLICY:
 *   UmiLanguageRuntimeTextEditList is deliberately large because it owns every
 *   decoded replacement string.  It must not be placed on the comparatively
 *   small default Windows executable stack.  Phase 5 therefore uses controlled
 *   heap storage for the temporary list and releases it on every exit path.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/workspace_edit.h"
#include "umicom/language_runtime/decoders/text_edits.h"
#include "umicom/language_runtime/workspace_edit_catalogue.h"
#include "umicom/language_runtime/json_tree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Provide the language runtime decode workspace edit operation used by this module and its
 * client applications.
 */
/* Complete workspace decoding replaces incremental publication. The legacy projection keeps its fixed contract and refuses versions or annotations it cannot represent; retain the previous reader for review.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_language_runtime_decode_workspace_edit(
    const char *json,
    UmiLanguageRuntimeWorkspaceEdit *out)
{
    UmiLanguageRuntimeJsonDocument document;
    UmiLanguageRuntimeTextEditList *edits = NULL;
    int result_token;
    int changes_token;
    size_t entry_index;
    size_t entry_count;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (json == NULL || out == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out, 0, sizeof(*out));
    status = umi_language_runtime_json_parse(json, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    result_token = umi_language_runtime_decoder_result_token(&document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result_token < 0 ||
        umi_language_runtime_json_is_null(&document, result_token)) {
        return UMI_STATUS_OK;
    }

    changes_token = umi_language_runtime_json_object_get(
        &document,
        result_token,
        "changes");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (changes_token < 0) return UMI_STATUS_NOT_IMPLEMENTED;

    entry_count = umi_language_runtime_json_object_count(
        &document,
        changes_token);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (entry_count == 0U) return UMI_STATUS_OK;

    edits = (UmiLanguageRuntimeTextEditList *)calloc(1U, sizeof(*edits));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (edits == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    /* Visit each bounded item once so every record receives the same rule. */
    for (entry_index = 0U; entry_index < entry_count; ++entry_index) {
        int key_token = -1;
        int value_token = -1;
        char uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];
        size_t edit_index;

        status = umi_language_runtime_json_object_entry_at(
            &document,
            changes_token,
            entry_index,
            &key_token,
            &value_token);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto cleanup;

        status = umi_language_runtime_json_string(
            &document,
            key_token,
            uri,
            sizeof(uri));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto cleanup;

        /* The same heap block is safely reused for each URI in the map. */
        (void)memset(edits, 0, sizeof(*edits));
        status = umi_language_runtime_decode_text_edit_array_token(
            &document,
            value_token,
            edits);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto cleanup;

        /* Visit each bounded item once so every record receives the same rule. */
        for (edit_index = 0U; edit_index < edits->count; ++edit_index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (out->count >= UMI_LANGUAGE_RUNTIME_MAX_EDITS) {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                goto cleanup;
            }

            (void)snprintf(
                out->items[out->count].uri,
                sizeof(out->items[out->count].uri),
                "%s",
                uri);
            out->items[out->count].edit = edits->edits[edit_index];
            out->count += 1U;
        }
    }

    status = UMI_STATUS_OK;

cleanup:
    free(edits);
    return status;
}
#endif
UmiStatus umi_language_runtime_decode_workspace_edit(const char *json,UmiLanguageRuntimeWorkspaceEdit *out)
{
    if(json==NULL || out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));
    UmiJsonTree *tree=NULL;UmiJsonTreeLimits limits={1024U*1024U,131072U,32U};
    UmiStatus status=UmiJsonTreeCreate(json,strlen(json),&limits,NULL,&tree);int result=-1,error=-1;
    if(status==UMI_STATUS_OK) status=UmiJsonTreeMember(tree,0,"result",&result);
    if(status==UMI_STATUS_NOT_FOUND) status=UMI_STATUS_PARSE_ERROR;
    if(status==UMI_STATUS_OK) {
        UmiStatus found=UmiJsonTreeMember(tree,0,"error",&error);
        if(found!=UMI_STATUS_NOT_FOUND) status=found==UMI_STATUS_OK?UMI_STATUS_PARSE_ERROR:found;
    }
    const char *span=NULL;size_t bytes=0U;
    if(status==UMI_STATUS_OK) status=UmiJsonTreeSourceSpan(tree,result,&span,&bytes);
    UmiLanguageWorkspaceEditCatalogue *catalogue=NULL;
    if(status==UMI_STATUS_OK) status=UmiLanguageWorkspaceEditCatalogueCreate(span,bytes,NULL,&catalogue);
    /* This legacy structure has no place for confirmation annotations or a
     * required document revision. Refuse those requests instead of dropping
     * their conditions. New review workflows retain the complete catalogue. */
    if(status==UMI_STATUS_OK && UmiLanguageWorkspaceEditCatalogueAnnotationCount(catalogue)!=0U) status=UMI_STATUS_NOT_IMPLEMENTED;
    UmiLanguageRuntimeWorkspaceEdit *candidate=status==UMI_STATUS_OK?calloc(1U,sizeof(*candidate)):NULL;
    if(status==UMI_STATUS_OK && candidate==NULL) status=UMI_STATUS_OUT_OF_MEMORY;
    for(size_t i=0U;status==UMI_STATUS_OK && i<UmiLanguageWorkspaceEditCatalogueCount(catalogue);++i) {
        UmiLanguageWorkspaceDocumentChange document;status=UmiLanguageWorkspaceEditCatalogueDocument(catalogue,i,&document);
        if(status==UMI_STATUS_OK && document.has_version) status=UMI_STATUS_NOT_IMPLEMENTED;
        if(status==UMI_STATUS_OK && document.edit_count>UMI_LANGUAGE_RUNTIME_MAX_EDITS-candidate->count) status=UMI_STATUS_CAPACITY_EXCEEDED;
        if(status!=UMI_STATUS_OK) break;
        for(size_t j=0U;status==UMI_STATUS_OK && j<document.edit_count;++j) {
            UmiLanguageWorkspaceTextChange edit;status=UmiLanguageWorkspaceEditCatalogueEdit(catalogue,i,j,&edit);
            if(status!=UMI_STATUS_OK) break;
            UmiLanguageRuntimeWorkspaceEditItem *item=&candidate->items[candidate->count];
            if(strlen(document.uri)>=sizeof(item->uri) || edit.text_bytes>=sizeof(item->edit.new_text)) {status=UMI_STATUS_CAPACITY_EXCEEDED;break;}
            memcpy(item->uri,document.uri,strlen(document.uri)+1U);
            memcpy(item->edit.new_text,edit.text,edit.text_bytes+1U);
            item->edit.range.start.line=(uint32_t)edit.range.start.line;
            item->edit.range.start.character=(uint32_t)edit.range.start.utf16_column;
            item->edit.range.end.line=(uint32_t)edit.range.end.line;
            item->edit.range.end.character=(uint32_t)edit.range.end.utf16_column;
            ++candidate->count;
        }
    }
    if(status==UMI_STATUS_OK) *out=*candidate;
    free(candidate);UmiLanguageWorkspaceEditCatalogueDestroy(catalogue);UmiJsonTreeDestroy(tree);return status;
}
