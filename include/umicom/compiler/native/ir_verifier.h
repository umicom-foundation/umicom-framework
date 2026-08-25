/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/ir_verifier.h
 *
 * PURPOSE:
 *   Validate IR block termination, branch targets, instruction IDs and value-definition uniqueness.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_IR_VERIFIER_H
#define UMICOM_COMPILER_NATIVE_IR_VERIFIER_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_function.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiNativeIrVerifyReport { size_t blocks; size_t instructions; size_t errors; size_t unterminated_blocks; size_t missing_targets; size_t duplicate_values; } UmiNativeIrVerifyReport;
UmiStatus umi_nc_ir_verify_function(const UmiNativeIrFunction *function,UmiNativeIrVerifyReport *out_report);
#ifdef __cplusplus
}
#endif
#endif
