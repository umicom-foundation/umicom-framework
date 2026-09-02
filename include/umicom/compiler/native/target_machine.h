/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/target_machine.h
 *
 * PURPOSE:
 *   Combine target and optimization profiles into a validated native code-generation machine contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_TARGET_MACHINE_H
#define UMICOM_COMPILER_NATIVE_TARGET_MACHINE_H
#include <stdbool.h>
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/optimization_profile.h"
#include "umicom/compiler/native/target_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the native target machine data shared with callers of this public contract.
 */
typedef struct UmiNativeTargetMachine { UmiNativeTargetProfile target; UmiNativeOptimizationProfile optimization; size_t integer_registers; size_t floating_registers; bool supports_division; bool supports_vectors; } UmiNativeTargetMachine;
/**
 * Initialise nc target machine from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_target_machine_init(UmiNativeTargetMachine *machine,const UmiNativeTargetProfile *target,const UmiNativeOptimizationProfile *optimization);
/**
 * Provide the nc target machine supports opcode operation used by this module and its
 * client applications.
 */
bool umi_nc_target_machine_supports_opcode(const UmiNativeTargetMachine *machine,UmiNativeIrOpcode opcode);
#ifdef __cplusplus
}
#endif
#endif
