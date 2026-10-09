/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/compiler_stream.c
 * PURPOSE: Keep compiler diagnostics independent of bounded console previews.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/diagnostics/compiler_stream.h"
#include "compiler_parser_internal.h"
#include <stdlib.h>
#include <string.h>

struct UmiCompilerDiagnosticStream
{
    UmiCompilerDiagnosticObserver observer;
    void *context;
    char line[8192];
    size_t line_bytes;
    char record[32768];
    size_t record_bytes;
    UmiStatus line_status;
    UmiStatus record_status;
    int pending_cmake;
    int finished;
};

/* Report a complete record once. ParseBlock remains the authority for message,
 * path and severity; streaming supplies boundaries and owns temporary bytes. */
static void StreamEmit(UmiCompilerDiagnosticStream *stream)
{
    if (!stream->pending_cmake)
        return;
    UmiCompilerDiagnosticFields fields;
    size_t consumed = 0U;
    UmiStatus status = stream->record_status;
    if (status == UMI_STATUS_OK)
        status = UmiCompilerDiagnosticParseBlock(stream->record, &fields, &consumed);
    if (status == UMI_STATUS_OK)
        stream->observer(status, &fields, stream->context);
    else if (status != UMI_STATUS_NOT_FOUND)
        stream->observer(status, NULL, stream->context);
    stream->pending_cmake = 0;
    stream->record_bytes = 0U;
    stream->record[0] = '\0';
    stream->record_status = UMI_STATUS_OK;
}

/* Once a logical record exceeds a bound, discard its remaining continuations
 * until the next boundary. A shortened error would conceal the lost evidence. */
static void StreamAppendRecord(UmiCompilerDiagnosticStream *stream)
{
    if (stream->record_status != UMI_STATUS_OK)
        return;
    if (stream->line_status != UMI_STATUS_OK)
    {
        stream->record_status = stream->line_status;
        return;
    }
    size_t available = sizeof stream->record - 1U - stream->record_bytes;
    if (stream->line_bytes >= available)
    {
        stream->record_status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    memcpy(stream->record + stream->record_bytes, stream->line, stream->line_bytes);
    stream->record_bytes += stream->line_bytes;
    stream->record[stream->record_bytes++] = '\n';
    stream->record[stream->record_bytes] = '\0';
}

/* Newlines, not read() sizes, delimit producer lines. A CMake paragraph may
 * span many reads; a following ordinary line must still be considered anew. */
static void StreamLine(UmiCompilerDiagnosticStream *stream)
{
    stream->line[stream->line_bytes] = '\0';
    UmiCompilerLineKind kind = stream->line_status == UMI_STATUS_OK
                                   ? UmiCompilerClassifyLine(stream->line)
                                   : UMI_COMPILER_LINE_OTHER;
    int damaged_indented = stream->line_status != UMI_STATUS_OK && stream->line_bytes != 0U &&
                           (stream->line[0] == ' ' || stream->line[0] == '\t');
    if (stream->pending_cmake &&
        (kind == UMI_COMPILER_LINE_BLANK || kind == UMI_COMPILER_LINE_INDENTED || damaged_indented))
    {
        StreamAppendRecord(stream);
    }
    else
    {
        StreamEmit(stream);
        if (stream->line_status != UMI_STATUS_OK)
        {
            stream->observer(stream->line_status, NULL, stream->context);
        }
        else if (kind == UMI_COMPILER_LINE_CMAKE)
        {
            stream->pending_cmake = 1;
            StreamAppendRecord(stream);
        }
        else
        {
            UmiCompilerDiagnosticFields fields;
            UmiStatus status = UmiCompilerDiagnosticParseText(stream->line, &fields);
            if (status == UMI_STATUS_OK)
                stream->observer(status, &fields, stream->context);
            else if (status == UMI_STATUS_CAPACITY_EXCEEDED)
                stream->observer(status, NULL, stream->context);
        }
    }
    stream->line_bytes = 0U;
    stream->line[0] = '\0';
    stream->line_status = UMI_STATUS_OK;
}

UmiStatus UmiCompilerDiagnosticStreamCreate(UmiCompilerDiagnosticObserver observer, void *context,
                                            UmiCompilerDiagnosticStream **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (observer == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCompilerDiagnosticStream *stream = calloc(1U, sizeof *stream);
    if (stream == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    stream->observer = observer;
    stream->context = context;
    stream->line_status = UMI_STATUS_OK;
    stream->record_status = UMI_STATUS_OK;
    *out = stream;
    return UMI_STATUS_OK;
}

UmiStatus UmiCompilerDiagnosticStreamFeed(UmiCompilerDiagnosticStream *stream, const char *bytes,
                                          size_t length)
{
    if (stream == NULL || (bytes == NULL && length != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (stream->finished)
        return UMI_STATUS_INVALID_STATE;
    /* Drop an entire malformed physical line; never accept an embedded NUL as
     * a terminator and then treat its suffix as trustworthy source navigation. */
    for (size_t index = 0U; index < length; ++index)
    {
        unsigned char byte = (unsigned char)bytes[index];
        if (byte == '\n')
        {
            StreamLine(stream);
            continue;
        }
        if (stream->line_status != UMI_STATUS_OK)
            continue;
        if (byte == 0U)
        {
            stream->line_status = UMI_STATUS_INVALID_ARGUMENT;
        }
        else if (stream->line_bytes == sizeof stream->line - 1U)
        {
            stream->line_status = UMI_STATUS_CAPACITY_EXCEEDED;
        }
        else
        {
            stream->line[stream->line_bytes++] = (char)byte;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiCompilerDiagnosticStreamFinish(UmiCompilerDiagnosticStream *stream)
{
    if (stream == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (stream->finished)
        return UMI_STATUS_OK;
    if (stream->line_bytes != 0U || stream->line_status != UMI_STATUS_OK)
        StreamLine(stream);
    StreamEmit(stream);
    stream->finished = 1;
    return UMI_STATUS_OK;
}

void UmiCompilerDiagnosticStreamDestroy(UmiCompilerDiagnosticStream *stream) { free(stream); }
