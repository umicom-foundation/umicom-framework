/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/ir_verifier.h
 *
 * PURPOSE:
 *   Validate IR block termination, branch targets, instruction IDs and value-definition uniqueness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_IR_VERIFIER_H
#define UMICOM_COMPILER_NATIVE_IR_VERIFIER_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_function.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the native ir verify report data shared with callers of this public contract.
 */
typedef struct UmiNativeIrVerifyReport { size_t blocks; size_t instructions; size_t errors; size_t unterminated_blocks; size_t missing_targets; size_t duplicate_values; } UmiNativeIrVerifyReport;
/**
 * Provide the nc ir verify function operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_ir_verify_function(const UmiNativeIrFunction *function,UmiNativeIrVerifyReport *out_report);
#ifdef __cplusplus
}
#endif
#endif
