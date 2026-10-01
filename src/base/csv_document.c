/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/csv_document.c
 * PURPOSE: Own report bytes and centralise UTF-8, CSV escaping and row atomicity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/csv_document.h"
#include <float.h>
#include <inttypes.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiCsvDocument {
    char *data;
    size_t bytes, allocated, maximum, rows, columns;
};
typedef struct PreparedCell {
    const char *text;
    size_t length;
    int prefix;
    char number[128];
} PreparedCell;

UmiCsvCell UmiCsvTextSpan(const char *text, size_t length)
{ UmiCsvCell cell = {0}; cell.kind = UMI_CSV_TEXT; cell.text = text; cell.length = length; return cell; }
UmiCsvCell UmiCsvText(const char *text)
{ return UmiCsvTextSpan(text, text != NULL ? strlen(text) : 0U); }
UmiCsvCell UmiCsvSigned(int64_t value)
{ UmiCsvCell cell = {0}; cell.kind = UMI_CSV_SIGNED; cell.signedValue = value; return cell; }
UmiCsvCell UmiCsvUnsigned(uint64_t value)
{ UmiCsvCell cell = {0}; cell.kind = UMI_CSV_UNSIGNED; cell.unsignedValue = value; return cell; }
UmiCsvCell UmiCsvReal(double value)
{ UmiCsvCell cell = {0}; cell.kind = UMI_CSV_REAL; cell.realValue = value; return cell; }

static int ValidText(const char *text, size_t length)
{
    if (length > UMI_CSV_MAX_TEXT_BYTES || (text == NULL && length != 0U)) return 0;
    for (size_t i = 0U; i < length;) {
        unsigned char c = (unsigned char)text[i++];
        if (c < 0x80U) {
            if ((c < 0x20U && c != '\t' && c != '\r' && c != '\n') || c == 0x7fU) return 0;
            continue;
        }
        unsigned following;
        uint32_t value, minimum;
        if (c >= 0xc2U && c <= 0xdfU) { following = 1U; value = c & 0x1fU; minimum = 0x80U; }
        else if (c >= 0xe0U && c <= 0xefU) { following = 2U; value = c & 0x0fU; minimum = 0x800U; }
        else if (c >= 0xf0U && c <= 0xf4U) { following = 3U; value = c & 0x07U; minimum = 0x10000U; }
        else return 0;
        if (length - i < following) return 0;
        for (unsigned j = 0U; j < following; ++j) {
            c = (unsigned char)text[i++];
            if ((c & 0xc0U) != 0x80U) return 0;
            value = (value << 6U) | (c & 0x3fU);
        }
        if (value < minimum || value > 0x10ffffU || (value >= 0xd800U && value <= 0xdfffU)) return 0;
    }
    return 1;
}

static int FormulaPrefix(const char *text, size_t length)
{
    size_t i = 0U;
    int whitespaceControl = 0;
    while (i < length) {
        if (text[i] == ' ') { ++i; continue; }
        if (text[i] == '\t' || text[i] == '\r' || text[i] == '\n') {
            whitespaceControl = 1; ++i; continue;
        }
        if (length - i >= 3U && memcmp(text + i, "\xef\xbb\xbf", 3U) == 0) { i += 3U; continue; }
        break;
    }
    return whitespaceControl || (i < length && strchr("=+-@", text[i]) != NULL);
}

/* Only the numeric formatter enters this grammar. Caller text cannot choose
 * the unprefixed path merely by resembling an equation or numeric expression. */
static int NumberGrammar(const char *text)
{
    const char *p = text;
    if (*p == '-') ++p;
    if (*p < '0' || *p > '9') return 0;
    while (*p >= '0' && *p <= '9') ++p;
    if (*p == '.') {
        ++p;
        if (*p < '0' || *p > '9') return 0;
        while (*p >= '0' && *p <= '9') ++p;
    }
    if (*p == 'e' || *p == 'E') {
        ++p;
        if (*p == '+' || *p == '-') ++p;
        if (*p < '0' || *p > '9') return 0;
        while (*p >= '0' && *p <= '9') ++p;
    }
    return *p == '\0';
}

