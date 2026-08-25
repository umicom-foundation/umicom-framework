/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/types.c
 *
 * PURPOSE:
 *   Implement bounded text, identifier and stable hashing helpers used by the
 *   dependency injection and application bootstrap platform.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/types.h"

#include <string.h>

UmiStatus umi_bootstrap_copy_text(char *dst, size_t capacity, const char *src) {
    size_t length;
    if (dst == NULL || capacity == 0U || src == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(src);
    if (length >= capacity) {
        dst[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(dst, src, length + 1U);
    return UMI_STATUS_OK;
}

bool umi_bootstrap_id_valid(const char *text) {
    const unsigned char *cursor;
    if (text == NULL || text[0] == '\0') {
        return false;
    }
    for (cursor = (const unsigned char *)text; *cursor != 0U; ++cursor) {
        const bool valid = (*cursor >= (unsigned char)'a' && *cursor <= (unsigned char)'z') ||
                           (*cursor >= (unsigned char)'A' && *cursor <= (unsigned char)'Z') ||
                           (*cursor >= (unsigned char)'0' && *cursor <= (unsigned char)'9') ||
                           *cursor == (unsigned char)'.' || *cursor == (unsigned char)'-' ||
                           *cursor == (unsigned char)'_' || *cursor == (unsigned char)':';
        if (!valid) {
            return false;
        }
    }
    return true;
}

uint64_t umi_bootstrap_hash_text(const char *text) {
    const unsigned char *cursor = (const unsigned char *)(text != NULL ? text : "");
    uint64_t hash = UINT64_C(1469598103934665603);
    while (*cursor != 0U) {
        hash ^= (uint64_t)*cursor;
        hash *= UINT64_C(1099511628211);
        ++cursor;
    }
    return hash;
}

int umi_bootstrap_text_compare(const char *left, const char *right) {
    if (left == NULL && right == NULL) {
        return 0;
    }
    if (left == NULL) {
        return -1;
    }
    if (right == NULL) {
        return 1;
    }
    return strcmp(left, right);
}
