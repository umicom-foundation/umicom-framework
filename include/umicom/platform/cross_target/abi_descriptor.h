/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/abi_descriptor.h
 *
 * PURPOSE:
 *   Describe data model, calling convention, stack alignment and floating-point ABI properties for cross-target compatibility checks.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_ABI_DESCRIPTOR_H
#define UMICOM_PLATFORM_CROSS_TARGET_ABI_DESCRIPTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiCtDataModel { UMI_CT_DATA_ILP32=1, UMI_CT_DATA_LP64=2, UMI_CT_DATA_LLP64=3 } UmiCtDataModel;
typedef enum UmiCtCallingConvention { UMI_CT_CALL_C=1, UMI_CT_CALL_SYSV=2, UMI_CT_CALL_WIN64=3, UMI_CT_CALL_RISCV=4, UMI_CT_CALL_UMICOM=5 } UmiCtCallingConvention;
typedef struct UmiCtAbiDescriptor { char abi_id[32]; UmiCtDataModel data_model; UmiCtCallingConvention calling_convention; uint32_t pointer_bits; uint32_t stack_alignment; uint32_t long_double_bits; bool hard_float; } UmiCtAbiDescriptor;
UmiStatus umi_ct_abi_descriptor_validate(const UmiCtAbiDescriptor *descriptor);

#ifdef __cplusplus
}
#endif

#endif
