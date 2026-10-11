/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/instruction_fields.c
 * PURPOSE: Validate instruction responses and selected-frame references before publishing bounded captures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "instruction_fields_internal.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
/* Distinguish omitted coordinates from zero and reject lossy numeric conversions. */
/* The shared response-field reader now owns numeric validation for stack and instruction records. This earlier local implementation is retained for review. The earlier implementation is retained below for engineering review. */
#if 0
static UmiStatus InstructionNumber(const UmiLanguageRuntimeJsonDocument *document, int object,
                                   const char *name, int required, uint64_t limit, uint64_t *out,
                                   int *known)
{
    int token = DebugResponseJsonField(document, object, name);
    *known = token >= 0;
    *out = 0U;
    if (token < 0)
        return required ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    int64_t value = 0;
    UmiStatus status = umi_language_runtime_json_int64(document, token, &value);
    if (status != UMI_STATUS_OK)
        return status;
    if (value < 0 || (uint64_t)value > limit)
        return UMI_STATUS_PARSE_ERROR;
    *out = (uint64_t)value;
    return UMI_STATUS_OK;
}
#endif
static UmiStatus InstructionNumber(const UmiLanguageRuntimeJsonDocument *document, int object,
                                   const char *name, int required, uint64_t limit, uint64_t *out,
                                   int *known)
{
    /* Stack and instruction records must agree about exact protocol numbers. */
    return DebugResponseJsonUnsigned(document, object, name, required, limit, out, known);
}
static int InstructionAddress(const char *text)
{
    /* Keep address spelling and width intact. Only validate decimal/hex digits;
     * converting through a host pointer would misrepresent another architecture. */
    size_t i = 0U;
    int hex = 0;
    if (text[0] == '0' && (text[1] == 'x' || text[1] == 'X'))
    {
        i = 2U;
        hex = 1;
    }
    if (text[i] == '\0')
        return 0;
    for (; text[i] != '\0'; ++i)
    {
        char c = text[i];
        if ((c >= '0' && c <= '9') || (hex && ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))))
            continue;
        return 0;
    }
    return 1;
}
/* A source location is copied metadata; it grants no filesystem access. */
/* Source metadata validation now belongs to the common response-field helper. The preceding local implementation remains for review. The earlier implementation is retained below for engineering review. */
#if 0
static UmiStatus InstructionSource(const UmiLanguageRuntimeJsonDocument *document, int object,
                                   UmiDebugRuntimeSource *out)
{
    UmiStatus status = DebugResponseJsonObject(document, object);
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonString(document, object, "name", 0, out->name, sizeof out->name);
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonString(document, object, "path", 0, out->path, sizeof out->path);
    int known = 0;
    if (status == UMI_STATUS_OK)
        status = InstructionNumber(document, object, "sourceReference", 0, INT32_MAX,
                                   &out->source_reference, &known);
    return status;
}
#endif
static UmiStatus InstructionSource(const UmiLanguageRuntimeJsonDocument *document, int object,
                                   UmiDebugRuntimeSource *out)
{
    /* A location has the same validation whether it arrives with a frame or an instruction. */
    return DebugResponseJsonSource(document, object, out);
}
/* Stage each row completely, inheriting only location metadata from the prior row. */
static UmiStatus InstructionRead(const UmiLanguageRuntimeJsonDocument *document, int object,
                                 const UmiDebugInstructionRecord *previous,
                                 UmiDebugInstructionRecord *out)
{
    UmiStatus status = DebugResponseJsonObject(document, object);
#define INSTRUCTION_TEXT(key, field, required)                                                     \
    if (status == UMI_STATUS_OK)                                                                   \
    status = DebugResponseJsonString(document, object, key, required, out->field, sizeof out->field)
    INSTRUCTION_TEXT("address", address, 1);
    INSTRUCTION_TEXT("instruction", text, 1);
    INSTRUCTION_TEXT("instructionBytes", bytes, 0);
    INSTRUCTION_TEXT("symbol", symbol, 0);
    INSTRUCTION_TEXT("presentationHint", presentation_hint, 0);
#undef INSTRUCTION_TEXT
    if (status == UMI_STATUS_OK && !InstructionAddress(out->address))
        status = UMI_STATUS_PARSE_ERROR;
#define INSTRUCTION_NUMBER(key, field)                                                             \
    if (status == UMI_STATUS_OK)                                                                   \
    status = InstructionNumber(document, object, key, 0, UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT,       \
                               &out->field, &out->field##_known)
    INSTRUCTION_NUMBER("line", line);
    INSTRUCTION_NUMBER("column", column);
    INSTRUCTION_NUMBER("endLine", end_line);
    INSTRUCTION_NUMBER("endColumn", end_column);
#undef INSTRUCTION_NUMBER
    int source = DebugResponseJsonField(document, object, "location");
    if (status == UMI_STATUS_OK && source >= 0)
    {
        status = InstructionSource(document, source, &out->source);
        out->has_source = 1;
    }
    else if (status == UMI_STATUS_OK && previous != NULL && previous->has_source)
    {
        out->source = previous->source;
        out->has_source = 1;
    }
    return status;
}
/* Parse into caller-provided temporary rows; the public owner decides when to publish. */
UmiStatus DebugInstructionArrayDecode(const char *json, UmiDebugInstructionRecord *rows,
                                      size_t capacity, size_t *count)
{
    if (json == NULL || rows == NULL || count == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    if (document == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_language_runtime_json_parse(json, document);
    int body = -1, array = -1;
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonResponse(document, "disassemble", 0, &body);
    if (status == UMI_STATUS_OK)
    {
        array = DebugResponseJsonField(document, body, "instructions");
        if (array < 0 || document->tokens[array].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
    }
    size_t length = 0U;
    if (status == UMI_STATUS_OK)
    {
        length = umi_language_runtime_json_array_count(document, array);
        if (length > capacity)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < length; ++i)
    {
        memset(&rows[i], 0, sizeof rows[i]);
        status = InstructionRead(document, umi_language_runtime_json_array_at(document, array, i),
                                 i ? &rows[i - 1U] : NULL, &rows[i]);
    }
    if (status == UMI_STATUS_OK)
        *count = length;
    free(document);
    return status;
}
/* Match the frame requested by position before accepting its opaque instruction reference. */
UmiStatus DebugInstructionReferenceDecode(const char *json, uint32_t frame_id, char *out,
                                          size_t capacity)
{
    if (json == NULL || out == NULL || capacity == 0U || frame_id > INT32_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    if (document == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_language_runtime_json_parse(json, document);
    int body = -1, array = -1;
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonResponse(document, "stackTrace", 0, &body);
    if (status == UMI_STATUS_OK)
    {
        array = DebugResponseJsonField(document, body, "stackFrames");
        if (array < 0 || document->tokens[array].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY ||
            umi_language_runtime_json_array_count(document, array) != 1U)
            status = UMI_STATUS_PARSE_ERROR;
    }
    int frame = -1, known = 0;
    uint64_t actual = 0U;
    char reference[128] = {0};
    if (status == UMI_STATUS_OK)
    {
        frame = umi_language_runtime_json_array_at(document, array, 0U);
        status = DebugResponseJsonObject(document, frame);
    }
    if (status == UMI_STATUS_OK)
        status = InstructionNumber(document, frame, "id", 1, INT32_MAX, &actual, &known);
    if (status == UMI_STATUS_OK && actual != frame_id)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = DebugResponseJsonString(document, frame, "instructionPointerReference", 0,
                                         reference, sizeof reference);
    if (status == UMI_STATUS_OK && reference[0] == '\0')
        status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK && strlen(reference) >= capacity)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        memcpy(out, reference, strlen(reference) + 1U);
    free(document);
    return status;
}

/* Legacy address actions need an exact 64-bit value, independent of host pointer width. */
UmiStatus DebugInstructionAddressRead(const char *text, uint64_t *out)
{
    if (text == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!InstructionAddress(text))
        return UMI_STATUS_PARSE_ERROR;
    unsigned base = text[0] == '0' && (text[1] == 'x' || text[1] == 'X') ? 16U : 10U;
    const char *cursor = text + (base == 16U ? 2U : 0U);
    uint64_t value = 0U;
    for (; *cursor != '\0'; ++cursor)
    {
        unsigned digit = *cursor >= '0' && *cursor <= '9'   ? (unsigned)(*cursor - '0')
                         : *cursor >= 'a' && *cursor <= 'f' ? (unsigned)(*cursor - 'a') + 10U
                                                            : (unsigned)(*cursor - 'A') + 10U;
        if (value > (UINT64_MAX - digit) / base)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        value = value * base + digit;
    }
    *out = value;
    return UMI_STATUS_OK;
}
