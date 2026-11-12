/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/cmake_cache.c
 *
 * PURPOSE:
 *   Share literal cache-entry parsing between native repair and project identity review.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/cmake_cache.h"
#include "umicom/base/utf8.h"
#include "umicom/platform/input_file.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
/* The selected-entry parser originates in developer_project/build_cache.c.
 * Centralizing it here keeps CLI and Studio interpretation consistent while
 * leaving each consumer responsible for the fields its operation requires. */
typedef struct CacheField
{
    const char *name;
    size_t offset, capacity;
} CacheField;
#define CACHE_FIELD(name, member)                                                                  \
    {name, offsetof(UmiCMakeCache, member), sizeof(((UmiCMakeCache *)0)->member)}
static const CacheField CACHE_FIELDS[] = {CACHE_FIELD("CMAKE_HOME_DIRECTORY", source_directory),
                                          CACHE_FIELD("CMAKE_CACHEFILE_DIR", build_directory),
                                          CACHE_FIELD("CMAKE_GENERATOR", generator),
                                          CACHE_FIELD("CMAKE_C_COMPILER", compiler),
                                          CACHE_FIELD("CMAKE_BUILD_TYPE", configuration),
                                          CACHE_FIELD("CMAKE_CONFIGURATION_TYPES", configurations),
                                          CACHE_FIELD("CMAKE_INSTALL_PREFIX", install_directory),
                                          CACHE_FIELD("CMAKE_TOOLCHAIN_FILE", toolchain_file)};
#undef CACHE_FIELD
static bool CacheEqual(const char *text, size_t length, const char *expected)
{
    return strlen(expected) == length && memcmp(text, expected, length) == 0;
}
/* Cache values are not CMake quoted arguments: preserve spaces, backslashes,
 * semicolons and equals signs exactly. Only the first '=' separates the value.
 * Quoted key spelling is accepted because CMake can quote cache-entry names. */
static UmiStatus CacheLine(const char *line, size_t size, UmiCMakeCache *cache, unsigned *seen)
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
    for (size_t i = at; i < size; ++i)
        if ((unsigned char)line[i] < 0x20U || (unsigned char)line[i] == 0x7fU)
            return UMI_STATUS_PARSE_ERROR;
    char *value = (char *)cache + field->offset;
    memcpy(value, line + at, valueSize);
    value[valueSize] = '\0';
    *seen |= 1U << selected;
    return UMI_STATUS_OK;
}

UmiStatus UmiCMakeCacheParse(const void *bytes, size_t size, UmiCMakeCache *out)
{
    if (bytes == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (size > UMI_CMAKE_CACHE_BYTE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(bytes, 0, size) != NULL || !UmiUtf8Validate(bytes, size, NULL))
        return UMI_STATUS_PARSE_ERROR;
    UmiCMakeCache *cache = calloc(1U, sizeof *cache);
    if (cache == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    const char *text = bytes;
    size_t at = size >= 3U && memcmp(text, "\xef\xbb\xbf", 3U) == 0 ? 3U : 0U;
    UmiStatus status = UMI_STATUS_OK;
    while (at < size && status == UMI_STATUS_OK) {
        size_t begin = at;
        while (at < size && text[at] != '\n') ++at;
        size_t end = at;
        if (end > begin && text[end - 1U] == '\r') --end;
        status = CacheLine(text + begin, end - begin, cache, &cache->present);
        if (at < size) ++at;
    }
    if (status == UMI_STATUS_OK) *out = *cache;
    free(cache);
    return status;
}
UmiStatus UmiCMakeCacheRead(const char *path, UmiCMakeCache *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiStatus status = UmiInputFileRead(path, UMI_CMAKE_CACHE_BYTE_LIMIT, &bytes, &size);
    if (status == UMI_STATUS_OK) status = UmiCMakeCacheParse(bytes, size, out);
    UmiInputFileFree(bytes);
    return status;
}
