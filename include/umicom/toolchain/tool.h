/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/tool.h
 *
 * PURPOSE:
 *   Define native compiler, build, package, source-control, documentation, and
 *   runtime tools that may be discovered and validated by Umicom Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_TOOL_H
#define UMICOM_TOOLCHAIN_TOOL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOL_PATH_CAPACITY 2048U
#define UMI_TOOL_VERSION_CAPACITY 256U

typedef enum UmiToolKind {
    UMI_TOOL_CLANG = 0,
    UMI_TOOL_CLANGXX,
    UMI_TOOL_GCC,
    UMI_TOOL_GXX,
    UMI_TOOL_CMAKE,
    UMI_TOOL_CTEST,
    UMI_TOOL_NINJA,
    UMI_TOOL_PKG_CONFIG,
    UMI_TOOL_WINDRES,
    UMI_TOOL_GIT,
    UMI_TOOL_GH,
    UMI_TOOL_GDB,
    UMI_TOOL_CLANG_FORMAT,
    UMI_TOOL_CLANG_TIDY,
    UMI_TOOL_DOXYGEN,
    UMI_TOOL_DOT,
    UMI_TOOL_JAVA,
    UMI_TOOL_JAVAC,
    /* Batch 35 appends Microsoft discovery identities without changing the
     * numeric values of any established public tool kind. */
    UMI_TOOL_MSVC_CL,
    UMI_TOOL_MSVC_LINK,
    UMI_TOOL_VSWHERE,
    UMI_TOOL_COUNT
} UmiToolKind;

typedef enum UmiToolState {
    UMI_TOOL_MISSING = 0,
    UMI_TOOL_FOUND = 1,
    UMI_TOOL_VALIDATED = 2,
    UMI_TOOL_FAILED = 3
} UmiToolState;

typedef struct UmiToolInfo {
    UmiToolKind kind;
    UmiToolState state;
    int required;
    char path[UMI_TOOL_PATH_CAPACITY];
    char version[UMI_TOOL_VERSION_CAPACITY];
} UmiToolInfo;

const char *umi_tool_kind_name(UmiToolKind kind);
const char *umi_tool_default_executable(UmiToolKind kind);
const char *umi_tool_state_text(UmiToolState state);

#ifdef __cplusplus
}
#endif

#endif
