/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/federation/codec.c
 *
 * PURPOSE:
 *   Encode workspace headers as strict versioned text, independent of compiler layout.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

UmiStatus DfParseUnsigned(const char *text, uint64_t *outValue)
{
    uint64_t value = 0U;
    size_t index = 0U;
    if (text == NULL || outValue == NULL || text[0] == '\0') return UMI_STATUS_PARSE_ERROR;
    if (text[0] == '0' && text[1] != '\0') return UMI_STATUS_PARSE_ERROR;
    while (text[index] != '\0') {
        unsigned digit;
        if (text[index] < '0' || text[index] > '9' || index >= 20U) return UMI_STATUS_PARSE_ERROR;
        digit = (unsigned)(text[index++] - '0');
        if (value > (UINT64_MAX - digit) / UINT64_C(10)) return UMI_STATUS_PARSE_ERROR;
        value = value * UINT64_C(10) + digit;
    }
    *outValue = value; return UMI_STATUS_OK;
}
static int HexValue(char ch)
{
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return -1;
}
UmiStatus DfEncodeHeader(const UmiDeskWorkspace *workspace, char *output, size_t capacity)
{
    static const char digits[] = "0123456789abcdef";
    char hex[UMI_DESK_FEDERATION_TITLE_CAPACITY * 2U];
    int written;
    size_t length = strlen(workspace->title);
    for (size_t index = 0U; index < length; ++index) {
        unsigned char ch = (unsigned char)workspace->title[index];
        hex[index * 2U] = digits[ch >> 4U]; hex[index * 2U + 1U] = digits[ch & 15U];
    }
    hex[length * 2U] = '\0';
    written = snprintf(output, capacity, "DF1|%u|%s|%s|%zu", (unsigned)workspace->kind,
        workspace->id, hex, workspace->memberCount);
    return written >= 0 && (size_t)written < capacity ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}
UmiStatus DfDecodeHeader(const char *text, UmiDeskWorkspace *outWorkspace)
{
    char buffer[640], *fields[5], *cursor;
    UmiDeskWorkspace workspace = {0};
    uint64_t kind, count;
    size_t length;
    if (DfCopy(buffer, sizeof buffer, text) != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    fields[0] = buffer; cursor = buffer;
    for (size_t index = 1U; index < 5U; ++index) {
        cursor = strchr(cursor, '|'); if (cursor == NULL) return UMI_STATUS_PARSE_ERROR;
        *cursor++ = '\0'; fields[index] = cursor;
    }
    if (strchr(cursor, '|') != NULL || strcmp(fields[0], "DF1") != 0 ||
        DfParseUnsigned(fields[1], &kind) != UMI_STATUS_OK ||
        DfParseUnsigned(fields[4], &count) != UMI_STATUS_OK ||
        (kind != 1U && kind != 2U) || count == 0U || count > UMI_DESK_FEDERATION_MAX_APPS ||
        DfCopy(workspace.id, sizeof workspace.id, fields[2]) != UMI_STATUS_OK ||
        !DfIdValid(workspace.id, sizeof workspace.id)) return UMI_STATUS_PARSE_ERROR;
    workspace.kind = (UmiDeskWorkspaceKind)kind; workspace.memberCount = (size_t)count;
    length = strlen(fields[3]);
    if (length == 0U || length % 2U != 0U || length / 2U >= sizeof workspace.title) return UMI_STATUS_PARSE_ERROR;
    for (size_t index = 0U; index < length; index += 2U) {
        int high = HexValue(fields[3][index]), low = HexValue(fields[3][index + 1U]);
        if (high < 0 || low < 0 || (high == 0 && low == 0)) return UMI_STATUS_PARSE_ERROR;
        workspace.title[index / 2U] = (char)(high * 16 + low);
    }
    if (!DfTextValid(workspace.title, sizeof workspace.title)) return UMI_STATUS_PARSE_ERROR;
    *outWorkspace = workspace; return UMI_STATUS_OK;
}
