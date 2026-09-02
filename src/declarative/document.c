/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/document.c
 *
 * PURPOSE:
 *   Implement the authoritative declarative document with revision tracking and subtree-safe removal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/document.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeclDocument {
    char application_id[UMI_DECL_ID_CAPACITY];
    UmiDeclVersion version;
    UmiDeclNode *nodes;
    size_t node_count;
    size_t capacity;
    uint64_t revision;
};

/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(UmiDeclDocument *document, size_t required)
{
    size_t capacity;
    UmiDeclNode *nodes;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= document->capacity) return UMI_STATUS_OK;
    capacity = document->capacity == 0U ? 32U : document->capacity * 2U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) capacity *= 2U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > UMI_DECL_MAX_COMPONENTS) capacity = UMI_DECL_MAX_COMPONENTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity < required) return UMI_STATUS_CAPACITY_EXCEEDED;
    nodes = (UmiDeclNode *)realloc(document->nodes, capacity * sizeof(*nodes));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (nodes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    document->nodes = nodes;
    document->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find index operation used by this module and its client applications. */
static ptrdiff_t find_index(const UmiDeclDocument *document, const char *node_id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || node_id == NULL) return -1;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < document->node_count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(document->nodes[i].node_id, node_id) == 0) return (ptrdiff_t)i;
    }
    return -1;
}

/*
 * Initialise decl document from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_decl_document_create(const char *application_id, UmiDeclDocument **out_document)
{
    UmiDeclDocument *document;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!umi_decl_id_is_valid(application_id) || out_document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_document = NULL;
    document = (UmiDeclDocument *)calloc(1U, sizeof(*document));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)umi_decl_copy_text(document->application_id, sizeof(document->application_id), application_id);
    document->version = umi_decl_version_current();
    document->revision = 1U;
    *out_document = document;
    return UMI_STATUS_OK;
}

/* Release or reset state held by decl document so the same storage can be reused safely. */
void umi_decl_document_destroy(UmiDeclDocument *document)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) return;
    free(document->nodes);
    free(document);
}

/*
 * Provide the decl document clone operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_clone(const UmiDeclDocument *source, UmiDeclDocument **out_document)
{
    UmiDeclDocument *copy;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || out_document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_decl_document_create(source->application_id, &copy);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    copy->version = source->version;
    copy->revision = source->revision;
    status = ensure_capacity(copy, source->node_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && source->node_count > 0U) {
        (void)memcpy(copy->nodes, source->nodes, source->node_count * sizeof(source->nodes[0]));
        copy->node_count = source->node_count;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_decl_document_destroy(copy);
        return status;
    }
    *out_document = copy;
    return UMI_STATUS_OK;
}

/*
 * Provide the decl document set version operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_set_version(UmiDeclDocument *document, UmiDeclVersion version)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || version.major == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    document->version = version;
    document->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the decl document add node operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_add_node(UmiDeclDocument *document, const UmiDeclNode *node)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (find_index(document, node->node_id) >= 0) return UMI_STATUS_ALREADY_EXISTS;
    status = ensure_capacity(document, document->node_count + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    document->nodes[document->node_count++] = *node;
    document->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the decl document update node operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_update_node(UmiDeclDocument *document, const UmiDeclNode *node)
{
    ptrdiff_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(document, node->node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index < 0) return UMI_STATUS_NOT_FOUND;
    document->nodes[(size_t)index] = *node;
    document->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the decl document remove node operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_remove_node(UmiDeclDocument *document, const char *node_id)
{
    size_t i = 0U;
    int removed = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (find_index(document, node_id) < 0) return UMI_STATUS_NOT_FOUND;

    /* Removing a component also removes its descendants so no orphaned tree state remains. */
    while (i < document->node_count) {
        int erase = strcmp(document->nodes[i].node_id, node_id) == 0 ||
                    strcmp(document->nodes[i].parent_id, node_id) == 0;
        /* Apply this branch only when its contract condition is satisfied. */
        if (erase) {
            char child_id[UMI_DECL_ID_CAPACITY];
            size_t remaining;
            (void)umi_decl_copy_text(child_id, sizeof(child_id), document->nodes[i].node_id);
            remaining = document->node_count - i - 1U;
            /* Apply this branch only when its contract condition is satisfied. */
            if (remaining > 0U) (void)memmove(&document->nodes[i], &document->nodes[i + 1U], remaining * sizeof(document->nodes[0]));
            document->node_count -= 1U;
            removed = 1;
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(child_id, node_id) != 0) {
                (void)umi_decl_document_remove_node(document, child_id);
            }
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            i += 1U;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (removed) document->revision += 1U;
    return removed ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the decl document find node operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_find_node(const UmiDeclDocument *document, const char *node_id, UmiDeclNode *out_node)
{
    ptrdiff_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || node_id == NULL || out_node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(document, node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index < 0) return UMI_STATUS_NOT_FOUND;
    *out_node = document->nodes[(size_t)index];
    return UMI_STATUS_OK;
}

/*
 * Find decl document node while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_decl_document_node_at(const UmiDeclDocument *document, size_t index, UmiDeclNode *out_node)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= document->node_count) return UMI_STATUS_NOT_FOUND;
    *out_node = document->nodes[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the decl document snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_document_snapshot(const UmiDeclDocument *document, UmiDeclDocumentSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)umi_decl_copy_text(out_snapshot->application_id, sizeof(out_snapshot->application_id), document->application_id);
    out_snapshot->version = document->version;
    out_snapshot->node_count = document->node_count;
    out_snapshot->revision = document->revision;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by decl document node without changing their
 * state.
 */
size_t umi_decl_document_node_count(const UmiDeclDocument *document)
{
    return document != NULL ? document->node_count : 0U;
}
