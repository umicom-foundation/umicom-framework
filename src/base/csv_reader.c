/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/csv_reader.c
 * PURPOSE: Decode complete CSV records with bounded ownership and explicit terminal errors.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/csv_reader.h"
#include "csv_text_private.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct UmiCsvReader {
    char *source, *fields;
    size_t length, cursor, record;
    UmiStatus failure;
};
/* Source and field storage are separate: quote decoding cannot overwrite bytes
 * that are still waiting to be read. Whole-input limits bound both allocations. */
UmiStatus UmiCsvReaderCreate(const void *bytes, size_t length, UmiCsvReader **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (bytes == NULL && length != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_CSV_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!CsvTextValid(bytes, length, UMI_CSV_MAX_BYTES)) return UMI_STATUS_PARSE_ERROR;
    UmiCsvReader *reader = calloc(1U, sizeof *reader);
    if (reader == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    reader->source = malloc(length + 1U);
    reader->fields = malloc(length + 1U);
    if (reader->source == NULL || reader->fields == NULL) {
        UmiCsvReaderDestroy(reader);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    if (length != 0U) memcpy(reader->source, bytes, length);
    reader->source[length] = '\0';
    reader->length = length;
    reader->failure = UMI_STATUS_OK;
    if (length >= 3U && memcmp(reader->source, "\xef\xbb\xbf", 3U) == 0)
        reader->cursor = 3U;
    *out = reader;
    return UMI_STATUS_OK;
}
void UmiCsvReaderDestroy(UmiCsvReader *reader)
{
    if (reader != NULL) {
        free(reader->source);
        free(reader->fields);
        free(reader);
    }
}
/* A malformed record stops the stream. Continuing after a guessed boundary could
 * turn part of a quoted description into an apparently valid transaction. */
static UmiStatus CsvReaderFail(UmiCsvReader *reader, UmiStatus status)
{
    reader->failure = status;
    return status;
}
UmiStatus UmiCsvReaderNext(UmiCsvReader *reader, UmiCsvReadRow *out)
{
    if (reader == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (reader->failure != UMI_STATUS_OK) return reader->failure;
    if (reader->cursor == reader->length) return UMI_STATUS_NOT_FOUND;
    UmiCsvReadRow row = {0};
    size_t cursor = reader->cursor, used = 0U;
    for (;;) {
        if (row.count == UMI_CSV_MAX_COLUMNS)
            return CsvReaderFail(reader, UMI_STATUS_CAPACITY_EXCEEDED);
        size_t start = used;
        bool quoted = cursor < reader->length && reader->source[cursor] == '"';
        bool closed = !quoted;
        if (quoted) ++cursor;
        while (cursor < reader->length) {
            char ch = reader->source[cursor];
            if (quoted) {
                if (ch == '"') {
                    ++cursor;
                    if (cursor < reader->length && reader->source[cursor] == '"') ++cursor;
                    else { closed = true; break; }
                } else ++cursor;
            } else {
                if (ch == ',' || ch == '\r' || ch == '\n') break;
                if (ch == '"') return CsvReaderFail(reader, UMI_STATUS_PARSE_ERROR);
                ++cursor;
            }
            /* CR must belong to a CRLF pair, including inside a quoted field.
             * Preserve both bytes so decoding does not rewrite descriptions. */
            if (ch == '\r' && (cursor == reader->length || reader->source[cursor] != '\n'))
                return CsvReaderFail(reader, UMI_STATUS_PARSE_ERROR);
            if (used - start == UMI_CSV_MAX_TEXT_BYTES)
                return CsvReaderFail(reader, UMI_STATUS_CAPACITY_EXCEEDED);
            reader->fields[used++] = ch;
        }
        if (!closed) return CsvReaderFail(reader, UMI_STATUS_PARSE_ERROR);
        row.fields[row.count] = reader->fields + start;
        row.lengths[row.count++] = used - start;
        reader->fields[used++] = '\0';
        /* Every separator provides storage for the decoded field terminator;
         * the extra allocated byte covers an unterminated last input record. */
        if (cursor == reader->length) break;
        char separator = reader->source[cursor++];
        if (separator == ',') continue;
        if (separator == '\n') break;
        if (separator == '\r' && cursor < reader->length && reader->source[cursor] == '\n') {
            ++cursor;
            break;
        }
        return CsvReaderFail(reader, UMI_STATUS_PARSE_ERROR);
    }
    reader->cursor = cursor;
    row.record = ++reader->record;
    *out = row;
    return UMI_STATUS_OK;
}
