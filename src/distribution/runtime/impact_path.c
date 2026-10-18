/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/impact_path.c
 * Purpose: Match repository-relative paths without filesystem or working-directory assumptions.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "impact_internal.h"
#include <stdlib.h>
#include <string.h>

/* Lexical matching is deliberately exact. A case or alias mismatch becomes an
 * unmapped path and widens coverage; it must never silently drop a changed file. */
static bool Absolute(const char *path)
{
    return path[0] == '/' || (path[0] != '\0' && path[1] == ':');
}
static UmiStatus Normalise(const char *input, bool relative, char **out)
{
    if (input == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length <= UMI_RELEASE_IMPACT_PATH_LIMIT && input[length] != '\0') ++length;
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_RELEASE_IMPACT_PATH_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    char *copy = malloc(length + 1U);
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char ch = (unsigned char)input[i];
        if (ch < 32U || ch == 127U) { free(copy); return UMI_STATUS_INVALID_ARGUMENT; }
        copy[i] = ch == '\\' ? '/' : (char)ch;
    }
    copy[length] = '\0';
    if (relative && Absolute(copy)) { free(copy); return UMI_STATUS_INVALID_ARGUMENT; }
    size_t start = copy[0] == '/' ? 1U : 0U;
    if (!relative && length >= 3U && copy[1] == ':' && copy[2] == '/') start = 3U;
    for (size_t i = start; i <= length; ++i) {
        if (copy[i] != '/' && copy[i] != '\0') continue;
        size_t n = i - start;
        if (n == 0U || (n == 1U && copy[start] == '.') ||
            (n == 2U && copy[start] == '.' && copy[start + 1U] == '.')) {
            free(copy); return UMI_STATUS_INVALID_ARGUMENT;
        }
        start = i + 1U;
    }
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiImpactChangedPath(const char *input, char **outPath)
{
    return Normalise(input, true, outPath);
}
UmiStatus UmiImpactRelativePath(const char *root, const char *input, char **outPath)
{
    char *normal = NULL;
    UmiStatus status = Normalise(input, false, &normal);
    if (status != UMI_STATUS_OK) return status;
    if (!Absolute(normal)) { *outPath = normal; return UMI_STATUS_OK; }
    /* The inventory root is produced by CMake REAL_PATH. Normalise separators
     * only here: a filesystem root legitimately ends with its root separator. */
    if (root == NULL) { free(normal); return UMI_STATUS_INVALID_ARGUMENT; }
    size_t n = strlen(root);
    if (n == 0U || n > UMI_RELEASE_IMPACT_PATH_LIMIT) {
        free(normal); return UMI_STATUS_INVALID_ARGUMENT;
    }
    char *prefix = malloc(n + 1U);
    if (prefix == NULL) { free(normal); return UMI_STATUS_OUT_OF_MEMORY; }
    for (size_t i = 0U; i <= n; ++i) prefix[i] = root[i] == '\\' ? '/' : root[i];
    while (n > 1U && prefix[n - 1U] == '/') --n;
    bool rootSlash = n == 1U && prefix[0] == '/';
    size_t offset = rootSlash ? 1U : n + 1U;
    bool inside = strncmp(normal, prefix, n) == 0 &&
        (rootSlash || normal[n] == '/') && strlen(normal) > offset;
    free(prefix);
    if (!inside) { free(normal); return UMI_STATUS_NOT_FOUND; }
    memmove(normal, normal + offset, strlen(normal + offset) + 1U);
    *outPath = normal;
    return UMI_STATUS_OK;
}
bool UmiImpactBuildConfiguration(const char *path)
{
    const char *name = strrchr(path, '/');
    name = name == NULL ? path : name + 1;
    size_t size = strlen(name);
    return strcmp(name, "CMakeLists.txt") == 0 || strcmp(name, "CMakePresets.json") == 0 ||
        strcmp(name, "CMakeUserPresets.json") == 0 || strcmp(name, ".gitmodules") == 0 ||
        (size >= 6U && strcmp(name + size - 6U, ".cmake") == 0);
}
