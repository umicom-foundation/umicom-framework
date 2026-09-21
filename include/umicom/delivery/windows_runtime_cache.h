/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/windows_runtime_cache.h
 *
 * PURPOSE:
 *   Expand relocatable image-loader cache templates without depending on GTK.
 *   This bounded, portable contract is also used by the native Windows startup
 *   adapter before application code initialises its graphical interface.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_WINDOWS_RUNTIME_CACHE_H
#define UMICOM_DELIVERY_WINDOWS_RUNTIME_CACHE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WINDOWS_RUNTIME_ROOT_TOKEN "@UMICOM_RUNTIME_ROOT@"

typedef enum UmiWindowsRuntimeCacheResult {
    UMI_WINDOWS_RUNTIME_CACHE_OK = 0,
    UMI_WINDOWS_RUNTIME_CACHE_INVALID_ARGUMENT,
    UMI_WINDOWS_RUNTIME_CACHE_INSUFFICIENT_CAPACITY,
    UMI_WINDOWS_RUNTIME_CACHE_SIZE_OVERFLOW
} UmiWindowsRuntimeCacheResult;

/**
 * Expand every runtime-root token. requiredSize includes the terminating NUL.
 * A NULL output with zero capacity performs a size query. A short output is
 * never partially written. Input, runtimeRoot and output must not overlap.
 * The template may contain arbitrary UTF-8 bytes, but not embedded NUL bytes.
 * Windows backslashes in runtimeRoot become forward slashes in the cache.
 * Quotes and line breaks in the root are rejected rather than injected into
 * the GdkPixbuf cache grammar. No allocation, file I/O or global state is used.
 */
UmiWindowsRuntimeCacheResult UmiWindowsRuntimeExpandCache(
    const char *templateText,
    size_t templateSize,
    const char *runtimeRoot,
    char *output,
    size_t outputCapacity,
    size_t *requiredSize);

#ifdef __cplusplus
}
#endif
#endif
