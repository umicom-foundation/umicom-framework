/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/types.c
 *
 * PURPOSE:
 *   Implement the types behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
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

/*
 * Provide the bootstrap copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_copy_text(char *dst, size_t capacity, const char *src) {
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dst == NULL || capacity == 0U || src == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(src);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        dst[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(dst, src, length + 1U);
    return UMI_STATUS_OK;
}

/* Check that bootstrap id satisfies its contract before another service relies on it. */
bool umi_bootstrap_id_valid(const char *text) {
    const unsigned char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || text[0] == '\0') {
        return false;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = (const unsigned char *)text; *cursor != 0U; ++cursor) {
        const bool valid = (*cursor >= (unsigned char)'a' && *cursor <= (unsigned char)'z') ||
                           (*cursor >= (unsigned char)'A' && *cursor <= (unsigned char)'Z') ||
                           (*cursor >= (unsigned char)'0' && *cursor <= (unsigned char)'9') ||
                           *cursor == (unsigned char)'.' || *cursor == (unsigned char)'-' ||
                           *cursor == (unsigned char)'_' || *cursor == (unsigned char)':';
        /* Apply this operation only while the related capability or state is available. */
        if (!valid) {
            return false;
        }
    }
    return true;
}

/*
 * Provide the bootstrap hash text operation used by this module and its client
 * applications.
 */
uint64_t umi_bootstrap_hash_text(const char *text) {
    const unsigned char *cursor = (const unsigned char *)(text != NULL ? text : "");
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        hash ^= (uint64_t)*cursor;
        hash *= UINT64_C(1099511628211);
        ++cursor;
    }
    return hash;
}

/*
 * Provide the bootstrap text compare operation used by this module and its client
 * applications.
 */
int umi_bootstrap_text_compare(const char *left, const char *right) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL && right == NULL) {
        return 0;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL) {
        return -1;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (right == NULL) {
        return 1;
    }
    return strcmp(left, right);
}
