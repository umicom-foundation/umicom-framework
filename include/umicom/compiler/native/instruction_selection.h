/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/instruction_selection.h
 *
 * PURPOSE:
 *   Lower a practical IR subset into target-neutral machine instructions while reporting unsupported operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiNativeSelectionReport { size_t lowered; size_t unsupported; size_t blocks; } UmiNativeSelectionReport;
UmiStatus umi_nc_instruction_select(const UmiNativeIrFunction *ir,UmiNativeMachineArch architecture,UmiNativeMachineFunction *out_machine,UmiNativeSelectionReport *out_report);
#ifdef __cplusplus
}
#endif
#endif
