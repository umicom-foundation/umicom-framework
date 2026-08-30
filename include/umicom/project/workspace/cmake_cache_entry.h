/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/cmake_cache_entry.h
 *
 * PURPOSE:
 *   Publish the public cmake cache entry contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_CACHE_ENTRY_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_CACHE_ENTRY_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceCmakeCacheEntry {
        char key[UMI_PROJECT_WORKSPACE_NAME_CAPACITY];
        char type[64];
        char value[UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
    }
    UmiProjectWorkspaceCmakeCacheEntry;
    UmiStatus umi_project_workspace_cmake_cache_entry_parse(const char*line,UmiProjectWorkspaceCmakeCacheEntry*out);
#ifdef __cplusplus
}
#endif
#endif
