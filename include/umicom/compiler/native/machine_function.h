/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/machine_function.h
 *
 * PURPOSE:
 *   Aggregate machine blocks, frame size and target architecture for one lowered function.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_MACHINE_FUNCTION_H
#define UMICOM_COMPILER_NATIVE_MACHINE_FUNCTION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/machine_block.h"
#include "umicom/compiler/native/target_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the native machine function data shared with callers of this public contract.
 */
typedef struct UmiNativeMachineFunction { char name[UMI_NC_NAME_CAPACITY]; UmiNativeMachineArch architecture; UmiNativeMachineBlock blocks[UMI_NC_MAX_MACHINE_BLOCKS]; size_t block_count; size_t stack_size; size_t stack_alignment; uint32_t next_instruction_id; } UmiNativeMachineFunction;
/**
 * Initialise nc machine function from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_machine_function_init(UmiNativeMachineFunction *function,const char *name,UmiNativeMachineArch architecture);
/**
 * Provide the nc machine function add block operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_machine_function_add_block(UmiNativeMachineFunction *function,uint32_t id,const char *label);
/**
 * Provide the nc machine function block operation used by this module and its client
 * applications.
 */
UmiNativeMachineBlock *umi_nc_machine_function_block(UmiNativeMachineFunction *function,uint32_t id);
/**
 * Provide the nc machine function next instruction operation used by this module and its
 * client applications.
 */
uint32_t umi_nc_machine_function_next_instruction(UmiNativeMachineFunction *function);
#ifdef __cplusplus
}
#endif
#endif
