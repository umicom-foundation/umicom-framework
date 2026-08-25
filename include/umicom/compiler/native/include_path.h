/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/include_path.h
 *
 * PURPOSE:
 *   Manage portable user/system include roots and construct normalized include candidates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_INCLUDE_PATH_H
#define UMICOM_COMPILER_NATIVE_INCLUDE_PATH_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_INCLUDE_PATHS 64U
typedef struct UmiNativeIncludeRoot { char path[UMI_NC_PATH_CAPACITY]; bool system; } UmiNativeIncludeRoot;
typedef struct UmiNativeIncludePathSet { UmiNativeIncludeRoot items[UMI_NC_MAX_INCLUDE_PATHS]; size_t count; uint64_t revision; } UmiNativeIncludePathSet;
void umi_nc_include_paths_init(UmiNativeIncludePathSet *paths);
UmiStatus umi_nc_include_paths_add(UmiNativeIncludePathSet *paths,const char *path,bool system);
UmiStatus umi_nc_include_candidate(const UmiNativeIncludeRoot *root,const char *header,char *out_path,size_t out_capacity);
#ifdef __cplusplus
}
#endif
#endif
