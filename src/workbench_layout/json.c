/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/json.c
 *
 * PURPOSE:
 *   Encode and decode the portable semantic .umilayout representation without
 *   introducing filesystem, database or widget-toolkit dependencies.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/json.h"

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "umicom/workbench_layout/migration.h"

#define UMI_JSON_NUMBER_CAPACITY 96U
#define UMI_JSON_KEY_CAPACITY 96U

typedef struct UmiJsonWriter {
    char *buffer;
    size_t capacity;
    size_t written;
    size_t required;
    bool overflow;
    bool pretty;
    uint32_t indent_width;
    uint32_t depth;
} UmiJsonWriter;

typedef struct UmiJsonParser {
    const char *text;
    size_t length;
    size_t offset;
    size_t error_offset;
    char message[UMI_WORKBENCH_LAYOUT_ERROR_CAPACITY];
} UmiJsonParser;

static void json_result_init(UmiWorkbenchLayoutJsonResult *result)
{
    if (result == NULL) return;
    (void)memset(result, 0, sizeof(*result));
    result->structure_size = sizeof(*result);
    result->status = UMI_STATUS_OK;
}

static UmiStatus json_result_finish(
    UmiWorkbenchLayoutJsonResult *result,
    UmiStatus status,
    size_t written,
    size_t required,
    size_t error_offset,
    const char *message)
{
    if (result != NULL) {
        result->status = status;
        result->bytes_written = written;
        result->bytes_required = required;
        result->error_offset = error_offset;
        (void)umi_workbench_layout_copy_text(
            result->message,
            sizeof(result->message),
            message != NULL ? message : "",
            true);
    }
    return status;
}

static void writer_write_bytes(
    UmiJsonWriter *writer,
    const char *text,
    size_t length)
{
    size_t available;
    size_t copy_count;

    if (writer == NULL || text == NULL) return;
    writer->required += length;
    if (writer->buffer == NULL || writer->capacity == 0U) {
        writer->overflow = true;
        return;
    }
    available = writer->written < writer->capacity
        ? writer->capacity - writer->written
        : 0U;
    if (available == 0U) {
        writer->overflow = true;
        return;
    }
    copy_count = length < available - 1U ? length : available - 1U;
    if (copy_count > 0U) {
        (void)memcpy(writer->buffer + writer->written, text, copy_count);
        writer->written += copy_count;
    }
    writer->buffer[writer->written] = '\0';
    if (copy_count != length) writer->overflow = true;
}

static void writer_write(UmiJsonWriter *writer, const char *text)
{
    if (text != NULL) writer_write_bytes(writer, text, strlen(text));
}

static void writer_format(UmiJsonWriter *writer, const char *format, ...)
{
    char temporary[256U];
    va_list arguments;
    int written;

    va_start(arguments, format);
    written = vsnprintf(temporary, sizeof(temporary), format, arguments);
    va_end(arguments);
    if (written < 0) {
        writer->overflow = true;
        return;
    }
    if ((size_t)written < sizeof(temporary)) {
        writer_write_bytes(writer, temporary, (size_t)written);
        return;
    }
    {
        const size_t required = (size_t)written + 1U;
        char *dynamic = (char *)malloc(required);
        if (dynamic == NULL) {
            writer->overflow = true;
            return;
        }
        va_start(arguments, format);
        written = vsnprintf(dynamic, required, format, arguments);
        va_end(arguments);
        if (written >= 0) {
            writer_write_bytes(writer, dynamic, (size_t)written);
        } else {
            writer->overflow = true;
        }
        free(dynamic);
    }
}

static void writer_indent(UmiJsonWriter *writer)
{
    uint32_t index;
    uint32_t count;
    if (writer == NULL || !writer->pretty) return;
    count = writer->depth * writer->indent_width;
    for (index = 0U; index < count; ++index) writer_write(writer, " ");
}

static void writer_newline(UmiJsonWriter *writer)
{
    if (writer != NULL && writer->pretty) writer_write(writer, "\n");
}

static void writer_space(UmiJsonWriter *writer)
{
    if (writer != NULL && writer->pretty) writer_write(writer, " ");
}

static void writer_begin_object(UmiJsonWriter *writer)
{
    writer_write(writer, "{");
    writer->depth += 1U;
}

static void writer_end_object(UmiJsonWriter *writer)
{
    if (writer->depth > 0U) writer->depth -= 1U;
    writer_newline(writer);
    writer_indent(writer);
    writer_write(writer, "}");
}

static void writer_begin_array(UmiJsonWriter *writer)
{
    writer_write(writer, "[");
    writer->depth += 1U;
}

static void writer_end_array(UmiJsonWriter *writer)
{
    if (writer->depth > 0U) writer->depth -= 1U;
    writer_newline(writer);
    writer_indent(writer);
    writer_write(writer, "]");
}

static void writer_member_prefix(
    UmiJsonWriter *writer,
    bool *first,
    const char *key)
{
    if (!*first) writer_write(writer, ",");
    writer_newline(writer);
    writer_indent(writer);
    writer_format(writer, "\"%s\":", key);
    writer_space(writer);
    *first = false;
}

static void writer_array_item_prefix(UmiJsonWriter *writer, bool *first)
{
    if (!*first) writer_write(writer, ",");
    writer_newline(writer);
    writer_indent(writer);
    *first = false;
}

static void writer_string(UmiJsonWriter *writer, const char *text)
{
    const unsigned char *bytes =
        (const unsigned char *)(text != NULL ? text : "");
    size_t index;

    writer_write(writer, "\"");
    for (index = 0U; bytes[index] != '\0'; ++index) {
        const unsigned char character = bytes[index];
        switch (character) {
        case '\"': writer_write(writer, "\\\""); break;
        case '\\': writer_write(writer, "\\\\"); break;
        case '\b': writer_write(writer, "\\b"); break;
        case '\f': writer_write(writer, "\\f"); break;
        case '\n': writer_write(writer, "\\n"); break;
        case '\r': writer_write(writer, "\\r"); break;
        case '\t': writer_write(writer, "\\t"); break;
        default:
            if (character < 0x20U) {
                writer_format(writer, "\\u%04x", (unsigned int)character);
            } else {
                const char output = (char)character;
                writer_write_bytes(writer, &output, 1U);
            }
            break;
        }
    }
    writer_write(writer, "\"");
}

static void writer_size_index(UmiJsonWriter *writer, size_t value)
{
    if (value == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        writer_write(writer, "-1");
    } else {
        writer_format(writer, "%zu", value);
    }
}


static void writer_rect(
    UmiJsonWriter *writer,
    const UmiWorkbenchLayoutRect *rect)
{
    bool first = true;
    writer_begin_object(writer);
    writer_member_prefix(writer, &first, "x");
    writer_format(writer, "%" PRId32, rect->x);
    writer_member_prefix(writer, &first, "y");
    writer_format(writer, "%" PRId32, rect->y);
    writer_member_prefix(writer, &first, "width");
    writer_format(writer, "%" PRId32, rect->width);
    writer_member_prefix(writer, &first, "height");
    writer_format(writer, "%" PRId32, rect->height);
    writer_end_object(writer);
}

