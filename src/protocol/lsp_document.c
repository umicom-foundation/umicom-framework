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

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiLspDocumentRegistry *registry,
                         const char *uri)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->documents[index].uri, uri) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/*
 * Initialise lsp document registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_lsp_document_registry_create(
    UmiLspDocumentRegistry **out_registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Release or reset state held by lsp document registry so the same storage can be reused
 * safely.
 */
void umi_lsp_document_registry_destroy(UmiLspDocumentRegistry *registry)
{
    free(registry);
}

/*
 * Provide the lsp document registry open operation used by this module and its client
 * applications.
 */
UmiStatus umi_lsp_document_registry_open(
    UmiLspDocumentRegistry *registry,
    const char *uri,
    const char *language_id,
    int version)
{
    UmiLspDocument *document;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || uri == NULL || language_id == NULL ||
        version < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (find_index(registry, uri) != SIZE_MAX) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the lsp document registry change operation used by this module and its client
 * applications.
 */
UmiStatus umi_lsp_document_registry_change(
    UmiLspDocumentRegistry *registry,
    const char *uri,
    int version)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || uri == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, uri);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (version <= registry->documents[index].version) {
        return UMI_STATUS_INVALID_STATE;
    }
    registry->documents[index].version = version;
    return UMI_STATUS_OK;
}

/*
 * Provide the lsp document registry close operation used by this module and its client
 * applications.
 */
UmiStatus umi_lsp_document_registry_close(
    UmiLspDocumentRegistry *registry,
    const char *uri)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || uri == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, uri);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    registry->documents[index].open = 0;
    return UMI_STATUS_OK;
}

/*
 * Find lsp document registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiLspDocument *umi_lsp_document_registry_find(
    const UmiLspDocumentRegistry *registry,
    const char *uri)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || uri == NULL) {
        return NULL;
    }
    index = find_index(registry, uri);
    return index != SIZE_MAX ? &registry->documents[index] : NULL;
}

/*
 * Return the number of records represented by lsp document registry without changing their
 * state.
 */
size_t umi_lsp_document_registry_count(
    const UmiLspDocumentRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
