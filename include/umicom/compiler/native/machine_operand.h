/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/machine_operand.h
 *
 * PURPOSE:
 *   Represent machine registers, immediates, symbols and frame references in target-neutral form.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_MACHINE_OPERAND_H
#define UMICOM_COMPILER_NATIVE_MACHINE_OPERAND_H
#include <stdint.h>
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiNativeMachineOperandKind { UMI_NC_MOP_NONE=0, UMI_NC_MOP_REGISTER=1, UMI_NC_MOP_IMMEDIATE=2, UMI_NC_MOP_SYMBOL=3, UMI_NC_MOP_FRAME=4, UMI_NC_MOP_BLOCK=5 } UmiNativeMachineOperandKind;
typedef struct UmiNativeMachineOperand { UmiNativeMachineOperandKind kind; uint32_t register_id; int64_t immediate; int32_t frame_offset; uint32_t block_id; char symbol[UMI_NC_NAME_CAPACITY]; } UmiNativeMachineOperand;
UmiNativeMachineOperand umi_nc_machine_operand_register(uint32_t id);
UmiNativeMachineOperand umi_nc_machine_operand_immediate(int64_t value);
UmiNativeMachineOperand umi_nc_machine_operand_block(uint32_t id);
UmiStatus umi_nc_machine_operand_symbol(UmiNativeMachineOperand *operand,const char *symbol);
#ifdef __cplusplus
}
#endif
#endif
