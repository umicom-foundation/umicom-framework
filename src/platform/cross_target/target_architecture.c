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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/target_architecture.h"

#include <string.h>
/*
 * Read ct architecture into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_ct_architecture_parse(const char *text, UmiCtArchitecture *out_architecture) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_architecture == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text,"x86_64")==0 || strcmp(text,"amd64")==0) *out_architecture=UMI_CT_ARCH_X86_64;
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text,"aarch64")==0 || strcmp(text,"arm64")==0) *out_architecture=UMI_CT_ARCH_ARM64;
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text,"riscv32")==0 || strcmp(text,"rv32")==0) *out_architecture=UMI_CT_ARCH_RISCV32;
    else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(text,"riscv64")==0 || strcmp(text,"rv64")==0) *out_architecture=UMI_CT_ARCH_RISCV64;
    /* Use this fallback path when the earlier condition does not apply. */
    else return UMI_STATUS_NOT_FOUND;
    return UMI_STATUS_OK;
}
/*
 * Provide the ct architecture text operation used by this module and its client
 * applications.
 */
const char *umi_ct_architecture_text(UmiCtArchitecture architecture) {
    /* Select the behaviour associated with the requested command or state value. */
    switch (architecture) { case UMI_CT_ARCH_X86_64:return "x86_64"; case UMI_CT_ARCH_ARM64:return "aarch64"; case UMI_CT_ARCH_RISCV32:return "riscv32"; case UMI_CT_ARCH_RISCV64:return "riscv64"; default:return "unknown"; }
}
/*
 * Provide the ct architecture pointer bits operation used by this module and its client
 * applications.
 */
uint32_t umi_ct_architecture_pointer_bits(UmiCtArchitecture architecture) { return architecture==UMI_CT_ARCH_RISCV32 ? 32U : (architecture==UMI_CT_ARCH_UNKNOWN ? 0U : 64U); }
/*
 * Provide the ct architecture is riscv operation used by this module and its client
 * applications.
 */
bool umi_ct_architecture_is_riscv(UmiCtArchitecture architecture) { return architecture==UMI_CT_ARCH_RISCV32 || architecture==UMI_CT_ARCH_RISCV64; }
