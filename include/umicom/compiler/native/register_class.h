/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/register_class.h
 *
 * PURPOSE:
 *   Describe target register classes and volatile/callee-saved availability for future allocation passes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_REGISTER_CLASS_H
#define UMICOM_COMPILER_NATIVE_REGISTER_CLASS_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/target_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native register class kind values accepted by this public contract.
 */
typedef enum UmiNativeRegisterClassKind { UMI_NC_REG_GPR=1, UMI_NC_REG_FPR=2, UMI_NC_REG_VECTOR=3 } UmiNativeRegisterClassKind;
/**
 * Represent the native register class data shared with callers of this public contract.
 */
typedef struct UmiNativeRegisterClass { UmiNativeRegisterClassKind kind; size_t count; uint32_t width_bits; uint64_t caller_saved_mask; uint64_t callee_saved_mask; } UmiNativeRegisterClass;
/**
 * Provide the nc register class default operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_register_class_default(UmiNativeMachineArch architecture,UmiNativeRegisterClassKind kind,UmiNativeRegisterClass *out_class);
/**
 * Provide the nc register class usable operation used by this module and its client
 * applications.
 */
size_t umi_nc_register_class_usable(const UmiNativeRegisterClass *register_class);
#ifdef __cplusplus
}
#endif
#endif
