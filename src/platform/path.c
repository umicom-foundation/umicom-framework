/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/path.c
 *
 * PURPOSE:
 *   Implement deterministic path parsing and normalisation for Windows and
 *   POSIX while keeping the public contract independent from native path APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/path.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#define UMI_PATH_SEPARATOR '\\'
#else
#define UMI_PATH_SEPARATOR '/'
#endif

#define UMI_PATH_MAX_SEGMENTS 256U

static int is_separator(char value)
{
    return value == '/' || value == '\\';
}

static int path_character_equal(char left, char right)
{
#ifdef _WIN32
    return tolower((unsigned char)left) == tolower((unsigned char)right);
#else
    return left == right;
#endif
}

static int segment_equal(const char *left, const char *right)
{
    size_t index = 0U;
    if (left == NULL || right == NULL) {
        return 0;
    }
    while (left[index] != '\0' && right[index] != '\0') {
        if (!path_character_equal(left[index], right[index])) {
            return 0;
        }
        ++index;
    }
    return left[index] == right[index];
}

UmiStatus umi_path_copy(char *out_path,
                        size_t capacity,
                        const char *path)
{
    size_t length;
    if (out_path == NULL || capacity == 0U || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(path);
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_path, path, length + 1U);
    return UMI_STATUS_OK;
}

int umi_path_is_absolute(const char *path)
{
    if (path == NULL || path[0] == '\0') {
        return 0;
    }
#ifdef _WIN32
    return (isalpha((unsigned char)path[0]) && path[1] == ':' &&
            is_separator(path[2])) ||
           (is_separator(path[0]) && is_separator(path[1]));
#else
    return path[0] == '/';
#endif
}

