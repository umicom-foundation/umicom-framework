/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/ir_type.c
 *
 * PURPOSE:
 *   Describe compact native intermediate-representation scalar and pointer types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_type.h"
UmiNativeIrType umi_nc_ir_type_make(UmiNativeIrTypeKind k,uint16_t bits,uint16_t lanes){UmiNativeIrType t={k,bits,lanes==0U?1U:lanes};return t;}
size_t umi_nc_ir_type_size(const UmiNativeIrType *t,size_t pointer_size){if(t==NULL)return 0U;if(t->kind==UMI_NC_IR_VOID)return 0U;if(t->kind==UMI_NC_IR_PTR)return pointer_size*(size_t)t->lanes;return (((size_t)t->bits+7U)/8U)*(size_t)t->lanes;}
bool umi_nc_ir_type_equal(const UmiNativeIrType *a,const UmiNativeIrType *b){return a!=NULL&&b!=NULL&&a->kind==b->kind&&a->bits==b->bits&&a->lanes==b->lanes;}
bool umi_nc_ir_type_is_integer(const UmiNativeIrType *t){return t!=NULL&&(t->kind==UMI_NC_IR_I1||t->kind==UMI_NC_IR_I8||t->kind==UMI_NC_IR_I16||t->kind==UMI_NC_IR_I32||t->kind==UMI_NC_IR_I64);}
