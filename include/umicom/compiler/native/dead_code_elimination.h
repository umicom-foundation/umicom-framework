/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/dead_code_elimination.h
 *
 * PURPOSE:
 *   Remove unused side-effect-free IR instructions while retaining terminators and observable operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_DEAD_CODE_ELIMINATION_H
#define UMICOM_COMPILER_NATIVE_DEAD_CODE_ELIMINATION_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_function.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_nc_dead_code_eliminate(UmiNativeIrFunction *function,size_t *out_removed);
#ifdef __cplusplus
}
#endif
#endif
