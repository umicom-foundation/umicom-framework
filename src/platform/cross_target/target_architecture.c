/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/target_architecture.c
 *
 * PURPOSE:
 *   Parse and describe CPU architecture identities without duplicating compiler discovery.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/target_architecture.h"

#include <string.h>
UmiStatus umi_ct_architecture_parse(const char *text, UmiCtArchitecture *out_architecture) {
    if (text == NULL || out_architecture == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(text,"x86_64")==0 || strcmp(text,"amd64")==0) *out_architecture=UMI_CT_ARCH_X86_64;
    else if (strcmp(text,"aarch64")==0 || strcmp(text,"arm64")==0) *out_architecture=UMI_CT_ARCH_ARM64;
    else if (strcmp(text,"riscv32")==0 || strcmp(text,"rv32")==0) *out_architecture=UMI_CT_ARCH_RISCV32;
    else if (strcmp(text,"riscv64")==0 || strcmp(text,"rv64")==0) *out_architecture=UMI_CT_ARCH_RISCV64;
    else return UMI_STATUS_NOT_FOUND;
    return UMI_STATUS_OK;
}
const char *umi_ct_architecture_text(UmiCtArchitecture architecture) {
    switch (architecture) { case UMI_CT_ARCH_X86_64:return "x86_64"; case UMI_CT_ARCH_ARM64:return "aarch64"; case UMI_CT_ARCH_RISCV32:return "riscv32"; case UMI_CT_ARCH_RISCV64:return "riscv64"; default:return "unknown"; }
}
uint32_t umi_ct_architecture_pointer_bits(UmiCtArchitecture architecture) { return architecture==UMI_CT_ARCH_RISCV32 ? 32U : (architecture==UMI_CT_ARCH_UNKNOWN ? 0U : 64U); }
bool umi_ct_architecture_is_riscv(UmiCtArchitecture architecture) { return architecture==UMI_CT_ARCH_RISCV32 || architecture==UMI_CT_ARCH_RISCV64; }
