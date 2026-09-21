/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/windows_runtime_cache.c
 *
 * PURPOSE:
 *   Implement bounded relocation of GTK image-loader cache paths.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/windows_runtime_cache.h"

#include <stdint.h>
#include <string.h>

/* Measure before writing so a capacity failure cannot publish a partial cache. */
UmiWindowsRuntimeCacheResult UmiWindowsRuntimeExpandCache(
    const char *templateText, size_t templateSize, const char *runtimeRoot,
    char *output, size_t outputCapacity, size_t *requiredSize)
{
    static const char token[] = UMI_WINDOWS_RUNTIME_ROOT_TOKEN;
    const size_t tokenSize = sizeof(token) - 1U;
    size_t rootSize;
    size_t inputOffset = 0U;
    size_t needed = 1U;
    size_t outputOffset = 0U;

    if (requiredSize == NULL) {
        return UMI_WINDOWS_RUNTIME_CACHE_INVALID_ARGUMENT;
    }
    *requiredSize = 0U;
    if (templateText == NULL || runtimeRoot == NULL ||
        (output == NULL && outputCapacity != 0U)) {
        return UMI_WINDOWS_RUNTIME_CACHE_INVALID_ARGUMENT;
    }
    rootSize = strlen(runtimeRoot);
    if (rootSize == 0U || strchr(runtimeRoot, '"') != NULL ||
        strchr(runtimeRoot, '\r') != NULL || strchr(runtimeRoot, '\n') != NULL ||
        strchr(runtimeRoot, '\t') != NULL ||
        memchr(templateText, '\0', templateSize) != NULL) {
        return UMI_WINDOWS_RUNTIME_CACHE_INVALID_ARGUMENT;
    }

    while (inputOffset < templateSize) {
        size_t added = 1U;
        size_t consumed = 1U;
        if (templateSize - inputOffset >= tokenSize &&
            memcmp(templateText + inputOffset, token, tokenSize) == 0) {
            added = rootSize;
            consumed = tokenSize;
        }
        if (added > SIZE_MAX - needed) {
            return UMI_WINDOWS_RUNTIME_CACHE_SIZE_OVERFLOW;
        }
        needed += added;
        inputOffset += consumed;
    }
    *requiredSize = needed;
    if (output == NULL) {
        return UMI_WINDOWS_RUNTIME_CACHE_OK;
    }
    if (outputCapacity < needed) {
        return UMI_WINDOWS_RUNTIME_CACHE_INSUFFICIENT_CAPACITY;
    }

    inputOffset = 0U;
    while (inputOffset < templateSize) {
        if (templateSize - inputOffset >= tokenSize &&
            memcmp(templateText + inputOffset, token, tokenSize) == 0) {
            for (size_t rootOffset = 0U; rootOffset < rootSize; ++rootOffset) {
                const char value = runtimeRoot[rootOffset];
                output[outputOffset++] = value == '\\' ? '/' : value;
            }
            inputOffset += tokenSize;
        } else {
            output[outputOffset++] = templateText[inputOffset++];
        }
    }
    output[outputOffset] = '\0';
    return UMI_WINDOWS_RUNTIME_CACHE_OK;
}
