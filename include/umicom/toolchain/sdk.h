/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/sdk.h
 * PURPOSE: Describe SDK roots associated with validated compiler profiles.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_SDK_H
#define UMICOM_TOOLCHAIN_SDK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_SDK_API_VERSION 1U
#define UMI_TOOLCHAIN_SDK_CAPACITY 8U

typedef enum UmiSdkKind {
    UMI_SDK_UNKNOWN = 0,
    UMI_SDK_MSYS2_UCRT64 = 1,
    UMI_SDK_MSYS2_MINGW64 = 2,
    UMI_SDK_WINDOWS = 3,
    UMI_SDK_MSVC = 4,
    UMI_SDK_POSIX = 5
} UmiSdkKind;

typedef struct UmiSdkSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char name[256];
    char version[128];
    char root[UMI_TOOL_PATH_CAPACITY];
    char include_directory[UMI_TOOL_PATH_CAPACITY];
    char library_directory[UMI_TOOL_PATH_CAPACITY];
    UmiSdkKind kind;
    int available;
} UmiSdkSnapshot;

typedef struct UmiSdkCatalogueSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiSdkSnapshot items[UMI_TOOLCHAIN_SDK_CAPACITY];
    size_t count;
} UmiSdkCatalogueSnapshot;

UmiStatus umi_toolchain_sdk_discover(
    const UmiToolchainProfile *profile,
    UmiSdkCatalogueSnapshot *out_catalogue);
const char *umi_sdk_kind_text(UmiSdkKind kind);

#ifdef __cplusplus
}
#endif
#endif
