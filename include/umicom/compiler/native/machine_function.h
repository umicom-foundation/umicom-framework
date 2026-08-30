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
typedef struct UmiNativeMachineFunction { char name[UMI_NC_NAME_CAPACITY]; UmiNativeMachineArch architecture; UmiNativeMachineBlock blocks[UMI_NC_MAX_MACHINE_BLOCKS]; size_t block_count; size_t stack_size; size_t stack_alignment; uint32_t next_instruction_id; } UmiNativeMachineFunction;
UmiStatus umi_nc_machine_function_init(UmiNativeMachineFunction *function,const char *name,UmiNativeMachineArch architecture);
UmiStatus umi_nc_machine_function_add_block(UmiNativeMachineFunction *function,uint32_t id,const char *label);
UmiNativeMachineBlock *umi_nc_machine_function_block(UmiNativeMachineFunction *function,uint32_t id);
uint32_t umi_nc_machine_function_next_instruction(UmiNativeMachineFunction *function);
#ifdef __cplusplus
}
#endif
#endif
