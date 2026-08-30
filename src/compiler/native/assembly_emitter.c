/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/assembly_emitter.c
 *
 * PURPOSE:
 *   Emit deterministic textual assembly from selected machine functions for bootstrap inspection and external assemblers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/assembly_emitter.h"
#include "umicom/compiler/native/riscv64_lowering.h"
#include "umicom/compiler/native/x86_64_lowering.h"
#include <stdio.h>
#include <string.h>
static UmiStatus append(UmiNativeAssemblyBuffer *b,const char *s){size_t n=strlen(s);if(b->length+n>=sizeof(b->text))return UMI_STATUS_CAPACITY_EXCEEDED;memcpy(b->text+b->length,s,n+1U);b->length+=n;return UMI_STATUS_OK;}
void umi_nc_assembly_buffer_init(UmiNativeAssemblyBuffer *b){if(b!=NULL){b->length=0U;b->text[0]='\0';}}
static UmiStatus emit_operand(UmiNativeAssemblyBuffer *b,const UmiNativeMachineOperand *o){char x[128];int n=0;switch(o->kind){case UMI_NC_MOP_REGISTER:n=snprintf(x,sizeof(x),"r%u",o->register_id);break;case UMI_NC_MOP_IMMEDIATE:n=snprintf(x,sizeof(x),"%lld",(long long)o->immediate);break;case UMI_NC_MOP_BLOCK:n=snprintf(x,sizeof(x),".L%u",o->block_id);break;case UMI_NC_MOP_SYMBOL:n=snprintf(x,sizeof(x),"%s",o->symbol);break;default:n=snprintf(x,sizeof(x),"?");break;}if(n<0||(size_t)n>=sizeof(x))return UMI_STATUS_CAPACITY_EXCEEDED;return append(b,x);}
UmiStatus umi_nc_assembly_emit_function(const UmiNativeMachineFunction *f,UmiNativeAssemblyBuffer *b){if(f==NULL||b==NULL)return UMI_STATUS_INVALID_ARGUMENT;umi_nc_assembly_buffer_init(b);if(append(b,".text\n.globl ")!=UMI_STATUS_OK||append(b,f->name)!=UMI_STATUS_OK||append(b,"\n")!=UMI_STATUS_OK||append(b,f->name)!=UMI_STATUS_OK||append(b,":\n")!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;for(size_t bi=0U;bi<f->block_count;bi++){char label[64];int n=snprintf(label,sizeof(label),".L%u:\n",f->blocks[bi].id);if(n<0||(size_t)n>=sizeof(label)||append(b,label)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;for(size_t ii=0U;ii<f->blocks[bi].instruction_count;ii++){const UmiNativeMachineInstruction *in=&f->blocks[bi].instructions[ii];const char *mn=f->architecture==UMI_NC_ARCH_RISCV64?umi_nc_riscv64_mnemonic(in->opcode):umi_nc_x86_64_mnemonic(in->opcode);if(append(b,"    ")!=UMI_STATUS_OK||append(b,mn)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;for(size_t oi=0U;oi<in->operand_count;oi++){if(append(b,oi==0U?" ":", ")!=UMI_STATUS_OK||emit_operand(b,&in->operands[oi])!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;}if(append(b,"\n")!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;}}return UMI_STATUS_OK;}
