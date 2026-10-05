/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/build_cache.c
 * PURPOSE: Read selected build-cache values and expose project identity mismatches for review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/build_cache.h"
#include "umicom/document/text_encoding.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/path.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
typedef struct CacheField
{
    const char *name;
    size_t offset, capacity;
    int required;
} CacheField;
#define CACHE_FIELD(name, member, required)                                                                  \
    {name, offsetof(UmiProjectBuildCache, member), sizeof(((UmiProjectBuildCache *)0)->member), required}
static const CacheField CACHE_FIELDS[] = {CACHE_FIELD("CMAKE_HOME_DIRECTORY", source_directory, 1),
                                          CACHE_FIELD("CMAKE_CACHEFILE_DIR", build_directory, 1),
                                          CACHE_FIELD("CMAKE_GENERATOR", generator, 1),
                                          CACHE_FIELD("CMAKE_C_COMPILER", compiler, 0),
                                          CACHE_FIELD("CMAKE_BUILD_TYPE", configuration, 0),
                                          CACHE_FIELD("CMAKE_CONFIGURATION_TYPES", configurations, 0),
                                          CACHE_FIELD("CMAKE_INSTALL_PREFIX", install_directory, 0),
                                          CACHE_FIELD("CMAKE_TOOLCHAIN_FILE", toolchain_file, 0)};
#undef CACHE_FIELD
static bool CacheEqual(const char *text, size_t length, const char *expected)
{
    return strlen(expected) == length && memcmp(text, expected, length) == 0;
}
static UmiStatus CacheDirectory(const char *path, char *out)
{
    if (path == NULL || !umi_path_is_absolute(path))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_path_normalise(path, out, UMI_BUILD_PATH_CAPACITY);
    return status == UMI_STATUS_OK ? UmiOutputFileValidatePath(out) : status;
}
/* Cache values are not CMake quoted arguments: preserve spaces, backslashes,
 * semicolons and equals signs exactly. Only the first '=' separates the value.
 * Quoted key spelling is accepted because CMake can quote cache-entry names. */
