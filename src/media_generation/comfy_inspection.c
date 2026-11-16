/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_inspection.c
 *
 * PURPOSE:
 *   Compose bounded local observations into an owned report without activating provider settings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_inspection.h"
#include "umicom/provider_connections/document.h"
#include "umicom/provider_connections/document_file.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/base/utf8.h"
#include "comfy_response_internal.h"
#include <string.h>

/* Retire workflow text promptly, including the reader's terminating byte.
 * Caller files are untouched; copied report metadata has its own lifetime. */
static void InspectionRelease(unsigned char **bytes, size_t *length)
{
    if (*bytes != NULL) {
        umi_secret_clear(*bytes, *length + 1U);
        UmiInputFileFree(*bytes);
    }
    *bytes = NULL;
    *length = 0U;
}
static UmiStatus InspectionReadFile(const char *path, size_t maximum,
    const UmiCancellationToken *cancel, unsigned char **bytes, size_t *length)
{
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    return UmiInputFileRead(path, maximum, bytes, length);
}
/* Centralize file admission and temporary ownership for two independent observation reports. The legacy wrapper selects only presence checks, preserving its earlier acceptance limits. The former implementation is retained for owner review. */
#if 0
UmiStatus UmiComfyInspectionRead(const UmiComfyInspectionPaths *paths,
    const UmiCancellationToken *cancel, UmiComfyPreflightReport **out)
{
    if (paths == NULL || out == NULL || *out != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const char *end = memchr(paths->client_id, '\0', sizeof(paths->client_id));
/* The shared UTF-8 validator also accepts an optional error-offset output. Supply NULL because this boundary returns a status only; retain the earlier incomplete call for review. The former implementation is retained for owner review. */
#if 0
    if (end == NULL || end == paths->client_id ||
        !UmiUtf8Validate((const unsigned char *)paths->client_id,
            (size_t)(end - paths->client_id))) return UMI_STATUS_INVALID_ARGUMENT;
#endif
    if (end == NULL || end == paths->client_id ||
        !UmiUtf8Validate((const unsigned char *)paths->client_id,
            (size_t)(end - paths->client_id), NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Reject every malformed path before opening even the first file. Each
     * reader then independently verifies the actual opened file object. */
    const char *files[] = {paths->connection_document, paths->workflow_json, paths->node_catalogue_json};
    for (size_t i = 0U; i < sizeof(files) / sizeof(files[0]); ++i) {
        UmiStatus check = UmiOutputFileValidatePath(files[i]);
        if (check != UMI_STATUS_OK) return check;
    }
    unsigned char *bytes = NULL;
    size_t length = 0U;
    uint64_t revision = 0U;
    UmiProviderConnection connection = {0};
    UmiComfyPromptDraft draft = {0};
    UmiComfyPrompt *prompt = NULL;
    UmiComfyNodeCatalogue *catalogue = NULL;
    UmiComfyPreflightReport *report = NULL;
    UmiStatus status = InspectionReadFile(files[0], UMI_PROVIDER_CONNECTION_DOCUMENT_MAX_BYTES - 1U,
        cancel, &bytes, &length);
    if (status != UMI_STATUS_OK) goto done;
    if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto done; }
    status = UmiProviderConnectionDocumentImport(bytes, length, &connection, &revision);
    InspectionRelease(&bytes, &length);
    if (status != UMI_STATUS_OK) goto done;
    status = UmiComfyConnectionCheck(&connection);
    if (status != UMI_STATUS_OK) goto done;
    status = InspectionReadFile(files[1], UMI_COMFY_PROMPT_MAX_BYTES, cancel, &bytes, &length);
    if (status != UMI_STATUS_OK) goto done;
    draft.workflow_json = bytes;
    draft.workflow_length = length;
    memcpy(draft.client_id, paths->client_id, (size_t)(end - paths->client_id) + 1U);
    status = UmiComfyPromptCreate(&connection, revision, &draft, cancel, &prompt);
    InspectionRelease(&bytes, &length);
    draft.workflow_json = NULL;
    draft.workflow_length = 0U;
    if (status != UMI_STATUS_OK) goto done;
    status = InspectionReadFile(files[2], UMI_COMFY_NODES_MAX_BYTES, cancel, &bytes, &length);
    if (status != UMI_STATUS_OK) goto done;
    status = UmiComfyNodeCatalogueCreate(&connection, revision, bytes, length, cancel, &catalogue);
    InspectionRelease(&bytes, &length);
    if (status != UMI_STATUS_OK) goto done;
    /* All identities here derive from files, not a live store or server.
     * Matching them protects this composition but grants no execution authority. */
    status = UmiComfyPreflightCreate(&connection, revision, prompt, catalogue, cancel, &report);
    if (status == UMI_STATUS_OK) {
        *out = report;
        report = NULL;
    }
done:
    InspectionRelease(&bytes, &length);
    UmiComfyPreflightDestroy(report);
    UmiComfyNodeCatalogueDestroy(catalogue);
    UmiComfyPromptDestroy(prompt);
    umi_secret_clear(&connection, sizeof(connection));
    umi_secret_clear(&draft, sizeof(draft));
    return status;
}

#endif
static UmiStatus InspectionCompose(const UmiComfyInspectionPaths *paths,
    const UmiCancellationToken *cancel, UmiComfyPreflightReport **out_report,
    UmiComfyReferenceInventory **out_references)
{
    if (paths == NULL || (out_report == NULL) == (out_references == NULL) ||
        (out_report != NULL && *out_report != NULL) ||
        (out_references != NULL && *out_references != NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    const char *end = memchr(paths->client_id, '\0', sizeof(paths->client_id));
/* The shared UTF-8 validator also accepts an optional error-offset output. Supply NULL because this boundary returns a status only; retain the earlier incomplete call for review. The former implementation is retained for owner review. */
#if 0
    if (end == NULL || end == paths->client_id ||
        !UmiUtf8Validate((const unsigned char *)paths->client_id,
            (size_t)(end - paths->client_id))) return UMI_STATUS_INVALID_ARGUMENT;
#endif
    if (end == NULL || end == paths->client_id ||
        !UmiUtf8Validate((const unsigned char *)paths->client_id,
            (size_t)(end - paths->client_id), NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Reject every malformed path before opening even the first file. Each
     * reader then independently verifies the actual opened file object. */
    const char *files[] = {paths->connection_document, paths->workflow_json, paths->node_catalogue_json};
    for (size_t i = 0U; i < sizeof(files) / sizeof(files[0]); ++i) {
        UmiStatus check = UmiOutputFileValidatePath(files[i]);
        if (check != UMI_STATUS_OK) return check;
    }
    unsigned char *bytes = NULL;
    size_t length = 0U;
    uint64_t revision = 0U;
    UmiProviderConnection connection = {0};
    UmiComfyPromptDraft draft = {0};
    UmiComfyPrompt *prompt = NULL;
    UmiComfyNodeCatalogue *catalogue = NULL;
    UmiComfyPreflightReport *report = NULL;
    UmiComfyReferenceInventory *references = NULL;
/* Saved settings file admission now belongs to the shared provider-document service. It retains regular-file checks, canonical decoding and historical revision semantics while centralizing cancellation and temporary-data ownership. The former implementation is retained for owner review. */
#if 0
    UmiStatus status = InspectionReadFile(files[0], UMI_PROVIDER_CONNECTION_DOCUMENT_MAX_BYTES - 1U,
        cancel, &bytes, &length);
    if (status != UMI_STATUS_OK) goto done;
    if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto done; }
    status = UmiProviderConnectionDocumentImport(bytes, length, &connection, &revision);
    InspectionRelease(&bytes, &length);
#endif
    UmiStatus status = UmiProviderConnectionDocumentFileRead(files[0], cancel, &connection, &revision);
    if (status != UMI_STATUS_OK) goto done;
    status = UmiComfyConnectionCheck(&connection);
    if (status != UMI_STATUS_OK) goto done;
    status = InspectionReadFile(files[1], UMI_COMFY_PROMPT_MAX_BYTES, cancel, &bytes, &length);
    if (status != UMI_STATUS_OK) goto done;
    draft.workflow_json = bytes;
    draft.workflow_length = length;
    memcpy(draft.client_id, paths->client_id, (size_t)(end - paths->client_id) + 1U);
    status = UmiComfyPromptCreate(&connection, revision, &draft, cancel, &prompt);
    InspectionRelease(&bytes, &length);
    draft.workflow_json = NULL;
    draft.workflow_length = 0U;
    if (status != UMI_STATUS_OK) goto done;
    status = InspectionReadFile(files[2], UMI_COMFY_NODES_MAX_BYTES, cancel, &bytes, &length);
    if (status != UMI_STATUS_OK) goto done;
    status = UmiComfyNodeCatalogueCreate(&connection, revision, bytes, length, cancel, &catalogue);
    InspectionRelease(&bytes, &length);
    if (status != UMI_STATUS_OK) goto done;
    /* All identities here derive from files, not a live store or server.
     * Matching them protects this composition but grants no execution authority. */
    if (out_report != NULL) {
        status = UmiComfyPreflightCreate(&connection, revision, prompt, catalogue, cancel, &report);
        if (status == UMI_STATUS_OK) { *out_report = report; report = NULL; }
    } else {
        status = UmiComfyReferenceInventoryCreate(&connection, revision, prompt, catalogue, cancel, &references);
        if (status == UMI_STATUS_OK) { *out_references = references; references = NULL; }
    }
done:
    InspectionRelease(&bytes, &length);
    UmiComfyPreflightDestroy(report);
    UmiComfyReferenceInventoryDestroy(references);
    UmiComfyNodeCatalogueDestroy(catalogue);
    UmiComfyPromptDestroy(prompt);
    umi_secret_clear(&connection, sizeof(connection));
    umi_secret_clear(&draft, sizeof(draft));
    return status;
}

UmiStatus UmiComfyInspectionRead(const UmiComfyInspectionPaths *paths,
    const UmiCancellationToken *cancel,UmiComfyPreflightReport **out)
{
    return InspectionCompose(paths,cancel,out,NULL);
}
UmiStatus UmiComfyReferenceInspectionRead(const UmiComfyInspectionPaths *paths,
    const UmiCancellationToken *cancel,UmiComfyReferenceInventory **out)
{
    return InspectionCompose(paths,cancel,NULL,out);
}