static void writer_size(
    UmiJsonWriter *writer,
    const UmiWorkbenchLayoutSize *size)
{
    bool first = true;
    writer_begin_object(writer);
    writer_member_prefix(writer, &first, "width");
    writer_format(writer, "%" PRId32, size->width);
    writer_member_prefix(writer, &first, "height");
    writer_format(writer, "%" PRId32, size->height);
    writer_end_object(writer);
}

static void writer_node(
    UmiJsonWriter *writer,
    const UmiWorkbenchLayoutNode *node)
{
    bool first = true;
    bool first_child = true;
    size_t index;

    writer_begin_object(writer);
    writer_member_prefix(writer, &first, "node_id");
    writer_string(writer, node->node_id);
    writer_member_prefix(writer, &first, "title");
    writer_string(writer, node->title);
    writer_member_prefix(writer, &first, "component_id");
    writer_string(writer, node->component_id);
    writer_member_prefix(writer, &first, "owner_application_id");
    writer_string(writer, node->owner_application_id);
    writer_member_prefix(writer, &first, "context_group_id");
    writer_string(writer, node->context_group_id);
    writer_member_prefix(writer, &first, "monitor_id");
    writer_string(writer, node->monitor_id);
    writer_member_prefix(writer, &first, "kind");
    writer_string(writer, umi_workbench_layout_node_kind_text(node->kind));
    writer_member_prefix(writer, &first, "orientation");
    writer_string(
        writer,
        umi_workbench_layout_orientation_text(node->orientation));
    writer_member_prefix(writer, &first, "dock_region");
    writer_string(
        writer,
        umi_workbench_layout_dock_region_text(node->dock_region));
    writer_member_prefix(writer, &first, "visibility");
    writer_string(
        writer,
        umi_workbench_layout_visibility_text(node->visibility));
    writer_member_prefix(writer, &first, "split_ratio");
    writer_format(writer, "%.17g", node->split_ratio);
    writer_member_prefix(writer, &first, "bounds");
    writer_rect(writer, &node->bounds);
    writer_member_prefix(writer, &first, "minimum_size");
    writer_size(writer, &node->minimum_size);
    writer_member_prefix(writer, &first, "preferred_size");
    writer_size(writer, &node->preferred_size);
    writer_member_prefix(writer, &first, "parent_index");
    writer_size_index(writer, node->parent_index);
    writer_member_prefix(writer, &first, "children");
    writer_begin_array(writer);
    for (index = 0U; index < node->child_count; ++index) {
        writer_array_item_prefix(writer, &first_child);
        writer_size_index(writer, node->child_indices[index]);
    }
    writer_end_array(writer);
    writer_member_prefix(writer, &first, "active_child_index");
    writer_size_index(writer, node->active_child_index);
    writer_member_prefix(writer, &first, "flags");
    writer_format(writer, "%" PRIu32, node->flags);
    writer_member_prefix(writer, &first, "order");
    writer_format(writer, "%" PRId32, node->order);
    writer_member_prefix(writer, &first, "z_order");
    writer_format(writer, "%" PRId32, node->z_order);
    writer_member_prefix(writer, &first, "revision");
    writer_format(writer, "%" PRIu64, node->revision);
    writer_end_object(writer);
}

static void writer_identity(
    UmiJsonWriter *writer,
    const UmiWorkbenchLayoutIdentity *identity)
{
    bool first = true;
    writer_begin_object(writer);
    writer_member_prefix(writer, &first, "layout_id");
    writer_string(writer, identity->layout_id);
    writer_member_prefix(writer, &first, "owner_user_id");
    writer_string(writer, identity->owner_user_id);
    writer_member_prefix(writer, &first, "owner_application_id");
    writer_string(writer, identity->owner_application_id);
    writer_member_prefix(writer, &first, "workspace_id");
    writer_string(writer, identity->workspace_id);
    writer_end_object(writer);
}

static void writer_version(
    UmiJsonWriter *writer,
    const UmiWorkbenchLayoutVersion *version)
{
    bool first = true;
    writer_begin_object(writer);
    writer_member_prefix(writer, &first, "schema_version");
    writer_format(writer, "%" PRIu32, version->schema_version);
    writer_member_prefix(writer, &first, "revision");
    writer_format(writer, "%" PRIu64, version->revision);
    writer_member_prefix(writer, &first, "generation");
    writer_format(writer, "%" PRIu64, version->generation);
    writer_member_prefix(writer, &first, "base_revision");
    writer_format(writer, "%" PRIu64, version->base_revision);
    writer_end_object(writer);
}

static void writer_audit(
    UmiJsonWriter *writer,
    const UmiWorkbenchLayoutAudit *audit)
{
    bool first = true;
    writer_begin_object(writer);
    writer_member_prefix(writer, &first, "created_by");
    writer_string(writer, audit->created_by);
    writer_member_prefix(writer, &first, "modified_by");
    writer_string(writer, audit->modified_by);
    writer_member_prefix(writer, &first, "created_at_ms");
    writer_format(writer, "%" PRIu64, audit->created_at_ms);
    writer_member_prefix(writer, &first, "modified_at_ms");
    writer_format(writer, "%" PRIu64, audit->modified_at_ms);
    writer_member_prefix(writer, &first, "correlation_id");
    writer_string(writer, audit->correlation_id);
    writer_end_object(writer);
}

static void writer_document(
    UmiJsonWriter *writer,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutJsonOptions *options)
{
    bool first = true;
    bool first_tag = true;
    bool first_node = true;
    size_t index;

    writer_begin_object(writer);
    writer_member_prefix(writer, &first, "$schema");
    writer_string(
        writer,
        "https://schemas.umicom.org/workbench-layout.schema.json");
    writer_member_prefix(writer, &first, "schema");
    writer_string(writer, "umicom.workbench-layout/2");
    writer_member_prefix(writer, &first, "layout");
    {
        bool layout_first = true;
        writer_begin_object(writer);
        writer_member_prefix(writer, &layout_first, "identity");
        writer_identity(writer, &document->identity);
        writer_member_prefix(writer, &layout_first, "version");
        writer_version(writer, &document->version);
        if (options->include_audit) {
            writer_member_prefix(writer, &layout_first, "audit");
            writer_audit(writer, &document->audit);
        }
        writer_member_prefix(writer, &layout_first, "name");
        writer_string(writer, document->name);
        writer_member_prefix(writer, &layout_first, "category");
        writer_string(writer, document->category);
        writer_member_prefix(writer, &layout_first, "description");
        writer_string(writer, document->description);
        writer_member_prefix(writer, &layout_first, "tags");
        writer_begin_array(writer);
        for (index = 0U; index < document->tag_count; ++index) {
            writer_array_item_prefix(writer, &first_tag);
            writer_string(writer, document->tags[index].value);
        }
        writer_end_array(writer);
        writer_member_prefix(writer, &layout_first, "root_index");
        writer_size_index(writer, document->root_index);
        writer_member_prefix(writer, &layout_first, "flags");
        writer_format(writer, "%" PRIu32, document->flags);
        if (options->include_content_hash) {
            writer_member_prefix(writer, &layout_first, "content_hash");
            writer_format(writer, "\"%" PRIu64 "\"", document->content_hash);
        }
        writer_member_prefix(writer, &layout_first, "nodes");
        writer_begin_array(writer);
        for (index = 0U; index < document->node_count; ++index) {
            UmiWorkbenchLayoutNode node = document->nodes[index];
            if (!options->include_runtime_geometry) {
                (void)memset(&node.bounds, 0, sizeof(node.bounds));
                node.monitor_id[0] = '\0';
                node.z_order = 0;
            }
            writer_array_item_prefix(writer, &first_node);
            writer_node(writer, &node);
        }
        writer_end_array(writer);
        writer_end_object(writer);
    }
    writer_end_object(writer);
    writer_newline(writer);
}

