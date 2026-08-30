/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_document.c
 *
 * PURPOSE:
 *   Implement bounded LSP document registration and monotonic version tracking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/lsp_document.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiLspDocumentRegistry {
    UmiLspDocument documents[UMI_PROTOCOL_MAX_DOCUMENTS];
    size_t count;
};

static size_t find_index(const UmiLspDocumentRegistry *registry,
                         const char *uri)
{
    size_t index;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->documents[index].uri, uri) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

UmiStatus umi_lsp_document_registry_create(
    UmiLspDocumentRegistry **out_registry)
{
    if (out_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_registry = (UmiLspDocumentRegistry *)calloc(
        1U,
        sizeof(UmiLspDocumentRegistry)
    );
    return *out_registry != NULL
        ? UMI_STATUS_OK
        : UMI_STATUS_OUT_OF_MEMORY;
}

void umi_lsp_document_registry_destroy(UmiLspDocumentRegistry *registry)
{
    free(registry);
}

UmiStatus umi_lsp_document_registry_open(
    UmiLspDocumentRegistry *registry,
    const char *uri,
    const char *language_id,
    int version)
{
    UmiLspDocument *document;
    if (registry == NULL || uri == NULL || language_id == NULL ||
        version < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_index(registry, uri) != SIZE_MAX) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->count >= UMI_PROTOCOL_MAX_DOCUMENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    document = &registry->documents[registry->count++];
    (void)memset(document, 0, sizeof(*document));
    (void)snprintf(document->uri, sizeof(document->uri), "%s", uri);
    (void)snprintf(document->language_id,
                   sizeof(document->language_id),
                   "%s",
                   language_id);
    document->version = version;
    document->open = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_lsp_document_registry_change(
    UmiLspDocumentRegistry *registry,
    const char *uri,
    int version)
{
    size_t index;
    if (registry == NULL || uri == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, uri);
    if (index == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (version <= registry->documents[index].version) {
        return UMI_STATUS_INVALID_STATE;
    }
    registry->documents[index].version = version;
    return UMI_STATUS_OK;
}

UmiStatus umi_lsp_document_registry_close(
    UmiLspDocumentRegistry *registry,
    const char *uri)
{
    size_t index;
    if (registry == NULL || uri == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, uri);
    if (index == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    registry->documents[index].open = 0;
    return UMI_STATUS_OK;
}

const UmiLspDocument *umi_lsp_document_registry_find(
    const UmiLspDocumentRegistry *registry,
    const char *uri)
{
    size_t index;
    if (registry == NULL || uri == NULL) {
        return NULL;
    }
    index = find_index(registry, uri);
    return index != SIZE_MAX ? &registry->documents[index] : NULL;
}

size_t umi_lsp_document_registry_count(
    const UmiLspDocumentRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
