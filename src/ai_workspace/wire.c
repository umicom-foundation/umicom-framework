/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/wire.c
 *
 * PURPOSE:
 *   Encode portable, bounded source and job records without native-memory persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workspace_internal.h"
#include <string.h>

void AwWireInit(AwWire *wire) { memset(wire, 0, sizeof(*wire)); }
static void AwWireByte(AwWire *wire, char value)
{
    if (wire->status != UMI_STATUS_OK) return;
    if (wire->offset + 1U >= sizeof(wire->text)) { wire->status = UMI_STATUS_CAPACITY_EXCEEDED; return; }
    wire->text[wire->offset++] = value;
    wire->text[wire->offset] = '\0';
}
void AwWireText(AwWire *wire, const char *text)
{
    static const char hex[] = "0123456789abcdef";
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        const unsigned char byte = (unsigned char)text[i];
        AwWireByte(wire, hex[byte >> 4U]);
        AwWireByte(wire, hex[byte & 15U]);
    }
    AwWireByte(wire, '|');
}
void AwWireNumber(AwWire *wire, uint64_t value)
{
    char digits[20]; size_t count = 0U;
    do { digits[count++] = (char)('0' + value % 10U); value /= 10U; } while (value != 0U);
    while (count != 0U) AwWireByte(wire, digits[--count]);
    AwWireByte(wire, '|');
}
static int AwHex(char value)
{
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'a' && value <= 'f') return 10 + value - 'a';
    return -1;
}
UmiStatus AwWireReadText(AwWire *wire, char *out, size_t capacity)
{
    size_t count = 0U;
    if (wire == NULL || out == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    out[0] = '\0';
    while (wire->offset < sizeof(wire->text) && wire->text[wire->offset] != '|') {
        int high, low;
        if (wire->offset + 1U >= sizeof(wire->text) || count + 1U >= capacity) return UMI_STATUS_PARSE_ERROR;
        high = AwHex(wire->text[wire->offset++]);
        low = AwHex(wire->text[wire->offset++]);
        if (high < 0 || low < 0 || (high == 0 && low == 0)) return UMI_STATUS_PARSE_ERROR;
        out[count++] = (char)((unsigned)high * 16U + (unsigned)low);
    }
    if (wire->offset >= sizeof(wire->text) || wire->text[wire->offset++] != '|') return UMI_STATUS_PARSE_ERROR;
    out[count] = '\0';
    return AwTextValid(out, capacity, true) ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
UmiStatus AwWireReadNumber(AwWire *wire, uint64_t *out)
{
    uint64_t value = 0U; size_t count = 0U;
    if (wire == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    while (wire->offset < sizeof(wire->text) && wire->text[wire->offset] != '|') {
        unsigned digit; char next = wire->text[wire->offset++];
        if (next < '0' || next > '9' || count >= 20U) return UMI_STATUS_PARSE_ERROR;
        digit = (unsigned)(next - '0');
        if (value > (UINT64_MAX - digit) / 10U) return UMI_STATUS_PARSE_ERROR;
        value = value * 10U + digit; ++count;
    }
    if (count == 0U || wire->offset >= sizeof(wire->text) || wire->text[wire->offset++] != '|')
        return UMI_STATUS_PARSE_ERROR;
    *out = value; return UMI_STATUS_OK;
}
bool AwWireEnd(const AwWire *wire)
{ return wire->offset < sizeof(wire->text) && wire->text[wire->offset] == '\0'; }