UmiWorkbenchLayoutJsonOptions
umi_workbench_layout_json_options_default(void)
{
    UmiWorkbenchLayoutJsonOptions options;
    (void)memset(&options, 0, sizeof(options));
    options.structure_size = sizeof(options);
    options.pretty = true;
    options.include_audit = true;
    options.include_runtime_geometry = true;
    options.include_content_hash = true;
    options.indent_width = 2U;
    return options;
}

UmiStatus umi_workbench_layout_json_encode(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutJsonOptions *options,
    char *buffer,
    size_t capacity,
    UmiWorkbenchLayoutJsonResult *out_result)
{
    UmiWorkbenchLayoutJsonOptions effective;
    UmiJsonWriter writer;
    UmiStatus status;

    json_result_init(out_result);
    if (document == NULL || (buffer == NULL && capacity != 0U)) {
        return json_result_finish(
            out_result,
            UMI_STATUS_INVALID_ARGUMENT,
            0U,
            0U,
            0U,
            "A layout document and a valid output buffer are required.");
    }
    status = umi_workbench_layout_document_validate_structure(document);
    if (status != UMI_STATUS_OK) {
        return json_result_finish(
            out_result,
            status,
            0U,
            0U,
            0U,
            "The semantic layout document is not structurally valid.");
    }
    effective = options != NULL
        ? *options
        : umi_workbench_layout_json_options_default();
    if (effective.structure_size < sizeof(effective) ||
        effective.indent_width > 16U) {
        return json_result_finish(
            out_result,
            UMI_STATUS_INVALID_ARGUMENT,
            0U,
            0U,
            0U,
            "The JSON encoding options are invalid.");
    }

    (void)memset(&writer, 0, sizeof(writer));
    writer.buffer = buffer;
    writer.capacity = capacity;
    writer.pretty = effective.pretty;
    writer.indent_width = effective.indent_width;
    if (buffer != NULL && capacity > 0U) buffer[0] = '\0';
    writer_document(&writer, document, &effective);

    if (writer.overflow) {
        return json_result_finish(
            out_result,
            UMI_STATUS_CAPACITY_EXCEEDED,
            writer.written,
            writer.required + 1U,
            0U,
            "The output buffer is too small for the encoded layout.");
    }
    return json_result_finish(
        out_result,
        UMI_STATUS_OK,
        writer.written,
        writer.required + 1U,
        0U,
        "The layout was encoded successfully.");
}

static UmiStatus parser_fail(UmiJsonParser *parser, const char *message)
{
    if (parser != NULL) {
        parser->error_offset = parser->offset;
        (void)umi_workbench_layout_copy_text(
            parser->message,
            sizeof(parser->message),
            message != NULL ? message : "JSON parsing failed.",
            true);
    }
    return UMI_STATUS_PARSE_ERROR;
}

static void parser_skip_space(UmiJsonParser *parser)
{
    while (parser->offset < parser->length) {
        const unsigned char character =
            (unsigned char)parser->text[parser->offset];
        if (character != ' ' && character != '\t' &&
            character != '\n' && character != '\r') {
            break;
        }
        parser->offset += 1U;
    }
}

static bool parser_consume(UmiJsonParser *parser, char character)
{
    parser_skip_space(parser);
    if (parser->offset < parser->length &&
        parser->text[parser->offset] == character) {
        parser->offset += 1U;
        return true;
    }
    return false;
}

static bool parser_match_literal(
    UmiJsonParser *parser,
    const char *literal)
{
    const size_t length = strlen(literal);
    parser_skip_space(parser);
    if (parser->offset + length > parser->length ||
        memcmp(parser->text + parser->offset, literal, length) != 0) {
        return false;
    }
    parser->offset += length;
    return true;
}

static int hexadecimal_value(char character)
{
    if (character >= '0' && character <= '9') return character - '0';
    if (character >= 'a' && character <= 'f') return character - 'a' + 10;
    if (character >= 'A' && character <= 'F') return character - 'A' + 10;
    return -1;
}

