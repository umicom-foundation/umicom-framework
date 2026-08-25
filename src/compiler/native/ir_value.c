/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/ir_value.c
 *
 * PURPOSE:
 *   Represent SSA-style IR values, constants, parameters and temporaries using stable value IDs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/ir_value.h"
#include <string.h>
UmiStatus umi_nc_ir_value_init(UmiNativeIrValue *v,uint32_t id,UmiNativeIrValueKind kind,UmiNativeIrType type){if(v==NULL||id==0U||kind<UMI_NC_IR_VALUE_TEMP||kind>UMI_NC_IR_VALUE_GLOBAL)return UMI_STATUS_INVALID_ARGUMENT;memset(v,0,sizeof(*v));v->id=id;v->kind=kind;v->type=type;return UMI_STATUS_OK;}
UmiStatus umi_nc_ir_value_constant(UmiNativeIrValue *v,uint32_t id,UmiNativeIrType type,UmiNativeConstantValue c){UmiStatus st=umi_nc_ir_value_init(v,id,UMI_NC_IR_VALUE_CONSTANT,type);if(st!=UMI_STATUS_OK)return st;v->constant=c;return UMI_STATUS_OK;}