static UmiStatus append_text(char *out_path,
                             size_t capacity,
                             size_t *used,
                             const char *text)
{
    size_t length;
    if (out_path == NULL || used == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    if (*used + length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_path + *used, text, length);
    *used += length;
    out_path[*used] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_path_normalise(const char *path,
                             char *out_path,
                             size_t capacity)
{
    char buffer[UMI_PATH_CAPACITY];
    char *segments[UMI_PATH_MAX_SEGMENTS];
    size_t segment_count = 0U;
    size_t prefix_length = 0U;
    size_t read_index = 0U;
    size_t write_index = 0U;
    size_t output_used = 0U;
    int rooted = 0;
    int unc = 0;
    size_t index;

    if (path == NULL || out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(path) >= sizeof(buffer)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

#ifdef _WIN32
    if (isalpha((unsigned char)path[0]) && path[1] == ':') {
        buffer[write_index++] = (char)toupper((unsigned char)path[0]);
        buffer[write_index++] = ':';
        prefix_length = 2U;
        read_index = 2U;
        if (is_separator(path[read_index])) {
            rooted = 1;
            ++read_index;
        }
    } else if (is_separator(path[0]) && is_separator(path[1])) {
        unc = 1;
        rooted = 1;
        read_index = 2U;
    }
#else
    if (path[0] == '/') {
        rooted = 1;
        read_index = 1U;
    }
#endif

    while (path[read_index] != '\0') {
        size_t start;
        while (is_separator(path[read_index])) {
            ++read_index;
        }
        if (path[read_index] == '\0') {
            break;
        }
        start = write_index;
        while (path[read_index] != '\0' && !is_separator(path[read_index])) {
            if (write_index + 1U >= sizeof(buffer)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            buffer[write_index++] = path[read_index++];
        }
        buffer[write_index++] = '\0';
        segments[segment_count++] = &buffer[start];
        if (segment_count >= UMI_PATH_MAX_SEGMENTS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }

    {
        char *resolved[UMI_PATH_MAX_SEGMENTS];
        size_t resolved_count = 0U;
        for (index = 0U; index < segment_count; ++index) {
            char *segment = segments[index];
            if (strcmp(segment, ".") == 0 || segment[0] == '\0') {
                continue;
            }
            if (strcmp(segment, "..") == 0) {
                if (resolved_count > 0U &&
                    strcmp(resolved[resolved_count - 1U], "..") != 0) {
                    --resolved_count;
                } else if (!rooted) {
                    resolved[resolved_count++] = segment;
                }
                continue;
            }
            resolved[resolved_count++] = segment;
        }

        out_path[0] = '\0';
#ifdef _WIN32
        if (prefix_length == 2U) {
            char prefix[3];
            prefix[0] = buffer[0];
            prefix[1] = ':';
            prefix[2] = '\0';
            if (append_text(out_path, capacity, &output_used, prefix) !=
                UMI_STATUS_OK) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        } else if (unc) {
            char prefix[3] = {'\\', '\\', '\0'};
            if (append_text(out_path, capacity, &output_used, prefix) !=
                UMI_STATUS_OK) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        }
#else
        (void)prefix_length;
        (void)unc;
#endif
        if (rooted && output_used == 0U) {
            if (output_used + 2U > capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_path[output_used++] = UMI_PATH_SEPARATOR;
            out_path[output_used] = '\0';
        } else if (rooted && output_used > 0U &&
                   !is_separator(out_path[output_used - 1U])) {
            if (output_used + 2U > capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_path[output_used++] = UMI_PATH_SEPARATOR;
            out_path[output_used] = '\0';
        }

        for (index = 0U; index < resolved_count; ++index) {
            if (output_used > 0U &&
                !is_separator(out_path[output_used - 1U])) {
                if (output_used + 2U > capacity) {
                    return UMI_STATUS_CAPACITY_EXCEEDED;
                }
                out_path[output_used++] = UMI_PATH_SEPARATOR;
                out_path[output_used] = '\0';
            }
            if (append_text(out_path,
                            capacity,
                            &output_used,
                            resolved[index]) != UMI_STATUS_OK) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        }
    }

    if (out_path[0] == '\0') {
        if (rooted) {
            if (capacity < 2U) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_path[0] = UMI_PATH_SEPARATOR;
            out_path[1] = '\0';
        } else {
            return umi_path_copy(out_path, capacity, ".");
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_path_join(const char *left,
                        const char *right,
                        char *out_path,
                        size_t capacity)
{
    char combined[UMI_PATH_CAPACITY];
    int written;
    if (left == NULL || right == NULL || out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_path_is_absolute(right)) {
        return umi_path_normalise(right, out_path, capacity);
    }
    written = snprintf(combined,
                       sizeof(combined),
                       "%s%c%s",
                       left,
                       UMI_PATH_SEPARATOR,
                       right);
    if (written < 0 || (size_t)written >= sizeof(combined)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_path_normalise(combined, out_path, capacity);
}

UmiStatus umi_path_parent(const char *path,
                          char *out_path,
                          size_t capacity)
{
    char normalised[UMI_PATH_CAPACITY];
    char *last = NULL;
    size_t index;
    UmiStatus status;
    if (path == NULL || out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_path_normalise(path, normalised, sizeof(normalised));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    for (index = 0U; normalised[index] != '\0'; ++index) {
        if (is_separator(normalised[index])) {
            last = &normalised[index];
        }
    }
    if (last == NULL) {
        return umi_path_copy(out_path, capacity, ".");
    }
#ifdef _WIN32
    if (last == normalised + 2 && normalised[1] == ':') {
        last[1] = '\0';
    } else if (last == normalised) {
        last[1] = '\0';
    } else {
        *last = '\0';
    }
#else
    if (last == normalised) {
        last[1] = '\0';
    } else {
        *last = '\0';
    }
#endif
    return umi_path_copy(out_path, capacity, normalised);
}

UmiStatus umi_path_basename(const char *path,
                            char *out_name,
                            size_t capacity)
{
    char normalised[UMI_PATH_CAPACITY];
    const char *last = normalised;
    size_t index;
    UmiStatus status;
    if (path == NULL || out_name == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_path_normalise(path, normalised, sizeof(normalised));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    for (index = 0U; normalised[index] != '\0'; ++index) {
        if (is_separator(normalised[index])) {
            last = &normalised[index + 1U];
        }
    }
    return umi_path_copy(out_name, capacity, last);
}

UmiStatus umi_path_extension(const char *path,
                             char *out_extension,
                             size_t capacity)
{
    char name[UMI_PATH_CAPACITY];
    const char *dot;
    UmiStatus status;
    status = umi_path_basename(path, name, sizeof(name));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    dot = strrchr(name, '.');
    if (dot == NULL || dot == name) {
        return umi_path_copy(out_extension, capacity, "");
    }
    return umi_path_copy(out_extension, capacity, dot + 1);
}

UmiStatus umi_path_stem(const char *path,
                        char *out_stem,
                        size_t capacity)
{
    char name[UMI_PATH_CAPACITY];
    char *dot;
    UmiStatus status;
    status = umi_path_basename(path, name, sizeof(name));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    dot = strrchr(name, '.');
    if (dot != NULL && dot != name) {
        *dot = '\0';
    }
    return umi_path_copy(out_stem, capacity, name);
}

UmiStatus umi_path_absolute(const char *path,
                            const char *base_directory,
                            char *out_path,
                            size_t capacity)
{
    if (path == NULL || out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_path_is_absolute(path)) {
        return umi_path_normalise(path, out_path, capacity);
    }
    if (base_directory == NULL || base_directory[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_path_join(base_directory, path, out_path, capacity);
}

static size_t split_segments(char *path,
                             char **segments,
                             size_t capacity)
{
    size_t count = 0U;
    char *cursor = path;
    while (*cursor != '\0') {
        while (is_separator(*cursor)) {
            ++cursor;
        }
        if (*cursor == '\0') {
            break;
        }
        if (count >= capacity) {
            return SIZE_MAX;
        }
        segments[count++] = cursor;
        while (*cursor != '\0' && !is_separator(*cursor)) {
            ++cursor;
        }
        if (*cursor != '\0') {
            *cursor++ = '\0';
        }
    }
    return count;
}

UmiStatus umi_path_relative(const char *base_directory,
                            const char *target_path,
                            char *out_path,
                            size_t capacity)
{
    char base[UMI_PATH_CAPACITY];
    char target[UMI_PATH_CAPACITY];
    char *base_segments[UMI_PATH_MAX_SEGMENTS];
    char *target_segments[UMI_PATH_MAX_SEGMENTS];
    size_t base_count;
    size_t target_count;
    size_t common = 0U;
    size_t index;
    size_t used = 0U;
    UmiStatus status;

    if (base_directory == NULL || target_path == NULL || out_path == NULL ||
        capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_path_normalise(base_directory, base, sizeof(base));
    if (status != UMI_STATUS_OK) return status;
    status = umi_path_normalise(target_path, target, sizeof(target));
    if (status != UMI_STATUS_OK) return status;
#ifdef _WIN32
    if (isalpha((unsigned char)base[0]) && base[1] == ':' &&
        isalpha((unsigned char)target[0]) && target[1] == ':' &&
        !path_character_equal(base[0], target[0])) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
#endif
    base_count = split_segments(base, base_segments, UMI_PATH_MAX_SEGMENTS);
    target_count = split_segments(target,
                                  target_segments,
                                  UMI_PATH_MAX_SEGMENTS);
    if (base_count == SIZE_MAX || target_count == SIZE_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    while (common < base_count && common < target_count &&
           segment_equal(base_segments[common], target_segments[common])) {
        ++common;
    }
    out_path[0] = '\0';
    for (index = common; index < base_count; ++index) {
        if (used > 0U) {
            if (used + 2U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            out_path[used++] = UMI_PATH_SEPARATOR;
            out_path[used] = '\0';
        }
        status = append_text(out_path, capacity, &used, "..");
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = common; index < target_count; ++index) {
        if (used > 0U) {
            if (used + 2U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
            out_path[used++] = UMI_PATH_SEPARATOR;
            out_path[used] = '\0';
        }
        status = append_text(out_path,
                             capacity,
                             &used,
                             target_segments[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    if (used == 0U) {
        return umi_path_copy(out_path, capacity, ".");
    }
    return UMI_STATUS_OK;
}

int umi_path_equal(const char *left, const char *right)
{
    char left_normalised[UMI_PATH_CAPACITY];
    char right_normalised[UMI_PATH_CAPACITY];
    size_t index = 0U;
    if (umi_path_normalise(left,
                           left_normalised,
                           sizeof(left_normalised)) != UMI_STATUS_OK ||
        umi_path_normalise(right,
                           right_normalised,
                           sizeof(right_normalised)) != UMI_STATUS_OK) {
        return 0;
    }
    while (left_normalised[index] != '\0' &&
           right_normalised[index] != '\0') {
        if (!path_character_equal(left_normalised[index],
                                  right_normalised[index])) {
            return 0;
        }
        ++index;
    }
    return left_normalised[index] == right_normalised[index];
}

int umi_path_is_within(const char *parent, const char *candidate)
{
    char parent_normalised[UMI_PATH_CAPACITY];
    char candidate_normalised[UMI_PATH_CAPACITY];
    size_t index;
    size_t parent_length;
    if (umi_path_normalise(parent,
                           parent_normalised,
                           sizeof(parent_normalised)) != UMI_STATUS_OK ||
        umi_path_normalise(candidate,
                           candidate_normalised,
                           sizeof(candidate_normalised)) != UMI_STATUS_OK) {
        return 0;
    }
    parent_length = strlen(parent_normalised);
    if (strlen(candidate_normalised) < parent_length) {
        return 0;
    }
    for (index = 0U; index < parent_length; ++index) {
        if (!path_character_equal(parent_normalised[index],
                                  candidate_normalised[index])) {
            return 0;
        }
    }
    if (candidate_normalised[parent_length] == '\0') {
        return 1;
    }
    return is_separator(candidate_normalised[parent_length]);
}
