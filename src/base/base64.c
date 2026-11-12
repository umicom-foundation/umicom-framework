/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/base64.c
 *
 * PURPOSE:
 *   Share strict, allocation-free base64 admission between binary content consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/base/base64.h"
#include <stdbool.h>
#include <stdint.h>

static bool B64Space(unsigned char value)
{
    return value == ' ' || value == '\t' || value == '\r' || value == '\n' ||
        value == '\f' || value == '\v';
}
static int B64Value(unsigned char value)
{
    if (value >= 'A' && value <= 'Z') return value - 'A';
    if (value >= 'a' && value <= 'z') return 26 + value - 'a';
    if (value >= '0' && value <= '9') return 52 + value - '0';
    if (value == '+') return 62;
    if (value == '/') return 63;
    return -1;
}
/* uintptr_t comparisons avoid relational comparison of unrelated C objects.
 * Reject wrapping spans before any overlap calculation or input traversal.
 * This checks arithmetic only; callers still own readable/writable extents. */
static bool B64Span(const void *pointer, size_t length)
{
    return length == 0U || (pointer != NULL &&
        (uintmax_t)length <= (uintmax_t)UINTPTR_MAX - (uintptr_t)pointer);
}
static bool B64Overlap(const void *a, size_t an, const void *b, size_t bn)
{
    uintptr_t av = (uintptr_t)a, bv = (uintptr_t)b;
    if (an == 0U || bn == 0U) return false;
    return av <= bv ? bv - av < an : av - bv < bn;
}
/* Admission has no output side effects. Counting each complete quartet also
 * avoids multiplying the caller length, which can overflow at SIZE_MAX. */
static UmiStatus B64Measure(const char *text, size_t length, unsigned flags,
    size_t *count)
{
    unsigned quartet[4] = {0U};
    size_t used = 0U, total = 0U;
    bool padded = false;
    if ((flags & ~UMI_BASE64_IGNORE_ASCII_WHITESPACE) != 0U ||
        !B64Span(text, length)) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char c = (unsigned char)text[i];
        if ((flags & UMI_BASE64_IGNORE_ASCII_WHITESPACE) != 0U && B64Space(c)) continue;
        if (padded) return UMI_STATUS_PARSE_ERROR;
        int value = B64Value(c);
        if (c != '=' && value < 0) return UMI_STATUS_PARSE_ERROR;
        quartet[used++] = c == '=' ? 64U : (unsigned)value;
        if (used == 4U) {
            size_t addition = 3U;
            if (quartet[0] == 64U || quartet[1] == 64U) return UMI_STATUS_PARSE_ERROR;
            if (quartet[2] == 64U) {
                if (quartet[3] != 64U || (quartet[1] & 15U) != 0U)
                    return UMI_STATUS_PARSE_ERROR;
                addition = 1U;
                padded = true;
            } else if (quartet[3] == 64U) {
                if ((quartet[2] & 3U) != 0U) return UMI_STATUS_PARSE_ERROR;
                addition = 2U;
                padded = true;
            }
            if (total > SIZE_MAX - addition) return UMI_STATUS_CAPACITY_EXCEEDED;
            total += addition;
            used = 0U;
        }
    }
    if (used != 0U) return UMI_STATUS_PARSE_ERROR;
    *count = total;
    return UMI_STATUS_OK;
}
UmiStatus UmiBase64DecodedSize(const char *text, size_t length,
    unsigned flags, size_t *out_count)
{
    size_t count;
    if (out_count == NULL || !B64Span(out_count, sizeof(*out_count)) ||
        !B64Span(text, length) ||
        B64Overlap(text, length, out_count, sizeof(*out_count)))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = B64Measure(text, length, flags, &count);
    if (status == UMI_STATUS_OK) *out_count = count;
    return status;
}
UmiStatus UmiBase64Decode(const char *text, size_t length, unsigned flags,
    unsigned char *bytes, size_t capacity, size_t *out_count)
{
    size_t count, used = 0U, position = 0U;
    unsigned quartet[4] = {0U};
    if (out_count == NULL || !B64Span(out_count, sizeof(*out_count)) ||
        !B64Span(text, length) ||
        B64Overlap(text, length, out_count, sizeof(*out_count)))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = B64Measure(text, length, flags, &count);
    if (status != UMI_STATUS_OK) return status;
    if (!B64Span(bytes, count)) return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < count) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (B64Overlap(text, length, bytes, count) ||
        B64Overlap(bytes, count, out_count, sizeof(*out_count)))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* The checked first pass guarantees complete canonical quartets. Keep the
     * second pass bounded by the same stable input; never rescan with strlen. */
    for (size_t i = 0U; i < length; ++i) {
        unsigned char c = (unsigned char)text[i];
        if ((flags & UMI_BASE64_IGNORE_ASCII_WHITESPACE) != 0U && B64Space(c)) continue;
        quartet[used++] = c == '=' ? 64U : (unsigned)B64Value(c);
        if (used == 4U) {
            bytes[position++] = (unsigned char)((quartet[0] << 2U) | (quartet[1] >> 4U));
            if (quartet[2] != 64U)
                bytes[position++] = (unsigned char)((quartet[1] << 4U) | (quartet[2] >> 2U));
            if (quartet[3] != 64U)
                bytes[position++] = (unsigned char)((quartet[2] << 6U) | quartet[3]);
            used = 0U;
        }
    }
    *out_count = count;
    return UMI_STATUS_OK;
}
