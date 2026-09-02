/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/serializer.c
 *
 * PURPOSE:
 *   Write deterministic declarative source so generated files remain reviewable and version-control friendly.
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

#include "umicom/declarative/serializer.h"

#include <stdio.h>
#include <string.h>

/* Provide the append text operation used by this module and its client applications. */
static UmiStatus append_text(char *out, size_t cap, size_t *used, const char *text)
{
    size_t length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*used + length + 1U > cap) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out + *used, text, length);
    *used += length;
    out[*used] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Write decl in its stable representation and report capacity or input failures to the
 * caller.
 */
UmiStatus umi_decl_serialize(const UmiDeclDocument *document, char *out_text, size_t capacity, size_t *out_length)
{
    UmiDeclDocumentSnapshot snapshot;
    size_t used = 0U;
    size_t i;
    char line[1024];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_text == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out_text[0] = '\0';
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_decl_document_snapshot(document, &snapshot) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    (void)snprintf(line, sizeof(line), "application %s\nversion %u.%u.%u\n", snapshot.application_id, (unsigned int)snapshot.version.major, (unsigned int)snapshot.version.minor, (unsigned int)snapshot.version.patch);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (append_text(out_text, capacity, &used, line) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Components are emitted in document order, followed by their properties for readable diffs. */
    for (i = 0U; i < snapshot.node_count; ++i) {
        UmiDeclNode node;
        size_t p;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_decl_document_node_at(document, i, &node) != UMI_STATUS_OK) return UMI_STATUS_INTERNAL_ERROR;
        (void)snprintf(line, sizeof(line), "component %s %s %s\n", node.node_id, node.component_type, node.parent_id[0] != '\0' ? node.parent_id : "-");
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (append_text(out_text, capacity, &used, line) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
        /* Visit each bounded item once so every record receives the same rule. */
        for (p = 0U; p < node.attribute_count; ++p) {
            char value[UMI_DECL_TEXT_CAPACITY];
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (umi_decl_value_as_text(&node.attributes[p].value, value, sizeof(value)) != UMI_STATUS_OK) return UMI_STATUS_INTERNAL_ERROR;
            (void)snprintf(line, sizeof(line), "property %s %s %s\n", node.node_id, node.attributes[p].name, value);
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (append_text(out_text, capacity, &used, line) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_length != NULL) *out_length = used;
    return UMI_STATUS_OK;
}

/*
 * Provide the decl serialize file operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_serialize_file(const UmiDeclDocument *document, const char *path)
{
    char text[65536];
    size_t length;
    FILE *file;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_decl_serialize(document, text, sizeof(text), &length);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    file = fopen(path, "wb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U && fwrite(text, 1U, length, file) != length) { (void)fclose(file); return UMI_STATUS_IO_ERROR; }
    return fclose(file) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}