static UmiStatus CacheLine(const char *line, size_t size, UmiProjectBuildCache *cache, unsigned *seen)
{
    while (size != 0U && (*line == ' ' || *line == '\t'))
    {
        ++line;
        --size;
    }
    if (size == 0U || line[0] == '#' || (size >= 2U && line[0] == '/' && line[1] == '/'))
        return UMI_STATUS_OK;
    const char *name = line;
    size_t nameSize = 0U, at = 0U;
    if (line[0] == '"')
    {
        name = ++line;
        --size;
        while (at < size && line[at] != '"')
            ++at;
        nameSize = at;
        if (at < size)
            ++at;
    }
    else
    {
        while (at < size && line[at] != ':' && line[at] != '=')
            ++at;
        nameSize = at;
    }
    size_t selected = sizeof(CACHE_FIELDS) / sizeof(CACHE_FIELDS[0]);
    for (size_t i = 0U; i < sizeof(CACHE_FIELDS) / sizeof(CACHE_FIELDS[0]); ++i)
        if (CacheEqual(name, nameSize, CACHE_FIELDS[i].name))
        {
            selected = i;
            break;
        }
    if (selected == sizeof(CACHE_FIELDS) / sizeof(CACHE_FIELDS[0]))
        return UMI_STATUS_OK;
    if ((*seen & (1U << selected)) != 0U)
        return UMI_STATUS_ALREADY_EXISTS;
    if (at == size || line[at++] != ':')
        return UMI_STATUS_PARSE_ERROR;
    size_t type = at;
    while (at < size && line[at] != '=')
        ++at;
    if (at == size)
        return UMI_STATUS_PARSE_ERROR;
    bool knownType = false;
    const char *types[] = {"INTERNAL", "STRING", "PATH", "FILEPATH", "STATIC", "UNINITIALIZED"};
    for (size_t i = 0U; i < sizeof(types) / sizeof(types[0]); ++i)
        if (CacheEqual(line + type, at - type, types[i]))
            knownType = true;
    if (!knownType)
        return UMI_STATUS_PARSE_ERROR;
    ++at;
    const CacheField *field = &CACHE_FIELDS[selected];
    size_t valueSize = size - at;
    if (valueSize >= field->capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (field->required && valueSize == 0U)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = at; i < size; ++i)
        if ((unsigned char)line[i] < 0x20U || (unsigned char)line[i] == 0x7fU)
            return UMI_STATUS_PARSE_ERROR;
    char *value = (char *)cache + field->offset;
    memcpy(value, line + at, valueSize);
    value[valueSize] = '\0';
    *seen |= 1U << selected;
    return UMI_STATUS_OK;
}
UmiStatus UmiProjectBuildCacheParse(const void *bytes, size_t size, const char *sourceDirectory,
                                    const char *buildDirectory, UmiProjectBuildCache *out)
{
    if (bytes == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (size > UMI_PROJECT_BUILD_CACHE_BYTE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(bytes, 0, size) != NULL || !umi_document_utf8_validate(bytes, size, NULL))
        return UMI_STATUS_PARSE_ERROR;
    char source[UMI_BUILD_PATH_CAPACITY], build[UMI_BUILD_PATH_CAPACITY];
    UmiStatus status = CacheDirectory(sourceDirectory, source);
    if (status == UMI_STATUS_OK)
        status = CacheDirectory(buildDirectory, build);
    if (status != UMI_STATUS_OK)
        return status;
    /* Build the full result separately. A late malformed line must not leave a
     * caller displaying a mixture of a previous cache and this partial read. */
    UmiProjectBuildCache *cache = calloc(1U, sizeof(*cache));
    if (cache == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    const char *text = bytes;
    size_t at = size >= 3U && memcmp(text, "\xef\xbb\xbf", 3U) == 0 ? 3U : 0U;
    unsigned seen = 0U;
    while (at < size && status == UMI_STATUS_OK)
    {
        size_t begin = at;
        while (at < size && text[at] != '\n')
            ++at;
        size_t end = at;
        if (end > begin && text[end - 1U] == '\r')
            --end;
        status = CacheLine(text + begin, end - begin, cache, &seen);
        if (at < size)
            ++at;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < sizeof(CACHE_FIELDS) / sizeof(CACHE_FIELDS[0]); ++i)
        if (CACHE_FIELDS[i].required && (seen & (1U << i)) == 0U)
            status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
    {
        char recorded[UMI_BUILD_PATH_CAPACITY];
        status = CacheDirectory(cache->source_directory, recorded);
        if (status == UMI_STATUS_OK)
            cache->source_matches = umi_path_equal(source, recorded);
        if (status == UMI_STATUS_OK)
            status = CacheDirectory(cache->build_directory, recorded);
        if (status == UMI_STATUS_OK)
            cache->build_matches = umi_path_equal(build, recorded);
    }
    if (status == UMI_STATUS_OK)
        *out = *cache;
    free(cache);
    return status;
}
UmiStatus UmiProjectBuildCacheRead(const char *sourceDirectory, const char *buildDirectory,
                                   UmiProjectBuildCache *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    char source[UMI_BUILD_PATH_CAPACITY], build[UMI_BUILD_PATH_CAPACITY], path[UMI_BUILD_PATH_CAPACITY];
    UmiStatus status = CacheDirectory(sourceDirectory, source);
    if (status == UMI_STATUS_OK)
        status = CacheDirectory(buildDirectory, build);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(build, "CMakeCache.txt", path, sizeof(path));
    unsigned char *bytes = NULL;
    size_t size = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiInputFileRead(path, UMI_PROJECT_BUILD_CACHE_BYTE_LIMIT, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiProjectBuildCacheParse(bytes, size, source, build, out);
    UmiInputFileFree(bytes);
    return status;
}
