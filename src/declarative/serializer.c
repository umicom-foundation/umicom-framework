/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/serializer.c
 *
 * PURPOSE:
 *   Write deterministic declarative source so generated files remain reviewable and version-control friendly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/serializer.h"

#include <stdio.h>
#include <string.h>

static UmiStatus append_text(char *out, size_t cap, size_t *used, const char *text)
{
    size_t length = strlen(text);
    if (*used + length + 1U > cap) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out + *used, text, length);
    *used += length;
    out[*used] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_decl_serialize(const UmiDeclDocument *document, char *out_text, size_t capacity, size_t *out_length)
{
    UmiDeclDocumentSnapshot snapshot;
    size_t used = 0U;
    size_t i;
    char line[1024];
    if (document == NULL || out_text == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out_text[0] = '\0';
    if (umi_decl_document_snapshot(document, &snapshot) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    (void)snprintf(line, sizeof(line), "application %s\nversion %u.%u.%u\n", snapshot.application_id, (unsigned int)snapshot.version.major, (unsigned int)snapshot.version.minor, (unsigned int)snapshot.version.patch);
    if (append_text(out_text, capacity, &used, line) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Components are emitted in document order, followed by their properties for readable diffs. */
    for (i = 0U; i < snapshot.node_count; ++i) {
        UmiDeclNode node;
        size_t p;
        if (umi_decl_document_node_at(document, i, &node) != UMI_STATUS_OK) return UMI_STATUS_INTERNAL_ERROR;
        (void)snprintf(line, sizeof(line), "component %s %s %s\n", node.node_id, node.component_type, node.parent_id[0] != '\0' ? node.parent_id : "-");
        if (append_text(out_text, capacity, &used, line) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
        for (p = 0U; p < node.attribute_count; ++p) {
            char value[UMI_DECL_TEXT_CAPACITY];
            if (umi_decl_value_as_text(&node.attributes[p].value, value, sizeof(value)) != UMI_STATUS_OK) return UMI_STATUS_INTERNAL_ERROR;
            (void)snprintf(line, sizeof(line), "property %s %s %s\n", node.node_id, node.attributes[p].name, value);
            if (append_text(out_text, capacity, &used, line) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    if (out_length != NULL) *out_length = used;
    return UMI_STATUS_OK;
}

UmiStatus umi_decl_serialize_file(const UmiDeclDocument *document, const char *path)
{
    char text[65536];
    size_t length;
    FILE *file;
    UmiStatus status;
    if (path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_decl_serialize(document, text, sizeof(text), &length);
    if (status != UMI_STATUS_OK) return status;
    file = fopen(path, "wb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    if (length > 0U && fwrite(text, 1U, length, file) != length) { (void)fclose(file); return UMI_STATUS_IO_ERROR; }
    return fclose(file) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}
