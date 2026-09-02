/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/branch_simplify.h
 *
 * PURPOSE:
 *   Simplify conditional branches whose condition is a compile-time IR constant.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_BRANCH_SIMPLIFY_H
#define UMICOM_COMPILER_NATIVE_BRANCH_SIMPLIFY_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_function.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the nc branch simplify operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_branch_simplify(UmiNativeIrFunction *function,size_t *out_simplified);
#ifdef __cplusplus
}
#endif
#endif
