/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/types.c
 * PURPOSE: Implement shared bounded Desk control helpers.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/types.h"

#include <ctype.h>
#include <string.h>

/*
 * Provide the desktop control copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_control_copy_text(char *destination, size_t capacity,
                                        const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        (void)memcpy(destination, source, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Check that desktop control id satisfies its contract before another service relies on
 * it.
 */
bool umi_desktop_control_id_valid(const char *value)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value[0] == '\0') return false;
    length = strlen(value);
    return length < UMI_DESKTOP_CONTROL_ID_CAPACITY;
}

/*
 * Check that desktop control rect satisfies its contract before another service relies on
 * it.
 */
bool umi_desktop_control_rect_valid(const UmiDesktopControlRect *rect)
{
    return rect != NULL && rect->width > 0 && rect->height > 0;
}

/*
 * Provide the desktop control rect clamp operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_control_rect_clamp(UmiDesktopControlRect *rect,
                                         const UmiDesktopControlRect *bounds)
{
    int64_t max_x;
    int64_t max_y;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_desktop_control_rect_valid(rect) ||
        !umi_desktop_control_rect_valid(bounds)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (rect->width > bounds->width) rect->width = bounds->width;
    /* Apply this branch only when its contract condition is satisfied. */
    if (rect->height > bounds->height) rect->height = bounds->height;
    /* Apply this branch only when its contract condition is satisfied. */
    if (rect->x < bounds->x) rect->x = bounds->x;
    /* Apply this branch only when its contract condition is satisfied. */
    if (rect->y < bounds->y) rect->y = bounds->y;
    max_x = (int64_t)bounds->x + (int64_t)bounds->width - (int64_t)rect->width;
    max_y = (int64_t)bounds->y + (int64_t)bounds->height - (int64_t)rect->height;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((int64_t)rect->x > max_x) rect->x = (int32_t)max_x;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((int64_t)rect->y > max_y) rect->y = (int32_t)max_y;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop control hash text operation used by this module and its client
 * applications.
 */
uint64_t umi_desktop_control_hash_text(const char *value)
{
    const unsigned char *cursor = (const unsigned char *)value;
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return 0U;
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
 * Provide the desktop control text contains operation used by this module and its client
 * applications.
 */
bool umi_desktop_control_text_contains(const char *text, const char *needle)
{
    size_t text_length;
    size_t needle_length;
    size_t start;
    size_t offset;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || needle == NULL) return false;
    needle_length = strlen(needle);
    /* Apply this branch only when its contract condition is satisfied. */
    if (needle_length == 0U) return true;
    text_length = strlen(text);
    /* Apply this branch only when its contract condition is satisfied. */
    if (needle_length > text_length) return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (start = 0U; start + needle_length <= text_length; ++start) {
        bool match = true;
        /* Visit each bounded item once so every record receives the same rule. */
        for (offset = 0U; offset < needle_length; ++offset) {
            unsigned char left = (unsigned char)text[start + offset];
            unsigned char right = (unsigned char)needle[offset];
            /* Apply this branch only when its contract condition is satisfied. */
            if (tolower((int)left) != tolower((int)right)) {
                match = false;
                break;
            }
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (match) return true;
    }
    return false;
}
