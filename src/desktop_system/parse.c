/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/parse.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/desktop_system/linux_parse.h"
#include "internal.h"
#include <string.h>
#include <stdlib.h>

int UmiDesktopSystemAdd(uint64_t a, uint64_t b, uint64_t *out)
{
    if (b > UINT64_MAX - a) return 0;
    *out = a + b; return 1;
}
int UmiDesktopSystemMultiply(uint64_t a, uint64_t b, uint64_t *out)
{
    if (b != 0 && a > UINT64_MAX / b) return 0;
    *out = a * b; return 1;
}
static int Space(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
static void Skip(const char **p, const char *end) { while (*p < end && Space(**p)) ++*p; }
static int Number(const char **p, const char *end, uint64_t *out)
{
    uint64_t n = 0; const char *start = *p;
    while (*p < end && **p >= '0' && **p <= '9') {
        unsigned digit = (unsigned)(**p - '0');
        if (n > (UINT64_MAX - digit) / 10) return 0;
        n = n * 10 + digit; ++*p;
    }
    if (*p == start) return 0;
    *out = n; return 1;
}
static int ValidBytes(const char *text, size_t length)
{ return text != NULL && length != 0 && length <= 65536U && memchr(text, 0, length) == NULL; }
void UmiDesktopSystemDisplayName(char *out, size_t capacity, const char *in, size_t length)
{
    if (capacity == 0) return;
    size_t count = length < capacity - 1 ? length : capacity - 1;
    for (size_t i = 0; i < count; ++i) {
        unsigned char c = (unsigned char)in[i];
        out[i] = c >= 32U && c < 127U ? (char)c : '?';
    }
    out[count] = 0;
}
UmiStatus UmiDesktopSystemParseMemory(const char *text, size_t length,
    UmiDesktopSystemMemory *outMemory)
{
    if (!outMemory || !ValidBytes(text, length)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDesktopSystemMemory value = {0}; unsigned found = 0;
    const char *p = text, *end = text + length;
    while (p < end) {
        const char *lineEnd = memchr(p, '\n', (size_t)(end - p));
        if (!lineEnd) lineEnd = end;
        const char *colon = memchr(p, ':', (size_t)(lineEnd - p));
        if (!colon) return UMI_STATUS_PARSE_ERROR;
        size_t keyLength = (size_t)(colon - p); unsigned bit = 0;
        if (keyLength == 8 && memcmp(p, "MemTotal", 8) == 0) bit = 1;
        if (keyLength == 12 && memcmp(p, "MemAvailable", 12) == 0) bit = 2;
        if (bit) {
            uint64_t number, bytes; const char *q = colon + 1; Skip(&q, lineEnd);
            if ((found & bit) || !Number(&q, lineEnd, &number)) return UMI_STATUS_PARSE_ERROR;
            if (q == lineEnd || !Space(*q)) return UMI_STATUS_PARSE_ERROR;
            Skip(&q, lineEnd);
            if (lineEnd - q < 2 || memcmp(q, "kB", 2) != 0) return UMI_STATUS_PARSE_ERROR;
            q += 2; Skip(&q, lineEnd);
            if (q != lineEnd || !UmiDesktopSystemMultiply(number, 1024, &bytes)) return UMI_STATUS_PARSE_ERROR;
            if (bit == 1) value.totalBytes = bytes; else value.availableBytes = bytes;
            found |= bit;
        }
        p = lineEnd < end ? lineEnd + 1 : end;
    }
    if (found != 3 || value.totalBytes == 0 || value.availableBytes > value.totalBytes)
        return UMI_STATUS_PARSE_ERROR;
    *outMemory = value; return UMI_STATUS_OK;
}
UmiStatus UmiDesktopSystemParseCpu(const char *text, size_t length,
    UmiDesktopSystemCpu *outCpu)
{
    if (!outCpu || !ValidBytes(text, length)) return UMI_STATUS_INVALID_ARGUMENT;
    const char *end = memchr(text, '\n', length); if (!end) end = text + length;
    if (end - text < 4 || memcmp(text, "cpu ", 4) != 0) return UMI_STATUS_PARSE_ERROR;
    const char *p = text + 3; uint64_t fields[10] = {0}; size_t count = 0;
    while (p < end) {
        if (!Space(*p)) return UMI_STATUS_PARSE_ERROR;
        Skip(&p, end); if (p == end) break;
        if (count == 10 || !Number(&p, end, &fields[count++])) return UMI_STATUS_PARSE_ERROR;
    }
    if (count < 4) return UMI_STATUS_PARSE_ERROR;
    UmiDesktopSystemCpu value = {0}; memcpy(value.ticks, fields, sizeof value.ticks);
    /* guest and guest_nice already contribute to user/nice. Do not add twice. */
    *outCpu = value; return UMI_STATUS_OK;
}
UmiStatus UmiDesktopSystemParseProcess(const char *text, size_t length,
    uint64_t pageSize, UmiDesktopSystemProcess *outProcess)
{
    if (!outProcess || !pageSize || !ValidBytes(text, length)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDesktopSystemProcess value = {0};
    const char *p = text, *end = text + length;
    if (!Number(&p, end, &value.pid) || value.pid == 0 || p == end || *p++ != ' ' || p == end || *p++ != '(')
        return UMI_STATUS_PARSE_ERROR;
    const char *name = p, *close = NULL;
    /* comm may contain spaces and ')' characters. The last ')' is its boundary;
     * all following stat fields are numeric apart from the one-byte state. */
    for (const char *q = p; q < end; ++q) if (*q == ')') close = q;
    if (!close || close - name >= (ptrdiff_t)sizeof value.name || end - close < 4 ||
        close[1] != ' ' || close[3] != ' ' || !strchr("RSDZTWtXxKWPIN", close[2]))
        return UMI_STATUS_PARSE_ERROR;
    UmiDesktopSystemDisplayName(value.name, sizeof value.name, name, (size_t)(close - name));
    value.state = close[2]; p = close + 3;
    for (unsigned field = 4; field <= 24; ++field) {
        if (p == end || !Space(*p)) return UMI_STATUS_PARSE_ERROR;
        Skip(&p, end); int negative = p < end && *p == '-'; if (negative) ++p;
        uint64_t number;
        if (!Number(&p, end, &number) || (p < end && !Space(*p))) return UMI_STATUS_PARSE_ERROR;
        if ((field == 4 || field == 22 || field == 24) && negative) return UMI_STATUS_PARSE_ERROR;
        if (field == 4) value.parentPid = number;
        if (field == 22) { value.startTicks = number; value.startKnown = 1; }
        if (field == 24) {
            if (!UmiDesktopSystemMultiply(number, pageSize, &value.residentBytes)) return UMI_STATUS_PARSE_ERROR;
            value.residentKnown = 1;
        }
    }
    /* Later kernel fields are accepted only as complete signed numeric tokens. */
    while (p < end) {
        Skip(&p, end); if (p == end) break;
        if (*p == '-') ++p;
        uint64_t ignored;
        if (!Number(&p, end, &ignored) || (p < end && !Space(*p))) return UMI_STATUS_PARSE_ERROR;
    }
    *outProcess = value; return UMI_STATUS_OK;
}
UmiStatus UmiDesktopSystemParseInterface(const char *text, size_t length,
    UmiDesktopSystemInterface *outInterface)
{
    if (!outInterface || !ValidBytes(text, length)) return UMI_STATUS_INVALID_ARGUMENT;
    const char *p = text, *end = text + length; Skip(&p, end);
    const char *colon = memchr(p, ':', (size_t)(end - p));
    if (!colon || colon == p || colon - p >= UMI_DESKTOP_SYSTEM_NAME_CAPACITY) return UMI_STATUS_PARSE_ERROR;
    for (const char *q = p; q < colon; ++q)
        if (Space(*q) || (unsigned char)*q < 33U || (unsigned char)*q >= 127U) return UMI_STATUS_PARSE_ERROR;
    UmiDesktopSystemInterface value = {0};
    memcpy(value.name, p, (size_t)(colon - p)); p = colon + 1;
    for (unsigned i = 0; i < 16; ++i) {
        if (p == end || !Space(*p)) return UMI_STATUS_PARSE_ERROR;
        Skip(&p, end); uint64_t number;
        if (!Number(&p, end, &number) || (p < end && !Space(*p))) return UMI_STATUS_PARSE_ERROR;
        if (i == 0) value.receivedBytes = number;
        if (i == 8) value.transmittedBytes = number;
    }
    Skip(&p, end); if (p != end) return UMI_STATUS_PARSE_ERROR;
    *outInterface = value; return UMI_STATUS_OK;
}
UmiStatus UmiDesktopSystemCpuUsage(const UmiDesktopSystemCpu *before,
    const UmiDesktopSystemCpu *after, uint32_t *outBasisPoints)
{
    if (!before || !after || !outBasisPoints) return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t total = 0, idle = 0;
    for (unsigned i = 0; i < 8; ++i) {
        if (after->ticks[i] < before->ticks[i]) return UMI_STATUS_INVALID_STATE;
        uint64_t delta = after->ticks[i] - before->ticks[i];
        if (!UmiDesktopSystemAdd(total, delta, &total)) return UMI_STATUS_CAPACITY_EXCEEDED;
        if ((i == 3 || i == 4) && !UmiDesktopSystemAdd(idle, delta, &idle)) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (!total) return UMI_STATUS_UNAVAILABLE;
    uint64_t busy = total - idle, remainder = 0; uint32_t result = 0;
    /* Bounded repeated modular addition computes the exact integer ratio even
     * at UINT64_MAX. It needs no floating point, __int128 or overflowing product. */
    for (unsigned i = 0; i < 10000; ++i) {
        if (remainder >= total - busy) { remainder -= total - busy; ++result; }
        else remainder += busy;
    }
    *outBasisPoints = result; return UMI_STATUS_OK;
}
