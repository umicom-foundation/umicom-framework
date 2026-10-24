/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Bounded metadata fields shared by lineage and amendment-history codecs.
 * This is not a free-text document format: control characters are rejected so
 * separators cannot change the meaning of a saved actor or source identifier. */
#ifndef UMICOM_DATA_RECORD_FIELDS_INTERNAL_H
#define UMICOM_DATA_RECORD_FIELDS_INTERNAL_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static inline bool umi_record_field_valid(const char *text, size_t capacity,
                                          bool allow_empty)
{
    const char *end = memchr(text, '\0', capacity);
    if (end == NULL || (!allow_empty && end == text)) {
        return false;
    }
    for (const char *p = text; p < end; ++p) {
        unsigned char byte = (unsigned char)*p;
        if (byte < 32U || byte == 127U) {
            return false;
        }
    }
    return true;
}

/* Split exactly the expected number of fields, including empty legacy fields.
 * No global parser state is used, so unrelated readers cannot interfere. */
static inline bool umi_record_fields_split(char *text, size_t capacity,
                                           char **fields, size_t count)
{
    if (count == 0U || memchr(text, '\0', capacity) == NULL) {
        return false;
    }
    fields[0] = text;
    size_t found = 1U;
    for (char *p = text; *p != '\0'; ++p) {
        if (*p == '\t') {
            if (found == count) {
                return false;
            }
            *p = '\0';
            fields[found++] = p + 1;
        }
    }
    return found == count;
}

static inline bool umi_record_field_copy(char *target, size_t capacity,
                                         const char *text, bool allow_empty)
{
    size_t length = strlen(text);
    if (length >= capacity || !umi_record_field_valid(text, length + 1U, allow_empty)) {
        return false;
    }
    memcpy(target, text, length + 1U);
    return true;
}

/* Decimal identifiers must not accept signs, suffixes or arithmetic wraparound. */
static inline bool umi_record_field_u64(const char *text, uint64_t *value)
{
    uint64_t parsed = 0U;
    if (*text == '\0') {
        return false;
    }
    for (const char *p = text; *p != '\0'; ++p) {
        if (*p < '0' || *p > '9') {
            return false;
        }
        uint64_t digit = (uint64_t)(*p - '0');
        if (parsed > (UINT64_MAX - digit) / 10U) {
            return false;
        }
        parsed = parsed * 10U + digit;
    }
    *value = parsed;
    return true;
}
#endif
