/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/constant_folding.h
 *
 * PURPOSE:
 *   Fold integer IR operations whose operands are constants defined in the same function.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_CONSTANT_FOLDING_H
#define UMICOM_COMPILER_NATIVE_CONSTANT_FOLDING_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_function.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the nc constant fold function operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_constant_fold_function(UmiNativeIrFunction *function,size_t *out_folded);
#ifdef __cplusplus
}
#endif
#endif