static UmiStatus Prepare(const UmiCsvCell *cell, PreparedCell *out)
{
    memset(out, 0, sizeof(*out));
    if (cell->kind == UMI_CSV_TEXT) {
        if (!ValidText(cell->text, cell->length)) return UMI_STATUS_INVALID_ARGUMENT;
        out->text = cell->text; out->length = cell->length;
        out->prefix = FormulaPrefix(cell->text, cell->length);
        return UMI_STATUS_OK;
    }
    int written;
    if (cell->kind == UMI_CSV_SIGNED)
        written = snprintf(out->number, sizeof(out->number), "%" PRId64, cell->signedValue);
    else if (cell->kind == UMI_CSV_UNSIGNED)
        written = snprintf(out->number, sizeof(out->number), "%" PRIu64, cell->unsignedValue);
    else if (cell->kind == UMI_CSV_REAL) {
        if (!isfinite(cell->realValue)) return UMI_STATUS_INVALID_ARGUMENT;
        written = snprintf(out->number, sizeof(out->number), "%.*g", DBL_DECIMAL_DIG, cell->realValue);
    } else return UMI_STATUS_INVALID_ARGUMENT;
    if (written < 0 || (size_t)written >= sizeof(out->number)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (cell->kind == UMI_CSV_REAL) {
        const struct lconv *format = localeconv();
        if (format == NULL || format->decimal_point == NULL || format->decimal_point[0] == '\0')
            return UMI_STATUS_UNAVAILABLE;
        if (strcmp(format->decimal_point, ".") != 0) {
            char *point = strstr(out->number, format->decimal_point);
            if (point != NULL) {
                size_t width = strlen(format->decimal_point);
                memmove(point + 1, point + width, strlen(point + width) + 1U);
                *point = '.';
            }
        }
    }
    if (!NumberGrammar(out->number)) return UMI_STATUS_UNAVAILABLE;
    out->text = out->number; out->length = strlen(out->number);
    return UMI_STATUS_OK;
}

UmiStatus UmiCsvDocumentCreate(size_t maxBytes, UmiCsvDocument **outDocument)
{
    if (outDocument == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outDocument = NULL;
    if (maxBytes == 0U || maxBytes > UMI_CSV_MAX_BYTES) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCsvDocument *document = calloc(1U, sizeof(*document));
    if (document == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    document->data = calloc(1U, 1U);
    if (document->data == NULL) { free(document); return UMI_STATUS_OUT_OF_MEMORY; }
    document->maximum = maxBytes; document->allocated = 1U;
    *outDocument = document;
    return UMI_STATUS_OK;
}
void UmiCsvDocumentDestroy(UmiCsvDocument *document)
{ if (document != NULL) { free(document->data); free(document); } }

UmiStatus UmiCsvDocumentAppendRow(UmiCsvDocument *document, const UmiCsvCell *cells, size_t count)
{
    if (document == NULL || cells == NULL || count == 0U || count > UMI_CSV_MAX_COLUMNS ||
        (document->columns != 0U && document->columns != count)) return UMI_STATUS_INVALID_ARGUMENT;
    PreparedCell prepared[UMI_CSV_MAX_COLUMNS];
    size_t bytes = 2U; /* CRLF; lengths are bounded before any additions. */
    for (size_t i = 0U; i < count; ++i) {
        UmiStatus status = Prepare(&cells[i], &prepared[i]);
        if (status != UMI_STATUS_OK) return status;
        bytes += 2U + prepared[i].length + (size_t)prepared[i].prefix + (i != 0U ? 1U : 0U);
        for (size_t j = 0U; j < prepared[i].length; ++j) if (prepared[i].text[j] == '"') ++bytes;
    }
    if (bytes > document->maximum - document->bytes - 1U) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* A temporary complete row makes self-copy safe across realloc and keeps
     * publication atomic if allocation fails. No partial CSV is observable. */
    char *row = malloc(bytes);
    if (row == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t at = 0U;
    for (size_t i = 0U; i < count; ++i) {
        if (i != 0U) row[at++] = ',';
        row[at++] = '"';
        if (prepared[i].prefix) row[at++] = '\'';
        for (size_t j = 0U; j < prepared[i].length; ++j) {
            char c = prepared[i].text[j];
            row[at++] = c;
            if (c == '"') row[at++] = c;
        }
        row[at++] = '"';
    }
    row[at++] = '\r'; row[at++] = '\n';
    size_t needed = document->bytes + bytes + 1U;
    if (needed > document->allocated) {
        size_t allocation = document->allocated * 2U;
        if (allocation < needed) allocation = needed;
        if (allocation > document->maximum) allocation = document->maximum;
        char *grown = realloc(document->data, allocation);
        if (grown == NULL) { free(row); return UMI_STATUS_OUT_OF_MEMORY; }
        document->data = grown; document->allocated = allocation;
    }
    memcpy(document->data + document->bytes, row, bytes); free(row);
    document->bytes += bytes; document->data[document->bytes] = '\0';
    document->columns = count; ++document->rows;
    return UMI_STATUS_OK;
}
const char *UmiCsvDocumentData(const UmiCsvDocument *document)
{ return document != NULL ? document->data : NULL; }
size_t UmiCsvDocumentBytes(const UmiCsvDocument *document)
{ return document != NULL ? document->bytes : 0U; }
size_t UmiCsvDocumentRows(const UmiCsvDocument *document)
{ return document != NULL ? document->rows : 0U; }
UmiStatus UmiCsvDocumentCopy(const UmiCsvDocument *document, char *output,
    size_t capacity, size_t *requiredCapacity)
{
    if (document == NULL || (output == NULL && capacity != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (requiredCapacity != NULL) *requiredCapacity = document->bytes + 1U;
    if (output == NULL) return UMI_STATUS_OK;
    if (capacity <= document->bytes) return UMI_STATUS_CAPACITY_EXCEEDED;
    memmove(output, document->data, document->bytes + 1U);
    return UMI_STATUS_OK;
}
