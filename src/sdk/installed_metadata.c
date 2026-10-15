/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/installed_metadata.c
 * PURPOSE: Parse the installed SDK receipt independently from the running Framework version.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/sdk/installed_metadata.h"
#include "umicom/platform/input_file.h"
#include <stdlib.h>
#include <string.h>

/* Metadata is generated as data, never executable CMake. A small grammar makes
 * malformed or partially copied receipts fail without running package scripts. */
static UmiStatus MetadataNumber(const char *text, uint32_t maximum, uint32_t *out)
{
    uint32_t value = 0U;
    if (*text == '\0') return UMI_STATUS_PARSE_ERROR;
    for (; *text != '\0'; ++text) {
        unsigned char digit = (unsigned char)*text;
        if (digit < '0' || digit > '9') return UMI_STATUS_PARSE_ERROR;
        uint32_t add = (uint32_t)(digit - '0');
        if (value > (maximum - add) / 10U) return UMI_STATUS_CAPACITY_EXCEEDED;
        value = value * 10U + add;
    }
    *out = value;
    return UMI_STATUS_OK;
}

static UmiStatus MetadataVersion(char *value, UmiVersion *out)
{
    uint16_t parts[3];
    /* Split exactly three numeric components, with no suffix or empty component.
     * Checked narrowing keeps an unrelated installation from wrapping a version. */
    for (size_t i = 0U; i < 3U; ++i) {
        char *dot = strchr(value, '.');
        if ((i < 2U && dot == NULL) || (i == 2U && dot != NULL))
            return UMI_STATUS_PARSE_ERROR;
        if (dot != NULL) *dot = '\0';
        uint32_t number;
        UmiStatus status = MetadataNumber(value, UINT16_MAX, &number);
        if (status != UMI_STATUS_OK) return status;
        parts[i] = (uint16_t)number;
        if (dot != NULL) value = dot + 1;
    }
    *out = (UmiVersion){parts[0], parts[1], parts[2]};
    return UMI_STATUS_OK;
}

static UmiStatus MetadataPath(const char *value, char *out, size_t capacity)
{
    size_t length = strlen(value), segment = 0U;
    if (length == 0U || length >= capacity)
        return length == 0U ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_CAPACITY_EXCEEDED;
    /* Relative path components keep the observation beneath the chosen prefix.
     * Symlinks in trusted installations are still followed by presence checks. */
    for (size_t i = 0U; i <= length; ++i) {
        unsigned char ch = (unsigned char)value[i];
        if (ch == '/' || ch == '\0') {
            size_t count = i - segment;
            if (count == 0U || (count == 1U && value[segment] == '.') ||
                (count == 2U && value[segment] == '.' && value[segment + 1U] == '.') ||
                value[segment] == ' ' || value[i - 1U] == ' ' || value[i - 1U] == '.')
                return UMI_STATUS_PARSE_ERROR;
            segment = i + 1U;
        } else if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') || ch == '_' || ch == '-' || ch == '.' ||
            ch == '+' || ch == ' ')) {
            return UMI_STATUS_PARSE_ERROR;
        }
    }
    memcpy(out, value, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus UmiSdkInstalledMetadataParse(const void *bytes, size_t length,
    UmiSdkInstalledMetadata *out_metadata)
{
    if (out_metadata == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_metadata, 0, sizeof *out_metadata);
    if (bytes == NULL || length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_SDK_METADATA_BYTE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    const unsigned char *input = bytes;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char ch = input[i];
        if ((ch < 32U && ch != '\n' && ch != '\r') || ch == 127U ||
            (ch == '\r' && (i + 1U == length || input[i + 1U] != '\n')))
            return UMI_STATUS_PARSE_ERROR;
    }
    char *text = malloc(length + 1U);
    UmiSdkInstalledMetadata *candidate = calloc(1U, sizeof *candidate);
    if (text == NULL || candidate == NULL) {
        free(text); free(candidate);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(text, bytes, length); text[length] = '\0';
    static const char *const keys[] = {"schema", "framework_version", "framework_abi",
        "include_directory", "library_directory", "base_library"};
    unsigned seen = 0U;
    UmiStatus status = UMI_STATUS_OK;
    char *cursor = text;
    while (*cursor != '\0' && status == UMI_STATUS_OK) {
        char *line = cursor, *newline = strchr(cursor, '\n');
        if (newline != NULL) { *newline = '\0'; cursor = newline + 1; }
        else cursor += strlen(cursor);
        size_t count = strlen(line);
        if (count != 0U && line[count - 1U] == '\r') line[--count] = '\0';
        if (count == 0U || line[0] == '#') continue;
        char *equals = strchr(line, '=');
        if (equals == NULL) { status = UMI_STATUS_PARSE_ERROR; break; }
        *equals = '\0';
        size_t key = 0U;
        while (key < 6U && strcmp(line, keys[key]) != 0) ++key;
        if (key == 6U || (seen & (1U << key)) != 0U) {
            status = UMI_STATUS_PARSE_ERROR; break;
        }
        seen |= 1U << key;
        char *value = equals + 1;
        switch (key) {
        case 0U:
            if (strcmp(value, "umicom.sdk") != 0) status = UMI_STATUS_PARSE_ERROR;
            break;
        case 1U: status = MetadataVersion(value, &candidate->framework_version); break;
        case 2U:
            status = MetadataNumber(value, UINT32_MAX, &candidate->framework_abi);
            if (status == UMI_STATUS_OK && candidate->framework_abi == 0U)
                status = UMI_STATUS_PARSE_ERROR;
            break;
        case 3U: status = MetadataPath(value, candidate->include_directory, sizeof candidate->include_directory); break;
        case 4U: status = MetadataPath(value, candidate->library_directory, sizeof candidate->library_directory); break;
        default: status = MetadataPath(value, candidate->base_library, sizeof candidate->base_library); break;
        }
    }
    if (status == UMI_STATUS_OK && seen != 63U) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) *out_metadata = *candidate;
    free(candidate); free(text);
    return status;
}

UmiStatus UmiSdkInstalledMetadataLoad(const char *prefix, UmiSdkInstalledMetadata *out_metadata)
{
    if (out_metadata == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_metadata, 0, sizeof *out_metadata);
    if (prefix == NULL || !umi_path_is_absolute(prefix)) return UMI_STATUS_INVALID_ARGUMENT;
    char path[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_join(prefix, UMI_SDK_METADATA_RELATIVE_PATH, path, sizeof path);
    unsigned char *bytes = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiInputFileRead(path, UMI_SDK_METADATA_BYTE_LIMIT, &bytes, &length);
    if (status == UMI_STATUS_OK) status = UmiSdkInstalledMetadataParse(bytes, length, out_metadata);
    UmiInputFileFree(bytes);
    return status;
}
