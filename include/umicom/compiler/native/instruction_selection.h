/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/instruction_selection.h
 *
 * PURPOSE:
 *   Lower a practical IR subset into target-neutral machine instructions while reporting unsupported operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_INSTRUCTION_SELECTION_H
#define UMICOM_COMPILER_NATIVE_INSTRUCTION_SELECTION_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_function.h"
#include "umicom/compiler/native/machine_function.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the native selection report data shared with callers of this public contract.
 */
typedef struct UmiNativeSelectionReport { size_t lowered; size_t unsupported; size_t blocks; } UmiNativeSelectionReport;
/**
 * Provide the nc instruction select operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_instruction_select(const UmiNativeIrFunction *ir,UmiNativeMachineArch architecture,UmiNativeMachineFunction *out_machine,UmiNativeSelectionReport *out_report);
#ifdef __cplusplus
}
#endif
#endif
