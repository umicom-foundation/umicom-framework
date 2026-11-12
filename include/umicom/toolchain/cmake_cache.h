/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/cmake_cache.h
 *
 * PURPOSE:
 *   Inspect selected CMake cache metadata without evaluating project scripts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TOOLCHAIN_CMAKE_CACHE_H
#define UMICOM_TOOLCHAIN_CMAKE_CACHE_H
#include "umicom/build/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CMAKE_CACHE_BYTE_LIMIT (8U * 1024U * 1024U)
#define UMI_CMAKE_CACHE_SOURCE (1U << 0)
#define UMI_CMAKE_CACHE_BUILD (1U << 1)
#define UMI_CMAKE_CACHE_GENERATOR (1U << 2)
#define UMI_CMAKE_CACHE_COMPILER (1U << 3)
/* The path/name capacities are shared value-record limits, not a dependency
 * on the build runner. The bit mask distinguishes an absent optional entry
 * from a present empty entry. It is not a configure/build success receipt. */
typedef struct UmiCMakeCache {
    char source_directory[UMI_BUILD_PATH_CAPACITY];
    char build_directory[UMI_BUILD_PATH_CAPACITY];
    char generator[UMI_BUILD_NAME_CAPACITY];
    char compiler[UMI_BUILD_PATH_CAPACITY];
    char configuration[UMI_BUILD_NAME_CAPACITY];
    char configurations[UMI_BUILD_PATH_CAPACITY];
    char install_directory[UMI_BUILD_PATH_CAPACITY];
    char toolchain_file[UMI_BUILD_PATH_CAPACITY];
    unsigned present;
} UmiCMakeCache;
/* Parse ordinary selected CMAKE_* entries. Unknown entries/lines are ignored.
 * Accept LF/CRLF, initial UTF-8 BOM, quoted keys and literal values. Reject
 * invalid UTF-8/NUL, duplicate selected keys, unsupported types and control
 * bytes in selected values. No field is required here: consumers enforce
 * their own identity requirements. Output changes only on full success.
 * Input and output storage must be disjoint and stable for the call. */
UmiStatus UmiCMakeCacheParse(const void *bytes, size_t size, UmiCMakeCache *out);
/* Read one absolute regular file through the bounded input service. The final
 * component cannot be a link/device; trusted parent aliases may be followed.
 * This blocking read belongs on a worker in a UI. No writes or tools run. */
UmiStatus UmiCMakeCacheRead(const char *path, UmiCMakeCache *out);
#ifdef __cplusplus
}
#endif
#endif