static UmiStatus append_utf8_codepoint(
    uint32_t codepoint,
    char *buffer,
    size_t capacity,
    size_t *length)
{
    unsigned char encoded[4U];
    size_t count;

    if (codepoint <= 0x7FU) {
        encoded[0] = (unsigned char)codepoint;
        count = 1U;
    } else if (codepoint <= 0x7FFU) {
        encoded[0] = (unsigned char)(0xC0U | (codepoint >> 6U));
        encoded[1] = (unsigned char)(0x80U | (codepoint & 0x3FU));
        count = 2U;
    } else if (codepoint <= 0xFFFFU) {
        encoded[0] = (unsigned char)(0xE0U | (codepoint >> 12U));
        encoded[1] = (unsigned char)(0x80U | ((codepoint >> 6U) & 0x3FU));
        encoded[2] = (unsigned char)(0x80U | (codepoint & 0x3FU));
        count = 3U;
    } else if (codepoint <= 0x10FFFFU) {
        encoded[0] = (unsigned char)(0xF0U | (codepoint >> 18U));
        encoded[1] = (unsigned char)(0x80U | ((codepoint >> 12U) & 0x3FU));
        encoded[2] = (unsigned char)(0x80U | ((codepoint >> 6U) & 0x3FU));
        encoded[3] = (unsigned char)(0x80U | (codepoint & 0x3FU));
        count = 4U;
    } else {
        return UMI_STATUS_PARSE_ERROR;
    }
    if (buffer != NULL && *length + count >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (buffer != NULL) {
        (void)memcpy(buffer + *length, encoded, count);
    }
    *length += count;
    return UMI_STATUS_OK;
}

static UmiStatus parser_string(
    UmiJsonParser *parser,
    char *buffer,
    size_t capacity)
{
    size_t output_length = 0U;

    parser_skip_space(parser);
    if (!parser_consume(parser, '\"')) {
        return parser_fail(parser, "Expected a JSON string.");
    }
    while (parser->offset < parser->length) {
        unsigned char character =
            (unsigned char)parser->text[parser->offset++];
        if (character == '\"') {
            if (buffer != NULL) {
                if (capacity == 0U || output_length >= capacity) {
                    return UMI_STATUS_CAPACITY_EXCEEDED;
                }
                buffer[output_length] = '\0';
            }
            return UMI_STATUS_OK;
        }
        if (character == '\\') {
            if (parser->offset >= parser->length) {
                return parser_fail(parser, "Incomplete JSON escape sequence.");
            }
            character = (unsigned char)parser->text[parser->offset++];
            switch (character) {
            case '\"': character = '\"'; break;
            case '\\': character = '\\'; break;
            case '/': character = '/'; break;
            case 'b': character = '\b'; break;
            case 'f': character = '\f'; break;
            case 'n': character = '\n'; break;
            case 'r': character = '\r'; break;
            case 't': character = '\t'; break;
            case 'u': {
                uint32_t codepoint = 0U;
                size_t digit;
                for (digit = 0U; digit < 4U; ++digit) {
                    int value;
                    if (parser->offset >= parser->length) {
                        return parser_fail(
                            parser,
                            "Incomplete Unicode escape sequence.");
                    }
                    value = hexadecimal_value(
                        parser->text[parser->offset++]);
                    if (value < 0) {
                        return parser_fail(
                            parser,
                            "Invalid Unicode escape sequence.");
                    }
                    codepoint = (codepoint << 4U) | (uint32_t)value;
                }
                {
                    UmiStatus status = append_utf8_codepoint(
                        codepoint, buffer, capacity, &output_length);
                    if (status != UMI_STATUS_OK) return status;
                }
                continue;
            }
            default:
                return parser_fail(parser, "Unsupported JSON escape sequence.");
            }
        } else if (character < 0x20U) {
            return parser_fail(parser, "Control character inside JSON string.");
        }
        if (buffer != NULL && output_length + 1U >= capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        if (buffer != NULL) buffer[output_length] = (char)character;
        output_length += 1U;
    }
    return parser_fail(parser, "Unterminated JSON string.");
}

static UmiStatus parser_number_token(
    UmiJsonParser *parser,
    char *buffer,
    size_t capacity)
{
    size_t start;
    size_t length;

    parser_skip_space(parser);
    start = parser->offset;
    if (parser->offset < parser->length &&
        (parser->text[parser->offset] == '-' ||
         parser->text[parser->offset] == '+')) {
        parser->offset += 1U;
    }
    while (parser->offset < parser->length) {
        const char character = parser->text[parser->offset];
        if ((character >= '0' && character <= '9') ||
            character == '.' || character == 'e' || character == 'E' ||
            character == '+' || character == '-') {
            parser->offset += 1U;
        } else {
            break;
        }
    }
    length = parser->offset - start;
    if (length == 0U || length >= capacity) {
        return parser_fail(parser, "Expected a JSON number.");
    }
    (void)memcpy(buffer, parser->text + start, length);
    buffer[length] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus parser_u64(UmiJsonParser *parser, uint64_t *out_value)
{
    char token[UMI_JSON_NUMBER_CAPACITY];
    char *end;
    unsigned long long value;
    UmiStatus status;

    if (out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    parser_skip_space(parser);
    if (parser->offset < parser->length &&
        parser->text[parser->offset] == '\"') {
        status = parser_string(parser, token, sizeof(token));
    } else {
        status = parser_number_token(parser, token, sizeof(token));
    }
    if (status != UMI_STATUS_OK) return status;
    errno = 0;
    end = NULL;
    value = strtoull(token, &end, 10);
    if (errno != 0 || end == token || *end != '\0') {
        return parser_fail(parser, "Invalid unsigned integer value.");
    }
    *out_value = (uint64_t)value;
    return UMI_STATUS_OK;
}

static UmiStatus parser_i64(UmiJsonParser *parser, int64_t *out_value)
{
    char token[UMI_JSON_NUMBER_CAPACITY];
    char *end;
    long long value;
    UmiStatus status;

    if (out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = parser_number_token(parser, token, sizeof(token));
    if (status != UMI_STATUS_OK) return status;
    errno = 0;
    end = NULL;
    value = strtoll(token, &end, 10);
    if (errno != 0 || end == token || *end != '\0') {
        return parser_fail(parser, "Invalid signed integer value.");
    }
    *out_value = (int64_t)value;
    return UMI_STATUS_OK;
}

static UmiStatus parser_double(UmiJsonParser *parser, double *out_value)
{
    char token[UMI_JSON_NUMBER_CAPACITY];
    char *end;
    double value;
    UmiStatus status;

    if (out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = parser_number_token(parser, token, sizeof(token));
    if (status != UMI_STATUS_OK) return status;
    errno = 0;
    end = NULL;
    value = strtod(token, &end);
    if (errno != 0 || end == token || *end != '\0') {
        return parser_fail(parser, "Invalid floating-point value.");
    }
    *out_value = value;
    return UMI_STATUS_OK;
}

static UmiStatus parser_skip_value(UmiJsonParser *parser);

static UmiStatus parser_skip_object(UmiJsonParser *parser)
{
    char key[UMI_JSON_KEY_CAPACITY];
    UmiStatus status;

    if (!parser_consume(parser, '{')) {
        return parser_fail(parser, "Expected a JSON object.");
    }
    parser_skip_space(parser);
    if (parser_consume(parser, '}')) return UMI_STATUS_OK;
    for (;;) {
        status = parser_string(parser, key, sizeof(key));
        if (status != UMI_STATUS_OK) return status;
        if (!parser_consume(parser, ':')) {
            return parser_fail(parser, "Expected ':' after an object key.");
        }
        status = parser_skip_value(parser);
        if (status != UMI_STATUS_OK) return status;
        if (parser_consume(parser, '}')) return UMI_STATUS_OK;
        if (!parser_consume(parser, ',')) {
            return parser_fail(parser, "Expected ',' between object members.");
        }
    }
}

static UmiStatus parser_skip_array(UmiJsonParser *parser)
{
    UmiStatus status;
    if (!parser_consume(parser, '[')) {
        return parser_fail(parser, "Expected a JSON array.");
    }
    parser_skip_space(parser);
    if (parser_consume(parser, ']')) return UMI_STATUS_OK;
    for (;;) {
        status = parser_skip_value(parser);
        if (status != UMI_STATUS_OK) return status;
        if (parser_consume(parser, ']')) return UMI_STATUS_OK;
        if (!parser_consume(parser, ',')) {
            return parser_fail(parser, "Expected ',' between array items.");
        }
    }
}

static UmiStatus parser_skip_value(UmiJsonParser *parser)
{
    char token[UMI_JSON_NUMBER_CAPACITY];
    parser_skip_space(parser);
    if (parser->offset >= parser->length) {
        return parser_fail(parser, "Unexpected end of JSON input.");
    }
    switch (parser->text[parser->offset]) {
    case '{': return parser_skip_object(parser);
    case '[': return parser_skip_array(parser);
    case '\"': return parser_string(parser, NULL, 0U);
    case 't':
        return parser_match_literal(parser, "true")
            ? UMI_STATUS_OK
            : parser_fail(parser, "Invalid JSON literal.");
    case 'f':
        return parser_match_literal(parser, "false")
            ? UMI_STATUS_OK
            : parser_fail(parser, "Invalid JSON literal.");
    case 'n':
        return parser_match_literal(parser, "null")
            ? UMI_STATUS_OK
            : parser_fail(parser, "Invalid JSON literal.");
    default:
        return parser_number_token(parser, token, sizeof(token));
    }
}

static UmiWorkbenchLayoutNodeKind node_kind_from_text(const char *text)
{
    UmiWorkbenchLayoutNodeKind kind;
    for (kind = UMI_WORKBENCH_LAYOUT_NODE_EMPTY;
         kind <= UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW;
         kind = (UmiWorkbenchLayoutNodeKind)((int)kind + 1)) {
        if (strcmp(text, umi_workbench_layout_node_kind_text(kind)) == 0) {
            return kind;
        }
    }
    return (UmiWorkbenchLayoutNodeKind)0;
}

static UmiWorkbenchLayoutOrientation orientation_from_text(const char *text)
{
    UmiWorkbenchLayoutOrientation value;
    for (value = UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE;
         value <= UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL;
         value = (UmiWorkbenchLayoutOrientation)((int)value + 1)) {
        if (strcmp(text, umi_workbench_layout_orientation_text(value)) == 0) {
            return value;
        }
    }
    return (UmiWorkbenchLayoutOrientation)-1;
}

static UmiWorkbenchLayoutDockRegion dock_region_from_text(const char *text)
{
    UmiWorkbenchLayoutDockRegion value;
    for (value = UMI_WORKBENCH_LAYOUT_DOCK_CANVAS;
         value <= UMI_WORKBENCH_LAYOUT_DOCK_FLOATING;
         value = (UmiWorkbenchLayoutDockRegion)((int)value + 1)) {
        if (strcmp(text, umi_workbench_layout_dock_region_text(value)) == 0) {
            return value;
        }
    }
    return (UmiWorkbenchLayoutDockRegion)0;
}

static UmiWorkbenchLayoutVisibility visibility_from_text(const char *text)
{
    UmiWorkbenchLayoutVisibility value;
    for (value = UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE;
         value <= UMI_WORKBENCH_LAYOUT_VISIBILITY_AUTO;
         value = (UmiWorkbenchLayoutVisibility)((int)value + 1)) {
        if (strcmp(text, umi_workbench_layout_visibility_text(value)) == 0) {
            return value;
        }
    }
    return (UmiWorkbenchLayoutVisibility)0;
}

static UmiStatus parser_size_index(UmiJsonParser *parser, size_t *out_value)
{
    int64_t value;
    UmiStatus status = parser_i64(parser, &value);
    if (status != UMI_STATUS_OK) return status;
    if (value == -1) {
        *out_value = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
        return UMI_STATUS_OK;
    }
    if (value < 0 || (uint64_t)value > (uint64_t)SIZE_MAX) {
        return parser_fail(parser, "Layout index is outside the supported range.");
    }
    *out_value = (size_t)value;
    return UMI_STATUS_OK;
}

static UmiStatus parser_i32(UmiJsonParser *parser, int32_t *out_value)
{
    int64_t value;
    UmiStatus status = parser_i64(parser, &value);
    if (status != UMI_STATUS_OK) return status;
    if (value < INT32_MIN || value > INT32_MAX) {
        return parser_fail(parser, "Integer is outside the 32-bit range.");
    }
    *out_value = (int32_t)value;
    return UMI_STATUS_OK;
}

static UmiStatus parser_u32(UmiJsonParser *parser, uint32_t *out_value)
{
    uint64_t value;
    UmiStatus status = parser_u64(parser, &value);
    if (status != UMI_STATUS_OK) return status;
    if (value > UINT32_MAX) {
        return parser_fail(parser, "Integer is outside the 32-bit range.");
    }
    *out_value = (uint32_t)value;
    return UMI_STATUS_OK;
}

static UmiStatus parser_rect(
    UmiJsonParser *parser,
    UmiWorkbenchLayoutRect *rect)
{
    char key[UMI_JSON_KEY_CAPACITY];
    UmiStatus status;

    if (!parser_consume(parser, '{')) {
        return parser_fail(parser, "Expected a rectangle object.");
    }
    if (parser_consume(parser, '}')) return UMI_STATUS_OK;
    for (;;) {
        status = parser_string(parser, key, sizeof(key));
        if (status != UMI_STATUS_OK) return status;
        if (!parser_consume(parser, ':')) {
            return parser_fail(parser, "Expected ':' in rectangle object.");
        }
        if (strcmp(key, "x") == 0) status = parser_i32(parser, &rect->x);
        else if (strcmp(key, "y") == 0) status = parser_i32(parser, &rect->y);
        else if (strcmp(key, "width") == 0) {
            status = parser_i32(parser, &rect->width);
        } else if (strcmp(key, "height") == 0) {
            status = parser_i32(parser, &rect->height);
        } else status = parser_skip_value(parser);
        if (status != UMI_STATUS_OK) return status;
        if (parser_consume(parser, '}')) return UMI_STATUS_OK;
        if (!parser_consume(parser, ',')) {
            return parser_fail(parser, "Expected ',' in rectangle object.");
        }
    }
}

static UmiStatus parser_size(
    UmiJsonParser *parser,
    UmiWorkbenchLayoutSize *size)
{
    char key[UMI_JSON_KEY_CAPACITY];
    UmiStatus status;

    if (!parser_consume(parser, '{')) {
        return parser_fail(parser, "Expected a size object.");
    }
    if (parser_consume(parser, '}')) return UMI_STATUS_OK;
    for (;;) {
        status = parser_string(parser, key, sizeof(key));
        if (status != UMI_STATUS_OK) return status;
        if (!parser_consume(parser, ':')) {
            return parser_fail(parser, "Expected ':' in size object.");
        }
        if (strcmp(key, "width") == 0) {
            status = parser_i32(parser, &size->width);
        } else if (strcmp(key, "height") == 0) {
            status = parser_i32(parser, &size->height);
        } else status = parser_skip_value(parser);
        if (status != UMI_STATUS_OK) return status;
        if (parser_consume(parser, '}')) return UMI_STATUS_OK;
        if (!parser_consume(parser, ',')) {
            return parser_fail(parser, "Expected ',' in size object.");
        }
    }
}

static UmiStatus parser_identity(
    UmiJsonParser *parser,
    UmiWorkbenchLayoutIdentity *identity)
{
    char key[UMI_JSON_KEY_CAPACITY];
    UmiStatus status;
    if (!parser_consume(parser, '{')) {
        return parser_fail(parser, "Expected an identity object.");
    }
    if (parser_consume(parser, '}')) return UMI_STATUS_OK;
    for (;;) {
        status = parser_string(parser, key, sizeof(key));
        if (status != UMI_STATUS_OK) return status;
        if (!parser_consume(parser, ':')) {
            return parser_fail(parser, "Expected ':' in identity object.");
        }
        if (strcmp(key, "layout_id") == 0) {
            status = parser_string(
                parser, identity->layout_id, sizeof(identity->layout_id));
        } else if (strcmp(key, "owner_user_id") == 0) {
            status = parser_string(
                parser,
                identity->owner_user_id,
                sizeof(identity->owner_user_id));
        } else if (strcmp(key, "owner_application_id") == 0) {
            status = parser_string(
                parser,
                identity->owner_application_id,
                sizeof(identity->owner_application_id));
        } else if (strcmp(key, "workspace_id") == 0) {
            status = parser_string(
                parser,
                identity->workspace_id,
                sizeof(identity->workspace_id));
        } else status = parser_skip_value(parser);
        if (status != UMI_STATUS_OK) return status;
        if (parser_consume(parser, '}')) return UMI_STATUS_OK;
        if (!parser_consume(parser, ',')) {
            return parser_fail(parser, "Expected ',' in identity object.");
        }
    }
}

static UmiStatus parser_version(
    UmiJsonParser *parser,
    UmiWorkbenchLayoutVersion *version)
{
    char key[UMI_JSON_KEY_CAPACITY];
    UmiStatus status;
    if (!parser_consume(parser, '{')) {
        return parser_fail(parser, "Expected a version object.");
    }
    if (parser_consume(parser, '}')) return UMI_STATUS_OK;
    for (;;) {
        status = parser_string(parser, key, sizeof(key));
        if (status != UMI_STATUS_OK) return status;
        if (!parser_consume(parser, ':')) {
            return parser_fail(parser, "Expected ':' in version object.");
        }
        if (strcmp(key, "schema_version") == 0) {
            status = parser_u32(parser, &version->schema_version);
        } else if (strcmp(key, "revision") == 0) {
            status = parser_u64(parser, &version->revision);
        } else if (strcmp(key, "generation") == 0) {
            status = parser_u64(parser, &version->generation);
        } else if (strcmp(key, "base_revision") == 0) {
            status = parser_u64(parser, &version->base_revision);
        } else status = parser_skip_value(parser);
        if (status != UMI_STATUS_OK) return status;
        if (parser_consume(parser, '}')) return UMI_STATUS_OK;
        if (!parser_consume(parser, ',')) {
            return parser_fail(parser, "Expected ',' in version object.");
        }
    }
}

static UmiStatus parser_audit(
    UmiJsonParser *parser,
    UmiWorkbenchLayoutAudit *audit)
{
    char key[UMI_JSON_KEY_CAPACITY];
    UmiStatus status;
    if (!parser_consume(parser, '{')) {
        return parser_fail(parser, "Expected an audit object.");
    }
    if (parser_consume(parser, '}')) return UMI_STATUS_OK;
    for (;;) {
        status = parser_string(parser, key, sizeof(key));
        if (status != UMI_STATUS_OK) return status;
        if (!parser_consume(parser, ':')) {
            return parser_fail(parser, "Expected ':' in audit object.");
        }
        if (strcmp(key, "created_by") == 0) {
            status = parser_string(
                parser, audit->created_by, sizeof(audit->created_by));
        } else if (strcmp(key, "modified_by") == 0) {
            status = parser_string(
                parser, audit->modified_by, sizeof(audit->modified_by));
        } else if (strcmp(key, "created_at_ms") == 0) {
            status = parser_u64(parser, &audit->created_at_ms);
        } else if (strcmp(key, "modified_at_ms") == 0) {
            status = parser_u64(parser, &audit->modified_at_ms);
        } else if (strcmp(key, "correlation_id") == 0) {
            status = parser_string(
                parser,
                audit->correlation_id,
                sizeof(audit->correlation_id));
        } else status = parser_skip_value(parser);
        if (status != UMI_STATUS_OK) return status;
        if (parser_consume(parser, '}')) return UMI_STATUS_OK;
        if (!parser_consume(parser, ',')) {
            return parser_fail(parser, "Expected ',' in audit object.");
        }
    }
}

static UmiStatus parser_tags(
    UmiJsonParser *parser,
    UmiWorkbenchLayoutDocument *document)
{
    UmiStatus status;
    if (!parser_consume(parser, '[')) {
        return parser_fail(parser, "Expected a tag array.");
    }
    if (parser_consume(parser, ']')) return UMI_STATUS_OK;
    for (;;) {
        if (document->tag_count >= UMI_WORKBENCH_LAYOUT_MAX_TAGS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = parser_string(
            parser,
            document->tags[document->tag_count].value,
            sizeof(document->tags[document->tag_count].value));
        if (status != UMI_STATUS_OK) return status;
        document->tag_count += 1U;
        if (parser_consume(parser, ']')) return UMI_STATUS_OK;
        if (!parser_consume(parser, ',')) {
            return parser_fail(parser, "Expected ',' between layout tags.");
        }
    }
}

static UmiStatus parser_child_indices(
    UmiJsonParser *parser,
    UmiWorkbenchLayoutNode *node)
{
    UmiStatus status;
    if (!parser_consume(parser, '[')) {
        return parser_fail(parser, "Expected a child-index array.");
    }
    if (parser_consume(parser, ']')) return UMI_STATUS_OK;
    for (;;) {
        if (node->child_count >= UMI_WORKBENCH_LAYOUT_MAX_CHILDREN) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = parser_size_index(
            parser, &node->child_indices[node->child_count]);
        if (status != UMI_STATUS_OK) return status;
        node->child_count += 1U;
        if (parser_consume(parser, ']')) return UMI_STATUS_OK;
        if (!parser_consume(parser, ',')) {
            return parser_fail(parser, "Expected ',' between child indices.");
        }
    }
}

static UmiStatus parser_node(
    UmiJsonParser *parser,
    UmiWorkbenchLayoutNode *node)
{
    char key[UMI_JSON_KEY_CAPACITY];
    char text[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    UmiStatus status;

    umi_workbench_layout_node_init(
        node, "", UMI_WORKBENCH_LAYOUT_NODE_EMPTY);
    if (!parser_consume(parser, '{')) {
        return parser_fail(parser, "Expected a semantic layout node object.");
    }
    if (parser_consume(parser, '}')) {
        return parser_fail(parser, "A semantic layout node cannot be empty.");
    }
    for (;;) {
        status = parser_string(parser, key, sizeof(key));
        if (status != UMI_STATUS_OK) return status;
        if (!parser_consume(parser, ':')) {
            return parser_fail(parser, "Expected ':' in layout node object.");
        }
        if (strcmp(key, "node_id") == 0) {
            status = parser_string(
                parser, node->node_id, sizeof(node->node_id));
        } else if (strcmp(key, "title") == 0) {
            status = parser_string(parser, node->title, sizeof(node->title));
        } else if (strcmp(key, "component_id") == 0) {
            status = parser_string(
                parser, node->component_id, sizeof(node->component_id));
        } else if (strcmp(key, "owner_application_id") == 0) {
            status = parser_string(
                parser,
                node->owner_application_id,
                sizeof(node->owner_application_id));
        } else if (strcmp(key, "context_group_id") == 0) {
            status = parser_string(
                parser,
                node->context_group_id,
                sizeof(node->context_group_id));
        } else if (strcmp(key, "monitor_id") == 0) {
            status = parser_string(
                parser, node->monitor_id, sizeof(node->monitor_id));
        } else if (strcmp(key, "kind") == 0) {
            status = parser_string(parser, text, sizeof(text));
            if (status == UMI_STATUS_OK) {
                node->kind = node_kind_from_text(text);
                if (node->kind == 0) {
                    status = parser_fail(parser, "Unknown layout node kind.");
                }
            }
        } else if (strcmp(key, "orientation") == 0) {
            status = parser_string(parser, text, sizeof(text));
            if (status == UMI_STATUS_OK) {
                node->orientation = orientation_from_text(text);
                if ((int)node->orientation < 0) {
                    status = parser_fail(
                        parser, "Unknown layout orientation.");
                }
            }
        } else if (strcmp(key, "dock_region") == 0) {
            status = parser_string(parser, text, sizeof(text));
            if (status == UMI_STATUS_OK) {
                node->dock_region = dock_region_from_text(text);
                if (node->dock_region == 0) {
                    status = parser_fail(parser, "Unknown dock region.");
                }
            }
        } else if (strcmp(key, "visibility") == 0) {
            status = parser_string(parser, text, sizeof(text));
            if (status == UMI_STATUS_OK) {
                node->visibility = visibility_from_text(text);
                if (node->visibility == 0) {
                    status = parser_fail(parser, "Unknown visibility value.");
                }
            }
        } else if (strcmp(key, "split_ratio") == 0) {
            status = parser_double(parser, &node->split_ratio);
        } else if (strcmp(key, "bounds") == 0) {
            status = parser_rect(parser, &node->bounds);
        } else if (strcmp(key, "minimum_size") == 0) {
            status = parser_size(parser, &node->minimum_size);
        } else if (strcmp(key, "preferred_size") == 0) {
            status = parser_size(parser, &node->preferred_size);
        } else if (strcmp(key, "parent_index") == 0) {
            status = parser_size_index(parser, &node->parent_index);
        } else if (strcmp(key, "children") == 0) {
            node->child_count = 0U;
            status = parser_child_indices(parser, node);
        } else if (strcmp(key, "active_child_index") == 0) {
            status = parser_size_index(parser, &node->active_child_index);
        } else if (strcmp(key, "flags") == 0) {
            status = parser_u32(parser, &node->flags);
        } else if (strcmp(key, "order") == 0) {
            status = parser_i32(parser, &node->order);
        } else if (strcmp(key, "z_order") == 0) {
            status = parser_i32(parser, &node->z_order);
        } else if (strcmp(key, "revision") == 0) {
            status = parser_u64(parser, &node->revision);
        } else {
            status = parser_skip_value(parser);
        }
        if (status != UMI_STATUS_OK) return status;
        if (parser_consume(parser, '}')) break;
        if (!parser_consume(parser, ',')) {
            return parser_fail(parser, "Expected ',' in layout node object.");
        }
    }
    node->structure_size = sizeof(*node);
    return UMI_STATUS_OK;
}

static UmiStatus parser_nodes(
    UmiJsonParser *parser,
    UmiWorkbenchLayoutDocument *document)
{
    UmiStatus status;
    if (!parser_consume(parser, '[')) {
        return parser_fail(parser, "Expected a semantic layout node array.");
    }
    if (parser_consume(parser, ']')) return UMI_STATUS_OK;
    for (;;) {
        if (document->node_count >= UMI_WORKBENCH_LAYOUT_MAX_NODES) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = parser_node(
            parser, &document->nodes[document->node_count]);
        if (status != UMI_STATUS_OK) return status;
        document->node_count += 1U;
        if (parser_consume(parser, ']')) return UMI_STATUS_OK;
        if (!parser_consume(parser, ',')) {
            return parser_fail(parser, "Expected ',' between layout nodes.");
        }
    }
}

static UmiStatus parser_layout(
    UmiJsonParser *parser,
    UmiWorkbenchLayoutDocument *document)
{
    char key[UMI_JSON_KEY_CAPACITY];
    UmiStatus status;

    if (!parser_consume(parser, '{')) {
        return parser_fail(parser, "Expected a layout object.");
    }
    if (parser_consume(parser, '}')) {
        return parser_fail(parser, "The layout object cannot be empty.");
    }
    for (;;) {
        status = parser_string(parser, key, sizeof(key));
        if (status != UMI_STATUS_OK) return status;
        if (!parser_consume(parser, ':')) {
            return parser_fail(parser, "Expected ':' in layout object.");
        }
        if (strcmp(key, "identity") == 0) {
            status = parser_identity(parser, &document->identity);
        } else if (strcmp(key, "version") == 0) {
            status = parser_version(parser, &document->version);
        } else if (strcmp(key, "audit") == 0) {
            status = parser_audit(parser, &document->audit);
        } else if (strcmp(key, "name") == 0) {
            status = parser_string(
                parser, document->name, sizeof(document->name));
        } else if (strcmp(key, "category") == 0) {
            status = parser_string(
                parser, document->category, sizeof(document->category));
        } else if (strcmp(key, "description") == 0) {
            status = parser_string(
                parser,
                document->description,
                sizeof(document->description));
        } else if (strcmp(key, "tags") == 0) {
            document->tag_count = 0U;
            status = parser_tags(parser, document);
        } else if (strcmp(key, "root_index") == 0) {
            status = parser_size_index(parser, &document->root_index);
        } else if (strcmp(key, "flags") == 0) {
            status = parser_u32(parser, &document->flags);
        } else if (strcmp(key, "content_hash") == 0) {
            status = parser_u64(parser, &document->content_hash);
        } else if (strcmp(key, "nodes") == 0) {
            document->node_count = 0U;
            status = parser_nodes(parser, document);
        } else {
            status = parser_skip_value(parser);
        }
        if (status != UMI_STATUS_OK) return status;
        if (parser_consume(parser, '}')) return UMI_STATUS_OK;
        if (!parser_consume(parser, ',')) {
            return parser_fail(parser, "Expected ',' in layout object.");
        }
    }
}

static UmiStatus parser_root(
    UmiJsonParser *parser,
    UmiWorkbenchLayoutDocument *document)
{
    char key[UMI_JSON_KEY_CAPACITY];
    char schema[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    UmiStatus status;
    bool layout_seen = false;

    schema[0] = '\0';
    if (!parser_consume(parser, '{')) {
        return parser_fail(parser, "Expected a JSON document object.");
    }
    if (parser_consume(parser, '}')) {
        return parser_fail(parser, "The JSON document cannot be empty.");
    }
    for (;;) {
        status = parser_string(parser, key, sizeof(key));
        if (status != UMI_STATUS_OK) return status;
        if (!parser_consume(parser, ':')) {
            return parser_fail(parser, "Expected ':' in JSON document.");
        }
        if (strcmp(key, "schema") == 0) {
            status = parser_string(parser, schema, sizeof(schema));
        } else if (strcmp(key, "layout") == 0) {
            status = parser_layout(parser, document);
            layout_seen = status == UMI_STATUS_OK;
        } else {
            status = parser_skip_value(parser);
        }
        if (status != UMI_STATUS_OK) return status;
        if (parser_consume(parser, '}')) break;
        if (!parser_consume(parser, ',')) {
            return parser_fail(parser, "Expected ',' in JSON document.");
        }
    }
    parser_skip_space(parser);
    if (parser->offset != parser->length) {
        return parser_fail(parser, "Unexpected data after the JSON document.");
    }
    if (!layout_seen) {
        return parser_fail(parser, "The JSON document does not contain layout data.");
    }
    if (schema[0] != '\0' &&
        strcmp(schema, "umicom.workbench-layout/2") != 0 &&
        strcmp(schema, "umicom.workbench-layout/1") != 0) {
        return parser_fail(parser, "Unsupported workbench layout schema identity.");
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_json_decode(
    const char *json,
    size_t length,
    UmiWorkbenchLayoutDocument *out_document,
    UmiWorkbenchLayoutJsonResult *out_result)
{
    UmiJsonParser parser;
    UmiWorkbenchLayoutMigrationReport migration;
    UmiStatus status;

    json_result_init(out_result);
    if (json == NULL || out_document == NULL) {
        return json_result_finish(
            out_result,
            UMI_STATUS_INVALID_ARGUMENT,
            0U,
            0U,
            0U,
            "JSON input and a destination document are required.");
    }
    if (length == 0U) length = strlen(json);
    (void)memset(&parser, 0, sizeof(parser));
    parser.text = json;
    parser.length = length;

    umi_workbench_layout_document_init(out_document, "", "");
    status = parser_root(&parser, out_document);
    if (status != UMI_STATUS_OK) {
        return json_result_finish(
            out_result,
            status,
            0U,
            length,
            parser.error_offset,
            parser.message);
    }
    out_document->structure_size = sizeof(*out_document);
    if (out_document->version.schema_version == 0U) {
        out_document->version.schema_version =
            UMI_WORKBENCH_LAYOUT_SCHEMA_VERSION;
    }
    if (out_document->version.revision == 0U) {
        out_document->version.revision = 1U;
    }
    if (out_document->version.generation == 0U) {
        out_document->version.generation = 1U;
    }

    if (out_document->version.schema_version !=
        UMI_WORKBENCH_LAYOUT_SCHEMA_VERSION) {
        status = umi_workbench_layout_migrate_in_place(
            out_document,
            UMI_WORKBENCH_LAYOUT_SCHEMA_VERSION,
            &migration);
        if (status != UMI_STATUS_OK) {
            return json_result_finish(
                out_result,
                status,
                0U,
                length,
                parser.offset,
                migration.message);
        }
    }
    status = umi_workbench_layout_document_validate_structure(out_document);
    if (status != UMI_STATUS_OK) {
        return json_result_finish(
            out_result,
            status,
            0U,
            length,
            parser.offset,
            "Decoded JSON does not describe a structurally valid layout.");
    }
    if (out_document->content_hash == 0U) {
        umi_workbench_layout_document_refresh_hash(out_document);
    }
    return json_result_finish(
        out_result,
        UMI_STATUS_OK,
        length,
        length,
        0U,
        "The layout was decoded successfully.");
}

UmiStatus umi_workbench_layout_json_validate(
    const char *json,
    size_t length,
    UmiWorkbenchLayoutValidationReport *out_report,
    UmiWorkbenchLayoutJsonResult *out_result)
{
    UmiWorkbenchLayoutDocument *document;
    UmiWorkbenchLayoutValidationOptions options;
    UmiStatus status;

    if (out_report == NULL) {
        return json_result_finish(
            out_result,
            UMI_STATUS_INVALID_ARGUMENT,
            0U,
            0U,
            0U,
            "A validation report destination is required.");
    }
    document = (UmiWorkbenchLayoutDocument *)calloc(1U, sizeof(*document));
    if (document == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_workbench_layout_json_decode(
        json, length, document, out_result);
    if (status == UMI_STATUS_OK) {
        options = umi_workbench_layout_validation_options_default();
        options.validate_hash = true;
        status = umi_workbench_layout_validate(
            document, &options, out_report);
        if (status == UMI_STATUS_OK && !out_report->valid) {
            status = UMI_STATUS_INVALID_STATE;
        }
    } else {
        umi_workbench_layout_validation_report_init(out_report);
    }
    free(document);
    return status;
}

UmiStatus umi_workbench_layout_json_escape(
    const char *text,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    UmiJsonWriter writer;
    if (text == NULL || out_required == NULL ||
        (buffer == NULL && capacity != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&writer, 0, sizeof(writer));
    writer.buffer = buffer;
    writer.capacity = capacity;
    if (buffer != NULL && capacity > 0U) buffer[0] = '\0';
    writer_string(&writer, text);
    *out_required = writer.required + 1U;
    return writer.overflow
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_json_unescape(
    const char *text,
    size_t length,
    char *buffer,
    size_t capacity,
    size_t *out_required)
{
    UmiJsonParser parser;
    UmiStatus status;
    char *quoted;

    if (text == NULL || out_required == NULL ||
        (buffer == NULL && capacity != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length == 0U) length = strlen(text);

    if (length >= 2U && text[0] == '\"' && text[length - 1U] == '\"') {
        quoted = NULL;
        (void)memset(&parser, 0, sizeof(parser));
        parser.text = text;
        parser.length = length;
    } else {
        quoted = (char *)malloc(length + 3U);
        if (quoted == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        quoted[0] = '\"';
        (void)memcpy(quoted + 1U, text, length);
        quoted[length + 1U] = '\"';
        quoted[length + 2U] = '\0';
        (void)memset(&parser, 0, sizeof(parser));
        parser.text = quoted;
        parser.length = length + 2U;
    }

    if (buffer != NULL && capacity > 0U) {
        status = parser_string(&parser, buffer, capacity);
        if (status == UMI_STATUS_OK) *out_required = strlen(buffer) + 1U;
    } else {
        size_t temporary_capacity = length * 4U + 1U;
        char *temporary = (char *)malloc(temporary_capacity);
        if (temporary == NULL) {
            free(quoted);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        status = parser_string(&parser, temporary, temporary_capacity);
        if (status == UMI_STATUS_OK) *out_required = strlen(temporary) + 1U;
        free(temporary);
    }
    free(quoted);
    return status;
}
