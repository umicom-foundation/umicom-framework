/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/workspace_edit_catalogue.h
 * PURPOSE: Own complete text-only workspace changes before source resolution, review and application.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_WORKSPACE_EDIT_CATALOGUE_H
#define UMICOM_LANGUAGE_RUNTIME_WORKSPACE_EDIT_CATALOGUE_H
#include "umicom/language_runtime/location_catalogue.h"
#include "umicom/language_runtime/text_edit_preview.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageWorkspaceDocumentChange
    {
        const char *uri;
        int has_version;
        int32_t version;
        size_t edit_count;
    } UmiLanguageWorkspaceDocumentChange;
    typedef struct UmiLanguageWorkspaceTextChange
    {
        UmiLanguageSourceRange range;
        const char *text;
        size_t text_bytes, annotation;
    } UmiLanguageWorkspaceTextChange;
    typedef struct UmiLanguageWorkspaceChangeAnnotation
    {
        const char *id, *label, *description;
        int needs_confirmation;
    } UmiLanguageWorkspaceChangeAnnotation;
    typedef struct UmiLanguageWorkspaceEditCatalogue UmiLanguageWorkspaceEditCatalogue;
    /* Decode WorkspaceEdit or null without accessing any documents or files.
 * Support changes maps and text-only documentChanges with integer/null document
 * versions. Prefer documentChanges if both forms are present. Keep annotations,
 * including unused ones, and require each referenced ID to exist. Annotation
 * indices are SIZE_MAX when absent. Resource create/rename/delete operations
 * return NOT_IMPLEMENTED for the whole request; no partial result is returned.
 *
 * Bounds: 1 MiB JSON, 256 distinct document URIs, 4096 total edits and 256
 * annotations. URIs allow 8192 UTF-8 bytes; replacement text allows 65536 bytes;
 * annotation IDs allow 4096 and labels/descriptions allow 65536 bytes each.
 * Repeated document entries are refused rather than interpreting sequential
 * edits against an unknown intermediate source. Ranges are ordered but still
 * require resolution and overlap checks against matching captured documents.
 * No confirmation or permission to apply follows from successful decoding.
 * Failure clears the output; strings are borrowed until catalogue destruction. */
    UmiStatus UmiLanguageWorkspaceEditCatalogueCreate(const void *json, size_t bytes,
                                                      const UmiCancellationToken *cancel,
                                                      UmiLanguageWorkspaceEditCatalogue **out_catalogue);
    UmiStatus UmiLanguageWorkspaceEditCatalogueReadResponse(
        const void *json, size_t bytes, uint64_t expected_request_id, const UmiCancellationToken *cancel,
        UmiLanguageWorkspaceEditCatalogue **out_catalogue);
    /* Build one document's complete proposed text without applying it. If the
 * group carries a version, source_version must be present and equal; otherwise
 * INVALID_STATE prevents preview against an unidentified revision. No version
 * is inferred from the document URI. The source must be the current captured
 * document, not an arbitrary file loaded later using the same name.
 *
 * Reuse the shared exact-range and overlap validation: at most 256 edits per
 * preview, 1 MiB serialized edit JSON and 16 MiB original/proposed text. Keep
 * the catalogue for displaying annotation labels and collecting any required
 * confirmations. Successful preview is never approval to apply or save, and
 * this function does not discard the catalogue's annotation requirements. */
    UmiStatus UmiLanguageWorkspaceEditCataloguePreview(const UmiLanguageWorkspaceEditCatalogue *catalogue,
                                                       size_t document_index, const char *source,
                                                       size_t source_bytes, size_t caret,
                                                       const int32_t *source_version,
                                                       const UmiCancellationToken *cancel,
                                                       UmiLanguageTextEditPreview **out_preview);
    /* Preview a whole proposal only when its one document exactly matches the
     * captured URI. No changes returns NOT_FOUND; another URI or multiple
     * document groups returns NOT_IMPLEMENTED, even if a group has no edits.
     * This prevents a single-document host from applying part of a rename.
     * All version, source, annotation and ownership rules of Preview apply. */
    UmiStatus UmiLanguageWorkspaceEditCataloguePreviewSingleDocument(
        const UmiLanguageWorkspaceEditCatalogue *catalogue, const char *document_uri, const char *source,
        size_t source_bytes, size_t caret, const int32_t *source_version, const UmiCancellationToken *cancel,
        UmiLanguageTextEditPreview **out_preview);
    void UmiLanguageWorkspaceEditCatalogueDestroy(UmiLanguageWorkspaceEditCatalogue *catalogue);
    size_t UmiLanguageWorkspaceEditCatalogueCount(const UmiLanguageWorkspaceEditCatalogue *catalogue);
    size_t
    UmiLanguageWorkspaceEditCatalogueAnnotationCount(const UmiLanguageWorkspaceEditCatalogue *catalogue);
    UmiStatus UmiLanguageWorkspaceEditCatalogueDocument(const UmiLanguageWorkspaceEditCatalogue *catalogue,
                                                        size_t index,
                                                        UmiLanguageWorkspaceDocumentChange *out_document);
    UmiStatus UmiLanguageWorkspaceEditCatalogueEdit(const UmiLanguageWorkspaceEditCatalogue *catalogue,
                                                    size_t document_index, size_t edit_index,
                                                    UmiLanguageWorkspaceTextChange *out_edit);
    UmiStatus
    UmiLanguageWorkspaceEditCatalogueAnnotation(const UmiLanguageWorkspaceEditCatalogue *catalogue,
                                                size_t index,
                                                UmiLanguageWorkspaceChangeAnnotation *out_annotation);
#ifdef __cplusplus
}
#endif
#endif
