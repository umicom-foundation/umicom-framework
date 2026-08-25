/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/machine_operand.c
 *
 * PURPOSE:
 *   Represent machine registers, immediates, symbols and frame references in target-neutral form.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/machine_operand.h"
#include <string.h>
UmiNativeMachineOperand umi_nc_machine_operand_register(uint32_t id){UmiNativeMachineOperand o;memset(&o,0,sizeof(o));o.kind=UMI_NC_MOP_REGISTER;o.register_id=id;return o;}
UmiNativeMachineOperand umi_nc_machine_operand_immediate(int64_t v){UmiNativeMachineOperand o;memset(&o,0,sizeof(o));o.kind=UMI_NC_MOP_IMMEDIATE;o.immediate=v;return o;}
UmiNativeMachineOperand umi_nc_machine_operand_block(uint32_t id){UmiNativeMachineOperand o;memset(&o,0,sizeof(o));o.kind=UMI_NC_MOP_BLOCK;o.block_id=id;return o;}
UmiStatus umi_nc_machine_operand_symbol(UmiNativeMachineOperand *o,const char *s){if(o==NULL||s==NULL||s[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));o->kind=UMI_NC_MOP_SYMBOL;return umi_nc_copy_text(o->symbol,sizeof(o->symbol),s);}
