/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/ir_value.h
 *
 * PURPOSE:
 *   Represent SSA-style IR values, constants, parameters and temporaries using stable value IDs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_IR_VALUE_H
#define UMICOM_COMPILER_NATIVE_IR_VALUE_H
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/constant_value.h"
#include "umicom/compiler/native/ir_type.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiNativeIrValueKind { UMI_NC_IR_VALUE_TEMP=1, UMI_NC_IR_VALUE_CONSTANT=2, UMI_NC_IR_VALUE_PARAMETER=3, UMI_NC_IR_VALUE_GLOBAL=4 } UmiNativeIrValueKind;
typedef struct UmiNativeIrValue { uint32_t id; UmiNativeIrValueKind kind; UmiNativeIrType type; UmiNativeConstantValue constant; } UmiNativeIrValue;
UmiStatus umi_nc_ir_value_init(UmiNativeIrValue *value,uint32_t id,UmiNativeIrValueKind kind,UmiNativeIrType type);
UmiStatus umi_nc_ir_value_constant(UmiNativeIrValue *value,uint32_t id,UmiNativeIrType type,UmiNativeConstantValue constant);
#ifdef __cplusplus
}
#endif
#endif
