/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/cross_toolchain.h
 *
 * PURPOSE:
 *   Discover target-prefixed GNU and Clang cross-development tools without
 *   leaking cross-compiler naming rules into Studio or another application.
 *
 * ARCHITECTURE:
 *   Framework owns cross-target discovery. Applications select a validated kit
 *   and never construct compiler, assembler, linker or debugger paths directly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_CROSS_TOOLCHAIN_H
#define UMICOM_TOOLCHAIN_CROSS_TOOLCHAIN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/toolchain/tool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CROSS_TOOLCHAIN_API_VERSION 1U
#define UMI_CROSS_TOOLCHAIN_ID_CAPACITY 128U
#define UMI_CROSS_TOOLCHAIN_TRIPLE_CAPACITY 128U
#define UMI_CROSS_TOOLCHAIN_EXECUTABLE_CAPACITY 256U
#define UMI_CROSS_TOOLCHAIN_VERSION_CAPACITY 256U

typedef enum UmiCrossToolchainFamily {
    UMI_CROSS_TOOLCHAIN_UNKNOWN = 0,
    UMI_CROSS_TOOLCHAIN_GNU = 1,
    UMI_CROSS_TOOLCHAIN_CLANG = 2
} UmiCrossToolchainFamily;

typedef enum UmiCrossToolchainState {
    UMI_CROSS_TOOLCHAIN_UNAVAILABLE = 0,
    UMI_CROSS_TOOLCHAIN_PARTIAL = 1,
    UMI_CROSS_TOOLCHAIN_READY = 2
} UmiCrossToolchainState;

typedef struct UmiCrossToolchainRequest {
    uint32_t struct_size;
    uint32_t api_version;
    UmiCrossToolchainFamily family;
    const char *target_triple;
    const char *tool_prefix;
    const char *explicit_root;
    const char *compiler_override;
    const char *cpp_compiler_override;
    const char *assembler_override;
    const char *linker_override;
    const char *debugger_override;
    int require_cpp;
    int require_debugger;
    int validate_versions;
} UmiCrossToolchainRequest;

typedef struct UmiCrossToolchainPlan {
    uint32_t struct_size;
    uint32_t api_version;
    char target_triple[UMI_CROSS_TOOLCHAIN_TRIPLE_CAPACITY];
    char tool_prefix[UMI_CROSS_TOOLCHAIN_EXECUTABLE_CAPACITY];
    char compiler[UMI_CROSS_TOOLCHAIN_EXECUTABLE_CAPACITY];
    char cpp_compiler[UMI_CROSS_TOOLCHAIN_EXECUTABLE_CAPACITY];
    char assembler[UMI_CROSS_TOOLCHAIN_EXECUTABLE_CAPACITY];
    char linker[UMI_CROSS_TOOLCHAIN_EXECUTABLE_CAPACITY];
    char debugger[UMI_CROSS_TOOLCHAIN_EXECUTABLE_CAPACITY];
    char target_argument[UMI_CROSS_TOOLCHAIN_EXECUTABLE_CAPACITY];
    UmiCrossToolchainFamily family;
} UmiCrossToolchainPlan;

typedef struct UmiCrossToolchainSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_CROSS_TOOLCHAIN_ID_CAPACITY];
    char target_triple[UMI_CROSS_TOOLCHAIN_TRIPLE_CAPACITY];
    char tool_prefix[UMI_CROSS_TOOLCHAIN_EXECUTABLE_CAPACITY];
    char compiler_path[UMI_TOOL_PATH_CAPACITY];
    char cpp_compiler_path[UMI_TOOL_PATH_CAPACITY];
    char assembler_path[UMI_TOOL_PATH_CAPACITY];
    char linker_path[UMI_TOOL_PATH_CAPACITY];
    char debugger_path[UMI_TOOL_PATH_CAPACITY];
    char sysroot[UMI_TOOL_PATH_CAPACITY];
    char compiler_version[UMI_CROSS_TOOLCHAIN_VERSION_CAPACITY];
    char debugger_version[UMI_CROSS_TOOLCHAIN_VERSION_CAPACITY];
    char target_argument[UMI_CROSS_TOOLCHAIN_EXECUTABLE_CAPACITY];
    UmiCrossToolchainFamily family;
    UmiCrossToolchainState state;
    size_t tools_found;
    size_t required_tools;
    size_t required_tools_missing;
} UmiCrossToolchainSnapshot;

void umi_cross_toolchain_request_init(UmiCrossToolchainRequest *request);
UmiStatus umi_cross_toolchain_plan(const UmiCrossToolchainRequest *request,
                                   UmiCrossToolchainPlan *out_plan);
UmiStatus umi_cross_toolchain_discover(const UmiCrossToolchainRequest *request,
                                       UmiCrossToolchainSnapshot *out_snapshot);
const char *umi_cross_toolchain_family_text(UmiCrossToolchainFamily family);
const char *umi_cross_toolchain_state_text(UmiCrossToolchainState state);

#ifdef __cplusplus
}
#endif
#endif
