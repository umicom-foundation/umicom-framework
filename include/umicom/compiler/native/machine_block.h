/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/machine_block.h
 *
 * PURPOSE:
 *   Store selected machine instructions under stable block labels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_MACHINE_BLOCK_H
#define UMICOM_COMPILER_NATIVE_MACHINE_BLOCK_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/machine_instruction.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiNativeMachineBlock { uint32_t id; char label[UMI_NC_NAME_CAPACITY]; UmiNativeMachineInstruction instructions[UMI_NC_MAX_MACHINE_INSTRUCTIONS]; size_t instruction_count; } UmiNativeMachineBlock;
UmiStatus umi_nc_machine_block_init(UmiNativeMachineBlock *block,uint32_t id,const char *label);
UmiStatus umi_nc_machine_block_append(UmiNativeMachineBlock *block,const UmiNativeMachineInstruction *instruction);
#ifdef __cplusplus
}
#endif
#endif
