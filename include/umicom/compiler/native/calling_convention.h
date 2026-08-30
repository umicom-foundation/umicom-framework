/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/calling_convention.h
 *
 * PURPOSE:
 *   Describe register argument, return and stack alignment rules for supported native ABIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_CALLING_CONVENTION_H
#define UMICOM_COMPILER_NATIVE_CALLING_CONVENTION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/target_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_ARGUMENT_REGISTERS 8U
typedef struct UmiNativeCallingConvention { char name[UMI_NC_NAME_CAPACITY]; uint32_t integer_argument_registers[UMI_NC_MAX_ARGUMENT_REGISTERS]; size_t integer_argument_count; uint32_t return_register; size_t stack_alignment; bool caller_cleans_stack; } UmiNativeCallingConvention;
UmiStatus umi_nc_calling_convention_default(const UmiNativeTargetProfile *target,UmiNativeCallingConvention *out_convention);
#ifdef __cplusplus
}
#endif
#endif
