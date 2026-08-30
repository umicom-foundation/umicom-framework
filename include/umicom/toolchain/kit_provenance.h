/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/kit_provenance.h
 * PURPOSE: Capture reproducible, inspectable developer-kit provenance evidence.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_KIT_PROVENANCE_H
#define UMICOM_TOOLCHAIN_KIT_PROVENANCE_H

#include <stdint.h>

#include "umicom/toolchain/kit.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_KIT_PROVENANCE_API_VERSION 1U
#define UMI_TOOLCHAIN_KIT_PROVENANCE_FINGERPRINT_CAPACITY 32U

typedef struct UmiToolchainKitProvenanceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char kit_id[UMI_TOOLCHAIN_KIT_ID_CAPACITY];
    char profile_id[UMI_TOOLCHAIN_TEXT_CAPACITY];
    char sdk_id[128];
    char target_triple[UMI_TOOLCHAIN_KIT_TRIPLE_CAPACITY];
    char generator[UMI_TOOLCHAIN_KIT_GENERATOR_CAPACITY];
    char c_compiler[UMI_TOOL_PATH_CAPACITY];
    char c_compiler_version[UMI_TOOL_VERSION_CAPACITY];
    char cpp_compiler[UMI_TOOL_PATH_CAPACITY];
    char debugger[UMI_TOOL_PATH_CAPACITY];
    char sdk_root[UMI_TOOL_PATH_CAPACITY];
    char sdk_version[128];
    char sysroot[UMI_TOOL_PATH_CAPACITY];
    char fingerprint[UMI_TOOLCHAIN_KIT_PROVENANCE_FINGERPRINT_CAPACITY];
    uint64_t revision;
} UmiToolchainKitProvenanceSnapshot;

UmiStatus umi_toolchain_kit_provenance_capture(
    const UmiToolchainKitSnapshot *kit,
    const UmiToolchainProfile *profile,
    const UmiSdkSnapshot *sdk,
    UmiToolchainKitProvenanceSnapshot *out_provenance);
UmiStatus umi_toolchain_kit_provenance_fingerprint(
    UmiToolchainKitProvenanceSnapshot *provenance);

#ifdef __cplusplus
}
#endif
#endif
