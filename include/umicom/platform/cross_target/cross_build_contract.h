/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/cross_build_contract.h
 *
 * PURPOSE:
 *   Declare cross-build requirements while leaving actual compiler/tool discovery to the existing Toolchain subsystem.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_CROSS_BUILD_CONTRACT_H
#define UMICOM_PLATFORM_CROSS_TARGET_CROSS_BUILD_CONTRACT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtCrossBuildContract { char contract_id[UMI_CT_ID_CAPACITY]; UmiCtTarget target; char required_toolchain_family[32]; char required_abi[32]; bool require_sysroot; bool require_emulator; bool require_debugger; bool require_assembly; } UmiCtCrossBuildContract;
UmiStatus umi_ct_cross_build_contract_validate(const UmiCtCrossBuildContract *contract);

#ifdef __cplusplus
}
#endif

#endif
