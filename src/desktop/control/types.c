/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/control/types.c
 * PURPOSE: Implement shared bounded Desk control helpers.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/control/types.h"

#include <ctype.h>
#include <string.h>

UmiStatus umi_desktop_control_copy_text(char *destination, size_t capacity,
                                        const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length > 0U) {
        (void)memcpy(destination, source, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

bool umi_desktop_control_id_valid(const char *value)
{
    size_t length;
    if (value == NULL || value[0] == '\0') return false;
    length = strlen(value);
    return length < UMI_DESKTOP_CONTROL_ID_CAPACITY;
}

bool umi_desktop_control_rect_valid(const UmiDesktopControlRect *rect)
{
    return rect != NULL && rect->width > 0 && rect->height > 0;
}

UmiStatus umi_desktop_control_rect_clamp(UmiDesktopControlRect *rect,
                                         const UmiDesktopControlRect *bounds)
{
    int64_t max_x;
    int64_t max_y;
    if (!umi_desktop_control_rect_valid(rect) ||
        !umi_desktop_control_rect_valid(bounds)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (rect->width > bounds->width) rect->width = bounds->width;
    if (rect->height > bounds->height) rect->height = bounds->height;
    if (rect->x < bounds->x) rect->x = bounds->x;
    if (rect->y < bounds->y) rect->y = bounds->y;
    max_x = (int64_t)bounds->x + (int64_t)bounds->width - (int64_t)rect->width;
    max_y = (int64_t)bounds->y + (int64_t)bounds->height - (int64_t)rect->height;
    if ((int64_t)rect->x > max_x) rect->x = (int32_t)max_x;
    if ((int64_t)rect->y > max_y) rect->y = (int32_t)max_y;
    return UMI_STATUS_OK;
}

uint64_t umi_desktop_control_hash_text(const char *value)
{
    const unsigned char *cursor = (const unsigned char *)value;
    uint64_t hash = UINT64_C(1469598103934665603);
    if (value == NULL) return 0U;
    while (*cursor != 0U) {
        hash ^= (uint64_t)*cursor;
        hash *= UINT64_C(1099511628211);
        ++cursor;
    }
    return hash;
}

bool umi_desktop_control_text_contains(const char *text, const char *needle)
{
    size_t text_length;
    size_t needle_length;
    size_t start;
    size_t offset;
    if (text == NULL || needle == NULL) return false;
    needle_length = strlen(needle);
    if (needle_length == 0U) return true;
    text_length = strlen(text);
    if (needle_length > text_length) return false;
    for (start = 0U; start + needle_length <= text_length; ++start) {
        bool match = true;
        for (offset = 0U; offset < needle_length; ++offset) {
            unsigned char left = (unsigned char)text[start + offset];
            unsigned char right = (unsigned char)needle[offset];
            if (tolower((int)left) != tolower((int)right)) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}
