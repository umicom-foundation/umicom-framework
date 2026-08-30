/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance_persistence.c
 *
 * PURPOSE:
 *   Encode and decode a complete appearance profile for session stores and
 *   settings providers without selecting a product-specific file format.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The record is versioned and percent-escapes text before using a vertical bar
 * as the separator.  The decoder reconstructs a temporary profile and only
 * returns it after the normal Framework validation succeeds.
 */

#include "umicom/ui/appearance_persistence.h"

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIELD_COUNT 33U
#define FIELD_CAPACITY 512U

static int unreserved(unsigned char value)
{
    return isalnum(value) || value == '-' || value == '_' || value == '.' ||
           value == ' ' || value == ',' || value == '#' || value == '/';
}

static UmiStatus append_raw(char *out, size_t capacity, size_t *used,
                            const char *text)
{
    size_t length;
    if (out == NULL || used == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    if (*used + length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out + *used, text, length);
    *used += length;
    out[*used] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus append_field(char *out, size_t capacity, size_t *used,
                              const char *text)
{
    static const char HEX[] = "0123456789ABCDEF";
    const unsigned char *cursor = (const unsigned char *)text;
    UmiStatus status = append_raw(out, capacity, used, "|");
    if (status != UMI_STATUS_OK) return status;
    while (*cursor != '\0') {
        char encoded[4] = {0};
        if (unreserved(*cursor) && *cursor != '|') {
            encoded[0] = (char)*cursor;
        } else {
            encoded[0] = '%';
            encoded[1] = HEX[*cursor >> 4U];
            encoded[2] = HEX[*cursor & 15U];
        }
        status = append_raw(out, capacity, used, encoded);
        if (status != UMI_STATUS_OK) return status;
        ++cursor;
    }
    return UMI_STATUS_OK;
}

static UmiStatus append_integer(char *out, size_t capacity, size_t *used,
                                int64_t value)
{
    char text[64U];
    int written = snprintf(text, sizeof(text), "|%" PRId64, value);
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    return append_raw(out, capacity, used, text);
}

static UmiStatus append_double(char *out, size_t capacity, size_t *used,
                               double value)
{
    char text[64U];
    int written = snprintf(text, sizeof(text), "|%.3f", value);
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    return append_raw(out, capacity, used, text);
}

UmiStatus umi_ui_appearance_profile_encode(
    const UmiUiAppearanceProfile *profile,
    char *out_text,
    size_t capacity)
{
    char reason[192U];
    size_t used = 0U;
    UmiStatus status;

#define TEXT(value) do { status = append_field(out_text, capacity, &used, value); if (status != UMI_STATUS_OK) return status; } while (0)
#define INTEGER(value) do { status = append_integer(out_text, capacity, &used, (int64_t)(value)); if (status != UMI_STATUS_OK) return status; } while (0)
#define DECIMAL(value) do { status = append_double(out_text, capacity, &used, value); if (status != UMI_STATUS_OK) return status; } while (0)

    if (profile == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_ui_appearance_profile_validate(profile, reason, sizeof(reason)) !=
        UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    out_text[0] = '\0';
    status = append_raw(out_text, capacity, &used, "appearance-v1");
    if (status != UMI_STATUS_OK) return status;
    TEXT(profile->profile_id); TEXT(profile->label); TEXT(profile->description);
    INTEGER(profile->mode); INTEGER(profile->density);
    TEXT(profile->brand_name); TEXT(profile->logo_resource);
    TEXT(profile->icon_resource); TEXT(profile->interface_font);
    TEXT(profile->editor_font); DECIMAL(profile->interface_font_size);
    DECIMAL(profile->editor_font_size); DECIMAL(profile->font_scale);
    TEXT(profile->background); TEXT(profile->surface);
    TEXT(profile->raised_surface); TEXT(profile->hover_surface);
    TEXT(profile->editor_background); TEXT(profile->foreground);
    TEXT(profile->muted_foreground); TEXT(profile->border);
    TEXT(profile->strong_border); TEXT(profile->accent);
    TEXT(profile->accent_surface); TEXT(profile->success);
    TEXT(profile->warning); TEXT(profile->danger);
    INTEGER(profile->active); INTEGER(profile->built_in);
    INTEGER(profile->locked); INTEGER(profile->reduce_motion);
    INTEGER(profile->revision);
    return UMI_STATUS_OK;

#undef TEXT
#undef INTEGER
#undef DECIMAL
}

static int hex_value(char value)
{
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    return -1;
}

static UmiStatus decode_field(const char *begin, const char *end,
                              char *out, size_t capacity)
{
    size_t used = 0U;
    while (begin < end) {
        unsigned char value = (unsigned char)*begin++;
        if (value == '%') {
            int high;
            int low;
            if (end - begin < 2) return UMI_STATUS_INVALID_STATE;
            high = hex_value(begin[0]);
            low = hex_value(begin[1]);
            if (high < 0 || low < 0) return UMI_STATUS_INVALID_STATE;
            value = (unsigned char)((high << 4) | low);
            begin += 2;
        }
        if (used + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        out[used++] = (char)value;
    }
    out[used] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus split_fields(const char *text,
                              char fields[FIELD_COUNT][FIELD_CAPACITY])
{
    const char *begin = text;
    const char *separator;
    size_t index = 0U;
    UmiStatus status;
    while (index < FIELD_COUNT) {
        separator = strchr(begin, '|');
        if (separator == NULL) separator = begin + strlen(begin);
        status = decode_field(begin, separator, fields[index], FIELD_CAPACITY);
        if (status != UMI_STATUS_OK) return status;
        ++index;
        if (*separator == '\0') break;
        begin = separator + 1;
    }
    if (index != FIELD_COUNT || *separator != '\0') {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

static UmiStatus parse_integer(const char *text, int64_t *out_value)
{
    char *end = NULL;
    long long value;
    errno = 0;
    value = strtoll(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0') {
        return UMI_STATUS_INVALID_STATE;
    }
    *out_value = (int64_t)value;
    return UMI_STATUS_OK;
}

static UmiStatus parse_double(const char *text, double *out_value)
{
    char *end = NULL;
    double value;
    errno = 0;
    value = strtod(text, &end);
    if (errno != 0 || end == text || *end != '\0') {
        return UMI_STATUS_INVALID_STATE;
    }
    *out_value = value;
    return UMI_STATUS_OK;
}

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    if (strlen(source) >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)snprintf(destination, capacity, "%s", source);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_appearance_profile_decode(
    const char *text,
    UmiUiAppearanceProfile *out_profile)
{
    char fields[FIELD_COUNT][FIELD_CAPACITY] = {{0}};
    UmiUiAppearanceProfile profile;
    int64_t values[7U];
    double interface_size;
    double editor_size;
    double font_scale;
    char reason[192U];
    UmiStatus status;

    if (text == NULL || out_profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = split_fields(text, fields);
    if (status != UMI_STATUS_OK || strcmp(fields[0], "appearance-v1") != 0) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (parse_integer(fields[4], &values[0]) != UMI_STATUS_OK ||
        parse_integer(fields[5], &values[1]) != UMI_STATUS_OK ||
        parse_double(fields[11], &interface_size) != UMI_STATUS_OK ||
        parse_double(fields[12], &editor_size) != UMI_STATUS_OK ||
        parse_double(fields[13], &font_scale) != UMI_STATUS_OK ||
        parse_integer(fields[28], &values[2]) != UMI_STATUS_OK ||
        parse_integer(fields[29], &values[3]) != UMI_STATUS_OK ||
        parse_integer(fields[30], &values[4]) != UMI_STATUS_OK ||
        parse_integer(fields[31], &values[5]) != UMI_STATUS_OK ||
        parse_integer(fields[32], &values[6]) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_ui_appearance_profile_init(
        &profile, fields[1], fields[2], (UmiUiThemeMode)values[0],
        (UmiUiDensity)values[1]);
    if (status != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;

    if (copy_text(profile.description, sizeof(profile.description), fields[3]) !=
            UMI_STATUS_OK ||
        copy_text(profile.brand_name, sizeof(profile.brand_name), fields[6]) !=
            UMI_STATUS_OK ||
        copy_text(profile.logo_resource, sizeof(profile.logo_resource),
                  fields[7]) != UMI_STATUS_OK ||
        copy_text(profile.icon_resource, sizeof(profile.icon_resource),
                  fields[8]) != UMI_STATUS_OK ||
        copy_text(profile.interface_font, sizeof(profile.interface_font),
                  fields[9]) != UMI_STATUS_OK ||
        copy_text(profile.editor_font, sizeof(profile.editor_font),
                  fields[10]) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    profile.interface_font_size = interface_size;
    profile.editor_font_size = editor_size;
    profile.font_scale = font_scale;
    if (copy_text(profile.background, sizeof(profile.background), fields[14]) !=
            UMI_STATUS_OK ||
        copy_text(profile.surface, sizeof(profile.surface), fields[15]) !=
            UMI_STATUS_OK ||
        copy_text(profile.raised_surface, sizeof(profile.raised_surface),
                  fields[16]) != UMI_STATUS_OK ||
        copy_text(profile.hover_surface, sizeof(profile.hover_surface),
                  fields[17]) != UMI_STATUS_OK ||
        copy_text(profile.editor_background, sizeof(profile.editor_background),
                  fields[18]) != UMI_STATUS_OK ||
        copy_text(profile.foreground, sizeof(profile.foreground), fields[19]) !=
            UMI_STATUS_OK ||
        copy_text(profile.muted_foreground,
                  sizeof(profile.muted_foreground), fields[20]) !=
            UMI_STATUS_OK ||
        copy_text(profile.border, sizeof(profile.border), fields[21]) !=
            UMI_STATUS_OK ||
        copy_text(profile.strong_border, sizeof(profile.strong_border),
                  fields[22]) != UMI_STATUS_OK ||
        copy_text(profile.accent, sizeof(profile.accent), fields[23]) !=
            UMI_STATUS_OK ||
        copy_text(profile.accent_surface, sizeof(profile.accent_surface),
                  fields[24]) != UMI_STATUS_OK ||
        copy_text(profile.success, sizeof(profile.success), fields[25]) !=
            UMI_STATUS_OK ||
        copy_text(profile.warning, sizeof(profile.warning), fields[26]) !=
            UMI_STATUS_OK ||
        copy_text(profile.danger, sizeof(profile.danger), fields[27]) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    profile.active = values[2] != 0;
    profile.built_in = values[3] != 0;
    profile.locked = values[4] != 0;
    profile.reduce_motion = values[5] != 0;
    profile.revision = (uint64_t)values[6];

    if (umi_ui_appearance_profile_validate(&profile, reason, sizeof(reason)) !=
        UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    *out_profile = profile;
    return UMI_STATUS_OK;
}
